// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "json_builder.h"

namespace ufal {
namespace microrestd {

const char* json_builder::mime = "application/json";

void json_builder::discard_current_prefix(size_t length) {
  if (!length) return;

  if (length < json.size())
    json.erase(json.begin(), json.begin() + length);
  else
    json.clear();
}

void json_builder::encode(string_piece str) {
  for (; str.len; str.str++, str.len--)
    switch (*str.str) {
      case '"': json.push_back('\\'); json.push_back('\"'); break;
      case '\\': json.push_back('\\'); json.push_back('\\'); break;
      case '\b': json.push_back('\\'); json.push_back('b'); break;
      case '\f': json.push_back('\\'); json.push_back('f'); break;
      case '\n': json.push_back('\\'); json.push_back('n'); break;
      case '\r': json.push_back('\\'); json.push_back('r'); break;
      case '\t': json.push_back('\\'); json.push_back('t'); break;
      default:
        if (((unsigned char)*str.str) < 32) {
          json.push_back('u'); json.push_back('0'); json.push_back('0'); json.push_back('0' + (*str.str >> 4)); json.push_back("0123456789ABCDEF"[*str.str & 0xF]);
        } else {
          json.push_back(*str.str);
        }
    }
}

void json_builder::encode_xml_escape(string_piece str) {
  for (; str.len; str.str++, str.len--)
    switch (*str.str) {
      case '&': json.push_back('&'); json.push_back('a'); json.push_back('m'); json.push_back('p'); json.push_back(';'); break;
      case '<': json.push_back('&'); json.push_back('l'); json.push_back('t'); json.push_back(';'); break;
      case '>': json.push_back('&'); json.push_back('g'); json.push_back('t'); json.push_back(';'); break;
      case '"': json.push_back('&'); json.push_back('q'); json.push_back('u'); json.push_back('o'); json.push_back('t'); json.push_back(';'); break;
      case '\\': json.push_back('\\'); json.push_back('\\'); break;
      case '\b': json.push_back('\\'); json.push_back('b'); break;
      case '\f': json.push_back('\\'); json.push_back('f'); break;
      case '\n': json.push_back('\\'); json.push_back('n'); break;
      case '\r': json.push_back('\\'); json.push_back('r'); break;
      case '\t': json.push_back('\\'); json.push_back('t'); break;
      default:
        if (((unsigned char)*str.str) < 32) {
          json.push_back('u'); json.push_back('0'); json.push_back('0'); json.push_back('0' + (*str.str >> 4)); json.push_back("0123456789ABCDEF"[*str.str & 0xF]);
        } else {
          json.push_back(*str.str);
        }
    }
}

} // namespace microrestd
} // namespace ufal
