// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>

#include "string_piece.h"

namespace ufal {
namespace microrestd {

// Declarations
class json_builder {
 public:
  // Clear
  inline json_builder& clear();

  // Encode
  inline json_builder& object();
  inline json_builder& array();
  inline json_builder& key(string_piece str);
  inline json_builder& value(string_piece str, bool append = false);
  inline json_builder& value_xml_escape(string_piece str, bool append = false);
  inline json_builder& indent();
  inline json_builder& close();
  inline json_builder& finish(bool indent = false);

  // Return current json
  inline string_piece current() const;
  inline operator string_piece() const;

  // Remove current json prefix; for response_generator
  void discard_current_prefix(size_t length);

  // JSON mime
  static const char* mime;

 private:
  enum mode_t { NORMAL, IN_VALUE, AFTER_VALUE };

  inline void normalize_mode(bool start_value);
  void encode(string_piece str);
  void encode_xml_escape(string_piece str);

  std::vector<char> json;
  std::vector<char> stack;
  mode_t mode = NORMAL;
  bool need_indent = false;
};


// Definitions
json_builder& json_builder::clear() {
  json.clear();
  stack.clear();
  mode = NORMAL;
  need_indent = false;
  return *this;
}

json_builder& json_builder::object() {
  normalize_mode(true);
  json.push_back('{');
  stack.push_back('}');
  return *this;
}

json_builder& json_builder::array() {
  normalize_mode(true);
  json.push_back('[');
  stack.push_back(']');
  return *this;
}

json_builder& json_builder::key(string_piece str) {
  normalize_mode(true);
  json.push_back('"');
  encode(str);
  json.push_back('"');
  json.push_back(':');
  return *this;
}

json_builder& json_builder::value(string_piece str, bool append) {
  if (!append || mode != IN_VALUE) {
    normalize_mode(true);
    json.push_back('"');
  }
  mode = IN_VALUE;
  encode(str);
  return *this;
}

json_builder& json_builder::value_xml_escape(string_piece str, bool append) {
  if (!append || mode != IN_VALUE) {
    normalize_mode(true);
    json.push_back('"');
  }
  mode = IN_VALUE;
  encode_xml_escape(str);
  return *this;
}

json_builder& json_builder::indent() {
  if (!need_indent) {
    normalize_mode(false);
    need_indent = true;
  }
  return *this;
}

json_builder& json_builder::close() {
  if (!stack.empty()) {
    char closing_char = stack.back();
    stack.pop_back();
    normalize_mode(false);
    json.push_back(closing_char);
    mode = AFTER_VALUE;
  }
  return *this;
}

json_builder& json_builder::finish(bool indent) {
  while (!stack.empty()) {
    if (indent) this->indent();
    close();
  }
  if (!json.empty() && json.back() != '\n') json.push_back('\n');
  return *this;
}

string_piece json_builder::current() const {
  return string_piece(json.data(), json.size());
}

json_builder::operator string_piece() const {
  return current();
}

void json_builder::normalize_mode(bool start_value) {
  if (mode == IN_VALUE) {
    json.push_back('"');
    mode = AFTER_VALUE;
  }

  if (start_value && mode == AFTER_VALUE) {
    json.push_back(',');
    mode = NORMAL;
  }

  if (need_indent) {
    if (!json.empty() && json.back() == ':') {
      json.push_back(' ');
    } else {
      json.push_back('\n');
      if (!stack.empty()) json.insert(json.end(), stack.size(), ' ');
    }
    need_indent = false;
  }
}

} // namespace microrestd
} // namespace ufal
