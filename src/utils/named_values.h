// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>
#include <fstream>
#include <unordered_map>

#include "common.h"
#include "parse_int.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

class named_values {
 public:
  typedef unordered_map<string, string> map;

  inline static bool parse(const string& values, map& parsed_values, string& error);
};

//
// Definitions
//

bool named_values::parse(const string& values, map& parsed_values, string& error) {
  error.clear();
  parsed_values.clear();

  string name, file;
  for (size_t start = 0; start < values.size(); ) {
    while (start < values.size() && values[start] == ';') start++;
    if (start >= values.size()) break;

    size_t equal_sign = values.find('=', start);
    if (equal_sign == string::npos) return error.assign("Cannot parse named values, equal sign was not found!"), false;
    name.assign(values, start, equal_sign - start);

    if (equal_sign + 1 + 5 <= values.size() && values.compare(equal_sign + 1, 5, "file:") == 0) {
      // Value of type file:
      size_t file_name = equal_sign + 1 + 5;
      size_t semicolon = min(values.find(';', file_name), values.size());

      file.assign(values, file_name, semicolon - file_name);
      ifstream is(file.c_str());
      if (!is.is_open()) return error.assign("Cannot open file '").append(file).append("'!"), false;

      string& value = parsed_values[name];
      char buffer[1024];
      for (value.clear(); is.read(buffer, sizeof(buffer)); )
        value.append(buffer, sizeof(buffer));
      value.append(buffer, is.gcount());

      start = semicolon + 1;
    } else if (equal_sign + 1 + 5 <= values.size() && values.compare(equal_sign + 1, 5, "data:") == 0) {
      // Value of type data:
      size_t data_size_start = equal_sign + 1 + 5;
      size_t data_size_end = values.find(':', data_size_start);
      if (data_size_end == string::npos) return error.assign("Cannot parse named values, data size of value '").append(name).append("' not terminated!"), false;

      int data_size;
      if (!parse_int(string_piece(values.c_str() + data_size_start, data_size_end - data_size_start), "data_size", data_size, error)) return false;

      size_t data_start = data_size_end + 1, data_end = data_start + data_size;
      if (data_end > values.size()) return error.assign("Cannot parse named values, value '").append(name).append("' shorter than specified length!"), false;
      if (data_end < values.size() && values[data_end] != ';') return error.assign("Cannot parse named values, value '").append(name).append("' not terminated by semicolon!"), false;

      parsed_values[name].assign(values, data_start, data_end - data_start);
      start = data_end + 1;
    } else {
      // Value of string type
      size_t semicolon = min(values.find(';', equal_sign), values.size());
      parsed_values[name].assign(values, equal_sign + 1, semicolon - equal_sign - 1);
      start = semicolon + 1;
    }
  }

  return true;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
