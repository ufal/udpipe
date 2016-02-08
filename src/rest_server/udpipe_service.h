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

namespace ufal {
namespace udpipe {

namespace microrestd = ufal::microrestd;

class udpipe_service : public microrestd::rest_service {
 public:
  struct model_description {
    string rest_id, file, acknowledgements;

    model_description(const string& rest_id, const string& file, const string& acknowledgements)
        : rest_id(rest_id), file(file), acknowledgements(acknowledgements) {}
  };

  bool init(const vector<model_description>& model_descriptions);

  virtual bool handle(microrestd::rest_request& req) override;

 private:
  static unordered_map<string, bool (udpipe_service::*)(microrestd::rest_request&)> handlers;

  // Models
  struct model_info {
    model_info(const string& rest_id, const string& acknowledgements)
        : rest_id(rest_id), acknowledgements(acknowledgements) {}

    string rest_id;
    string acknowledgements;
  };
  vector<model_info> models;
  unordered_map<string, const model_info*> rest_models_map;

  const model_info* load_rest_model(const string& rest_id, string& error);

  // REST service
  class rest_response_generator : public microrestd::json_response_generator {
   public:
    rest_response_generator(const model_info* model);
   protected:
    const model_info* model;
  };

  bool handle_rest_models(microrestd::rest_request& req);

  microrestd::json_builder json_models;
};

} // namespace udpipe
} // namespace ufal
