// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <limits>

#include "common.h"
#include "string_piece.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

// Try to parse an int from given string. If the int cannot be parsed or does
// not fit into int, false is returned and the error string is filled using the
// value_name argument.
inline bool parse_int(string_piece str, const char* value_name, int& value, string& error);

// Try to parse an int from given string. If the int cannot be parsed or does
// not fit into int, an error is displayed and program exits.
inline int parse_int(string_piece str, const char* value_name);

//
// Definitions
//

bool parse_int(string_piece str, const char* value_name, int& value, string& error) {
  string_piece original = str;

  // Skip spaces
  while (str.len && (str.str[0] == ' ' || str.str[0] == '\f' || str.str[0] == '\n' || str.str[0] == '\r' || str.str[0] == '\t' || str.str[0] == '\v'))
    str.str++, str.len--;

  // Allow minus
  bool positive = true;
  if (str.len && str.str[0] == '-') {
    positive = false;
    str.str++, str.len--;
  }

  // Parse value, checking for overflow/underflow
  if (!str.len) return error.assign("Cannot parse ").append(value_name).append(" int value '").append(original.str, original.len).append("': empty string."), false;
  if (!(str.str[0] >= '0' || str.str[0] <= '9')) return error.assign("Cannot parse ").append(value_name).append(" int value '").append(original.str, original.len).append("': non-digit character found."), false;

  value = 0;
  while (str.len && str.str[0] >= '0' && str.str[0] <= '9') {
    if (positive) {
      if (value > (numeric_limits<int>::max() - (str.str[0] - '0')) / 10)
        return error.assign("Cannot parse ").append(value_name).append(" int value '").append(original.str, original.len).append("': overflow occured."), false;
      value = 10 * value + (str.str[0] - '0');
    } else {
      if (value < (numeric_limits<int>::min() + (str.str[0] - '0')) / 10)
        return error.assign("Cannot parse ").append(value_name).append(" int value '").append(original.str, original.len).append("': underflow occured."), false;
      value = 10 * value - (str.str[0] - '0');
    }
    str.str++, str.len--;
  }

  // Skip spaces
  while (str.len && (str.str[0] == ' ' || str.str[0] == '\f' || str.str[0] == '\n' || str.str[0] == '\r' || str.str[0] == '\t' || str.str[0] == '\v'))
    str.str++, str.len--;

  // Check for remaining characters
  if (str.len) return error.assign("Cannot parse ").append(value_name).append(" int value '").append(original.str, original.len).append("': non-digit character found."), false;

  return true;
}

int parse_int(string_piece str, const char* value_name) {
  int result;
  string error;

  if (!parse_int(str, value_name, result, error))
    runtime_failure(error);

  return result;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
