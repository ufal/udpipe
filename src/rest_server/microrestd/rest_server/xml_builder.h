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
#include <vector>

#include "string_piece.h"

namespace ufal {
namespace microrestd {

// Declarations
class xml_builder {
 public:
  // Clear
  inline xml_builder& clear();

  // XML declaration
  xml_builder& declaration();

  // Encode
  inline xml_builder& element(string_piece name);
  inline xml_builder& attribute(string_piece name, string_piece value);
  inline xml_builder& text(string_piece str);
  inline xml_builder& indent();
  inline xml_builder& close();
  inline xml_builder& finish(bool indent = false);

  // Return current xml
  inline string_piece current() const;
  inline operator string_piece() const;

  // Remove current xml prefix; for response_generator
  void discard_current_prefix(size_t length);

  // XML mime
  static const char* mime;

 private:
  enum mode_t { NORMAL, IN_ELEMENT, NEED_INDENT };

  inline void normalize_mode();
  void encode(string_piece str);

  std::vector<char> xml;
  std::vector<std::string> stack;
  size_t stack_length = 0;
  mode_t mode = NORMAL;
};

// Definitions
xml_builder& xml_builder::clear() {
  xml.clear();
  stack.clear();
  stack_length = 0;
  mode = NORMAL;
  return *this;
}

xml_builder& xml_builder::element(string_piece name) {
  normalize_mode();

  xml.push_back('<');
  xml.insert(xml.end(), name.str, name.str + name.len);
  mode = IN_ELEMENT;

  if (stack_length < stack.size())
    stack[stack_length].assign(name.str, name.len);
  else
    stack.emplace_back(name.str, name.len);
  stack_length++;

  return *this;
}

xml_builder& xml_builder::attribute(string_piece name, string_piece value) {
  if (mode == IN_ELEMENT) {
    xml.push_back(' ');
    xml.insert(xml.end(), name.str, name.str + name.len);
    xml.push_back('=');
    xml.push_back('"');
    encode(value);
    xml.push_back('"');
  }
  return *this;
}

xml_builder& xml_builder::text(string_piece str) {
  normalize_mode();
  encode(str);
  return *this;
}

xml_builder& xml_builder::indent() {
  if (mode == IN_ELEMENT) normalize_mode();
  mode = NEED_INDENT;
  return *this;
}

xml_builder& xml_builder::close() {
  if (stack_length) {
    stack_length--;
    if (mode == NEED_INDENT) normalize_mode();
    if (mode == IN_ELEMENT) {
      xml.push_back('/');
      xml.push_back('>');
      mode = NORMAL;
    } else {
      xml.push_back('<');
      xml.push_back('/');
      xml.insert(xml.end(), stack[stack_length].begin(), stack[stack_length].end());
      xml.push_back('>');
    }
  }
  return *this;
}

xml_builder& xml_builder::finish(bool indent) {
  while (stack_length) {
    if (indent) this->indent();
    close();
  }
  if (!xml.empty() && xml.back() != '\n') xml.push_back('\n');
  return *this;
}

string_piece xml_builder::current() const {
  return string_piece(xml.data(), xml.size());
}

xml_builder::operator string_piece() const {
  return current();
}

void xml_builder::normalize_mode() {
  if (mode == NEED_INDENT) {
    xml.push_back('\n');
    if (stack_length) xml.insert(xml.end(), stack_length, ' ');
    mode = NORMAL;
  } else if (mode == IN_ELEMENT) {
    xml.push_back('>');
    mode = NORMAL;
  }
}

} // namespace microrestd
} // namespace ufal
