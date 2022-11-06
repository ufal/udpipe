// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "response_generator.h"
#include "string_piece.h"

namespace ufal {
namespace microrestd {

class rest_request {
 public:
  virtual ~rest_request() {}

  virtual bool respond(const char* content_type, string_piece body,
                       const std::vector<std::pair<const char*, const char*>>& headers = {}) = 0;
  virtual bool respond(const char* content_type, response_generator* generator,
                       const std::vector<std::pair<const char*, const char*>>& headers = {}) = 0;
  virtual bool respond_not_found() = 0;
  virtual bool respond_method_not_allowed(const char* comma_separated_allowed_methods) = 0;
  virtual bool respond_error(string_piece error, int code = 400) = 0;

  std::string url;
  std::string method;
  std::string body;
  std::string content_type;
  std::unordered_map<std::string, std::string> params;
};

} // namespace microrestd
} // namespace ufal
