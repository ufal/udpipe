// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "microrestd/microrestd.h"
#include "model/model.h"
#include "sentence/input_format.h"
#include "sentence/output_format.h"
#include "utils/threadsafe_resource_loader.h"

namespace ufal {
namespace udpipe {

namespace microrestd = ufal::microrestd;

class udpipe_service : public microrestd::rest_service {
 public:
  typedef ufal::udpipe::model Model;

  struct model_description {
    string ids, file, acknowledgements;

    model_description(const string& ids, const string& file, const string& acknowledgements)
        : ids(ids), file(file), acknowledgements(acknowledgements) {}
  };

  struct service_options {
    vector<model_description> model_descriptions;
    string default_model;
    unsigned concurrent_limit;
    bool preload_default;
    bool check_models_loadable;
  };

  bool init(const service_options& options);

  virtual bool handle(microrestd::rest_request& req) override;

 private:
  static unordered_map<string, bool (udpipe_service::*)(microrestd::rest_request&)> handlers;

  // Models
  struct model_info {
    model_info(const string& id, const string& acknowledgements, unsigned loader_id, istream* is)
        : id(id), acknowledgements(acknowledgements), loader_id(loader_id), is(is) {}

    bool load() {
      if (!model) {
        is->seekg(0);
        model.reset(Model::load(*is));
      }
      return model != nullptr;
    }

    void release() {
      model.reset();
    }

    void fill_capabilities() {
      if (model) {
        sentence s;
        string error;
        unique_ptr<input_format> tokenizer(model->new_tokenizer(Model::DEFAULT));

        can_tokenize = tokenizer.get() != nullptr;
        can_tag = model->tag(s, Model::DEFAULT, error);
        can_parse = model->parse(s, Model::DEFAULT, error);
      }
    }

    string id;
    string acknowledgements;
    unsigned loader_id;
    unique_ptr<istream> is;
    unique_ptr<Model> model;
    bool can_tokenize = true;
    bool can_tag = true;
    bool can_parse = true;
  };
  vector<model_info> models;
  unordered_map<string, const model_info*> models_map;

  typedef threadsafe_resource_loader<model_info> model_loader;
  unique_ptr<model_loader> loader;

  struct loaded_model {
    const model_info* model;
    model_loader* loader;

    loaded_model(const model_info* model, model_loader* loader) : model(model), loader(loader) {}
    ~loaded_model() { loader->release(model->loader_id); }
  };
  loaded_model* load_model(const string& id, string& error);

  // REST service
  class rest_response_generator : public microrestd::json_response_generator {
   public:
    rest_response_generator(const model_info* model);
   protected:
    const model_info* model;
  };

  bool handle_models(microrestd::rest_request& req);
  bool handle_process(microrestd::rest_request& req);

  // Weblicht service
  class weblicht_response_generator : public microrestd::response_generator {
   public:
    weblicht_response_generator(const model_info* model);
    virtual microrestd::string_piece current() const override;
    virtual void consume(size_t length) override;
    void append(const string& data);

    static const char* mime;
   protected:
    string data;
    const model_info* model;
  };

  bool handle_weblicht_tokenize(microrestd::rest_request& req);
  bool handle_weblicht_tag(microrestd::rest_request& req);
  bool handle_weblicht_parse(microrestd::rest_request& req);
  bool handle_weblicht_tag_parse(microrestd::rest_request& req, const string* tagger, const string* parser);

  // Helper functions
  const string& get_model_id(microrestd::rest_request& req);
  bool get_data(microrestd::rest_request& req, string& data, string& error);
  input_format* get_input_format(microrestd::rest_request& req, const model_info* model, bool& is_tokenizer, string& error);
  const string& get_tagger(microrestd::rest_request& req, const model_info* model, string& error);
  const string& get_parser(microrestd::rest_request& req, const model_info* model, string& error);
  output_format* get_output_format(microrestd::rest_request& req, string& error);

  microrestd::json_builder json_models;

  static const string empty;
  static const char* infclen_header;
};

} // namespace udpipe
} // namespace ufal
