// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <sstream>

#include "udpipe_service.h"
#include "unilib/unicode.h"
#include "unilib/uninorms.h"
#include "unilib/utf8.h"
#include "utils/path_from_utf8.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {

// Init the UDPipe service -- load the models
bool udpipe_service::init(const service_options& options) {
  if (options.model_descriptions.empty()) return false;

  // Init models
  models.clear();
  models_map.clear();
  models.reserve(options.model_descriptions.size());
  for (auto& model_description : options.model_descriptions) {
    unique_ptr<ifstream> is(new ifstream(path_from_utf8(model_description.file).c_str(), ifstream::binary));
    if (!is->is_open()) return false;

    // Split the ids
    vector<string> ids;
    split(model_description.ids, ':', ids);
    if (ids.empty()) return false;

    // Store the model
    models.emplace_back(ids.front(), model_description.acknowledgements, (unsigned)models.size(), is.release());

    // Fail if this model id is aready in use.
    if (!models_map.emplace(ids.front(), &models.back()).second) return false;

    // Fill models_map with model aliases
    for (auto&& id : ids) {
      models_map.emplace(id, &models.back());

      // Create (but not overwrite) hyphen-separated prefixes.
      for (unsigned i = 0; i < id.size(); i++)
        if (id[i] == '-')
          models_map.emplace(id.substr(0, i), &models.back());
    }
  }

  // Resolve default model
  if (!models_map.count(options.default_model)) return false;

  const model_info* default_model = models_map[options.default_model];
  models_map[string()] = default_model;

  // Create model loader
  loader.reset(new model_loader(options.concurrent_limit));
  for (auto&& model : models)
    loader->add(&model);

  // Check that models are loadable if requested
  for (size_t i = 0; i < models.size(); i++)
    if (options.check_models_loadable ||
        (options.preload_default && &models[i] == default_model)) {
      if (!models[i].load()) return false;
      models[i].fill_capabilities();
      if (&models[i] != default_model || !options.preload_default) models[i].release();
    }
  if (options.preload_default && !loader->load(default_model->loader_id)) return false;

  // Fill json_models
  json_models.clear().object().indent().key("models").indent().object();
  for (auto& model : models) {
    json_models.indent().key(model.id).indent().array();
    if (model.can_tokenize) json_models.value("tokenizer");
    if (model.can_tag) json_models.value("tagger");
    if (model.can_parse) json_models.value("parser");
    json_models.close();
  }
  json_models.indent().close().indent().key("default_model").indent().value(default_model->id).finish(true);

  return true;
}

// Handlers with their URLs
unordered_map<string, bool (udpipe_service::*)(microrestd::rest_request&)> udpipe_service::handlers = {
  // REST service
  {"/models", &udpipe_service::handle_models},
  {"/process", &udpipe_service::handle_process},
  // Weblicht service
  {"/weblicht/tokenize", &udpipe_service::handle_weblicht_tokenize},
  {"/weblicht/tag", &udpipe_service::handle_weblicht_tag},
  {"/weblicht/parse", &udpipe_service::handle_weblicht_parse},
};

// Handle a request using the specified URL/handler map
bool udpipe_service::handle(microrestd::rest_request& req) {
  auto handler_it = handlers.find(req.url);
  return handler_it == handlers.end() ? req.respond_not_found() : (this->*handler_it->second)(req);
}

// Load selected model
udpipe_service::loaded_model* udpipe_service::load_model(const string& id, string& error) {
  auto model_it = models_map.find(id);
  if (model_it == models_map.end())
    return error.assign("Requested model '").append(id).append("' does not exist.\n"), nullptr;

  if (!loader->load(model_it->second->loader_id))
    return error.assign("Cannot load required model (UDPipe server internal error)!"), nullptr;

  return new loaded_model(model_it->second, loader.get());
}

// REST service
udpipe_service::rest_response_generator::rest_response_generator(const model_info* model) : model(model) {
  json.object();
  json.indent().key("model").indent().value(model->id);
  json.indent().key("acknowledgements").indent().array();
  json.indent().value("http://ufal.mff.cuni.cz/udpipe/1#udpipe_acknowledgements");
  if (!model->acknowledgements.empty()) json.indent().value(model->acknowledgements);
  json.indent().close().indent().key("result").indent().value("");
}

bool udpipe_service::handle_models(microrestd::rest_request& req) {
  return req.respond(rest_response_generator::mime, json_models);
}

