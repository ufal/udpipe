// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "udpipe_service.h"

namespace ufal {
namespace udpipe {

// Init the UDPipe service -- load the models
bool udpipe_service::init(const vector<model_description>& model_descriptions) {
  if (model_descriptions.empty()) return false;

  // Load models
  models.clear();
  rest_models_map.clear();
  for (auto& model_description : model_descriptions) {
    // Store the model
    models.emplace_back(model_description.rest_id, model_description.acknowledgements);
  }

  // Fill rest_models_map with model name and aliases
  for (auto& model : models) {
    // Fail if this model id is aready in use.
    if (!rest_models_map.emplace(model.rest_id, &model).second) return false;

    // Create (but not overwrite) id without version.
    for (unsigned i = 0; i+1+6 < model.rest_id.size(); i++)
      if (model.rest_id[i] == '-') {
        bool is_version = true;
        for (unsigned j = i+1; j < i+1+6; j++)
          is_version = is_version && model.rest_id[j] >= '0' && model.rest_id[j] <= '9';
        if (is_version)
          rest_models_map.emplace(model.rest_id.substr(0, i) + model.rest_id.substr(i+1+6), &model);
      }

    // Create (but not overwrite) hyphen-separated prefixes.
    for (unsigned i = 0; i < model.rest_id.size(); i++)
      if (model.rest_id[i] == '-')
        rest_models_map.emplace(model.rest_id.substr(0, i), &model);
  }
  // Default model
  rest_models_map.emplace(string(), &models.front());

  // Init REST service
  json_models.clear().object().indent().key("models").indent().array();
  for (auto& model : models) {
    json_models.indent().value(model.rest_id);
  }
  json_models.indent().close().indent().key("default_model").indent().value(model_descriptions.front().rest_id).finish(true);

  return true;
}

// Handlers with their URLs
unordered_map<string, bool (udpipe_service::*)(microrestd::rest_request&)> udpipe_service::handlers = {
  // REST service
  {"/models", &udpipe_service::handle_rest_models},
};

// Handle a request using the specified URL/handler map
bool udpipe_service::handle(microrestd::rest_request& req) {
  auto handler_it = handlers.find(req.url);
  return handler_it == handlers.end() ? req.respond_not_found() : (this->*handler_it->second)(req);
}

// Load selected model
const udpipe_service::model_info* udpipe_service::load_rest_model(const string& rest_id, string& error) {
  auto model_it = rest_models_map.find(rest_id);
  if (model_it == rest_models_map.end())
    return error.assign("Requested model '").append(rest_id).append("' does not exist.\n"), nullptr;

  return model_it->second;
}

// REST service

udpipe_service::rest_response_generator::rest_response_generator(const model_info* model) : model(model) {
  json.object();
  json.indent().key("model").indent().value(model->rest_id);
  json.indent().key("acknowledgements").indent().array();
  json.indent().value("http://ufal.mff.cuni.cz/udpipe#udpipe_acknowledgements");
  if (!model->acknowledgements.empty()) json.indent().value(model->acknowledgements);
  json.indent().close().indent().key("result").indent().value("");
}

// REST service handlers

bool udpipe_service::handle_rest_models(microrestd::rest_request& req) {
  return req.respond(rest_response_generator::mime, json_models);
}

} // namespace udpipe
} // namespace ufal