bool udpipe_service::handle_process(microrestd::rest_request& req) {
  string error;
  auto id = get_model_id(req);
  unique_ptr<loaded_model> loaded(load_model(id, error));
  if (!loaded) return req.respond_error(error);

  string data; if (!get_data(req, data, error)) return req.respond_error(error);
  bool tokenizer = false;
  unique_ptr<input_format> input(get_input_format(req, loaded->model, tokenizer, error)); if (!input) return req.respond_error(error);
  auto& tagger = get_tagger(req, loaded->model, error); if (!error.empty()) return req.respond_error(error);
  auto& parser = get_parser(req, loaded->model, error); if (!error.empty()) return req.respond_error(error);
  unique_ptr<output_format> output(get_output_format(req, error)); if (!output) return req.respond_error(error);

  // Try loading the input and count the infclen header
  int infclen = 0;
  {
    input->set_text(data);
    sentence s;
    while (input->next_sentence(s, error))
      for (size_t i = 1; i < s.words.size(); i++) {
        const char* str = s.words[i].form.c_str();
        for (size_t len = s.words[i].form.size(); len; unilib::utf8::decode(str, len))
          infclen++;
      }
    if (!error.empty())
      return req.respond_error(error.insert(0, "Cannot read input data: ").append("\n"));

    input->reset_document();
  }

  input->set_text(data, true);
  class generator : public rest_response_generator {
   public:
    generator(loaded_model* loaded, input_format* input, const string& tagger, const string& parser, output_format* output)
        : rest_response_generator(loaded->model), loaded(loaded), input(input), tagger(tagger), parser(parser), output(output) {}

    bool generate() {
      if (!input->next_sentence(s, error)) {
        output->finish_document(os);
        json.value(os.str(), true);
        os.str(string());

        json.finish(true);

        return false;
      }

      if (tagger != "none")
        loaded->model->model->tag(s, tagger, error);
      if (parser != "none")
        loaded->model->model->parse(s, parser, error);

      output->write_sentence(s, os);
      json.value(os.str(), true);
      os.str(string());

      return true;
    }

   private:
    sentence s;
    string error;
    ostringstream os;
    unique_ptr<loaded_model> loaded;
    unique_ptr<input_format> input;
    const string& tagger;
    const string& parser;
    unique_ptr<output_format> output;
  };
  return req.respond(generator::mime, new generator(loaded.release(), input.release(), tagger, parser, output.release()), {{infclen_header, to_string(infclen).c_str()}});
}

// Weblicht service
udpipe_service::weblicht_response_generator::weblicht_response_generator(const model_info* model) : model(model) {}

microrestd::string_piece udpipe_service::weblicht_response_generator::current() const {
  return microrestd::string_piece(data.data(), data.size());
}

void udpipe_service::weblicht_response_generator::consume(size_t length) {
  data.erase(0, length);
}

void udpipe_service::weblicht_response_generator::append(const string& data) {
  this->data.append(data.begin(), data.end());
}

const char* udpipe_service::weblicht_response_generator::mime = "application/conllu";

bool udpipe_service::handle_weblicht_tokenize(microrestd::rest_request& req) {
  string error;
  auto id = get_model_id(req);
  unique_ptr<loaded_model> loaded(load_model(id, error));
  if (!loaded) return req.respond_error(error);

  unique_ptr<input_format> tokenizer(loaded->model->model->new_tokenizer(req.params["tokenizer"]));
  if (!tokenizer) return req.respond_error("Cannot create a tokenizer for a given model");
  unique_ptr<output_format> output(output_format::new_conllu_output_format());
  if (!output) return req.respond_error("Cannot create CoNLL-U output format");

  tokenizer->set_text(req.body);

  class generator : public weblicht_response_generator {
   public:
    generator(loaded_model* loaded, input_format* tokenizer, const string* tagger, const string* parser, output_format* output)
        : weblicht_response_generator(loaded->model), loaded(loaded), tokenizer(tokenizer), tagger(tagger), parser(parser), output(output) {}

    bool generate() {
      if (!tokenizer->next_sentence(s, error)) {
        output->finish_document(os); append(os.str()); os.str(string());
        return false;
      }

      if (tagger) model->model->tag(s, *tagger, error);
      if (parser) model->model->parse(s, *parser, error);

      output->write_sentence(s, os); append(os.str()); os.str(string());
      return true;
    }

   private:
    sentence s;
    string error;
    ostringstream os;
    unique_ptr<loaded_model> loaded;
    unique_ptr<input_format> tokenizer;
    const string* tagger;
    const string* parser;
    unique_ptr<output_format> output;
  };

  const string* tagger = (req.params.end() != req.params.find("tagger")) ?  &req.params["tagger"] : nullptr;
  const string* parser = (req.params.end() != req.params.find("parser")) ?  &req.params["parser"] : nullptr;
  return req.respond(generator::mime, new generator(loaded.release(), tokenizer.release(), tagger, parser, output.release()));
}

bool udpipe_service::handle_weblicht_tag(microrestd::rest_request& req) {
  return handle_weblicht_tag_parse(req, &req.params["tagger"], nullptr);
}

bool udpipe_service::handle_weblicht_parse(microrestd::rest_request& req) {
  return handle_weblicht_tag_parse(req, nullptr, &req.params["parser"]);
}

bool udpipe_service::handle_weblicht_tag_parse(microrestd::rest_request& req, const string* tagger, const string* parser) {
  string error;
  auto id = get_model_id(req);
  unique_ptr<loaded_model> loaded(load_model(id, error));
  if (!loaded) return req.respond_error(error);

  unique_ptr<input_format> input(input_format::new_conllu_input_format());
  if (!input) return req.respond_error("Cannot create CoNLL-U input format");
  unique_ptr<output_format> output(output_format::new_conllu_output_format());
  if (!output) return req.respond_error("Cannot create CoNLL-U output format");

  // Try loading the input CoNLL-U file
  input->set_text(req.body);
  sentence s;
  while (input->next_sentence(s, error)) {}
  if (!error.empty())
    return req.respond_error(error.insert(0, "Cannot read input CoNLL-U file: ").append("\n"));

  input->reset_document();
  input->set_text(req.body);
  class generator : public weblicht_response_generator {
   public:
    generator(loaded_model* loaded, const string* tagger, const string* parser, input_format* input, output_format* output)
        : weblicht_response_generator(loaded->model), loaded(loaded), tagger(tagger), parser(parser), input(input), output(output) {}

    bool generate() {
      if (!input->next_sentence(s, error)) {
        output->finish_document(os); append(os.str()); os.str(string());
        return false;
      }

      if (tagger) model->model->tag(s, *tagger, error);
      if (parser) model->model->parse(s, *parser, error);

      output->write_sentence(s, os); append(os.str()); os.str(string());
      return true;
    }

   private:
    sentence s;
    string error;
    ostringstream os;
    unique_ptr<loaded_model> loaded;
    const string* tagger;
    const string* parser;
    unique_ptr<input_format> input;
    unique_ptr<output_format> output;
  };
  return req.respond(generator::mime, new generator(loaded.release(), tagger, parser, input.release(), output.release()));
}

// Helper functions
const string& udpipe_service::get_model_id(microrestd::rest_request& req) {
  auto model_it = req.params.find("model");
  return model_it == req.params.end() ? empty : model_it->second;
}

bool udpipe_service::get_data(microrestd::rest_request& req, string& data, string& error) {
  auto data_it = req.params.find("data");
  if (data_it == req.params.end()) return error.assign("Required argument 'data' is missing.\n"), false;

  u32string codepoints;
  unilib::utf8::decode(data_it->second, codepoints);
  unilib::uninorms::nfc(codepoints);
  unilib::utf8::encode(codepoints, data);
  return true;
}

input_format* udpipe_service::get_input_format(microrestd::rest_request& req, const model_info* model, bool& is_tokenizer, string& error) {
  auto tokenizer_it = req.params.find("tokenizer");
  if (tokenizer_it != req.params.end()) {
    if (!model->can_tokenize) return error.assign("The required model does not contain a tokenizer!"), nullptr;
    input_format* tokenizer = model->model->new_tokenizer(tokenizer_it->second);
    if (!tokenizer) return error.assign("Cannot construct a tokenizer instance!"), nullptr;
    return is_tokenizer = true, tokenizer;
  }

  auto& input = req.params.emplace("input", "conllu").first->second;
  auto input_format = input_format::new_input_format(input);
  if (!input_format) return error.assign("Unknown input format '").append(input).append("'.\n"), nullptr;
  return is_tokenizer = false, input_format;
}

const string& udpipe_service::get_tagger(microrestd::rest_request& req, const model_info* model, string& error) {
  auto& tagger = req.params.emplace("tagger", "none").first->second;
  if (tagger != "none" && !model->can_tag) return error.assign("The required model does not contain a tagger!"), empty;
  return tagger;
}

const string& udpipe_service::get_parser(microrestd::rest_request& req, const model_info* model, string& error) {
  auto& parser = req.params.emplace("parser", "none").first->second;
  if (parser != "none" && !model->can_parse) return error.assign("The required model does not contain a parser!"), empty;
  return parser;
}

output_format* udpipe_service::get_output_format(microrestd::rest_request& req, string& error) {
  auto& output = req.params.emplace("output", "conllu").first->second;
  auto output_format = output_format::new_output_format(output);
  if (!output_format) return error.assign("Unknown output format '").append(output).append("'.\n"), nullptr;
  return output_format;
}

const string udpipe_service::empty;
const char* udpipe_service::infclen_header = "X-Billing-Input-NFC-Len";

} // namespace udpipe
} // namespace ufal
