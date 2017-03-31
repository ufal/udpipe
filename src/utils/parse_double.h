// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cmath>

#include "common.h"
#include "string_piece.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

// Try to parse an double from given string. If the double cannot be parsed or does
// not fit doubleo double, false is returned and the error string is filled using the
// value_name argument.
inline bool parse_double(string_piece str, const char* value_name, double& value, string& error);

// Try to parse an double from given string. If the double cannot be parsed or does
// not fit doubleo double, an error is displayed and program exits.
inline double parse_double(string_piece str, const char* value_name);

//
// Definitions
//

bool parse_double(string_piece str, const char* value_name, double& value, string& error) {
  string_piece original = str;

  // Skip spaces
  while (str.len && (str.str[0] == ' ' || str.str[0] == '\f' || str.str[0] == '\n' || str.str[0] == '\r' || str.str[0] == '\t' || str.str[0] == '\v'))
    str.str++, str.len--;

  // Allow plus/minus
  bool negative = false;
  if (str.len && (str.str[0] == '+' || str.str[0] == '-')) {
    negative = str.str[0] == '-';
    str.str++, str.len--;
  }

  // Parse value, checking for overflow/underflow
  if (!str.len) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': empty string."), false;
  if (!(str.str[0] >= '0' || str.str[0] <= '9')) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': non-digit character found."), false;

  value = 0;
  while (str.len && str.str[0] >= '0' && str.str[0] <= '9') {
    value = 10 * value + (str.str[0] - '0');
    str.str++, str.len--;
  }

  // If there is a decimal point, parse the rest of the
  if (str.len && str.str[0] == '.') {
    double divider = 1;

    str.str++, str.len--;
    while (str.len && str.str[0] >= '0' && str.str[0] <= '9') {
      value = 10 * value + (str.str[0] - '0');
      divider *= 10.;
      str.str++, str.len--;
    }

    value /= divider;
  }
  if (!isfinite(value)) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': overflow occured."), false;

  // Optionally parse an exponent
  if (str.len && (str.str[0] == 'e' || str.str[0] == 'E')) {
    str.str++, str.len--;

    double exponent = 0;
    bool exponent_negative = false;
    if (str.len && (str.str[0] == '+' || str.str[0] == '-')) {
      exponent_negative = str.str[0] == '-';
      str.str++, str.len--;
    }

    while (str.len && str.str[0] >= '0' && str.str[0] <= '9') {
      exponent = 10 * exponent + (str.str[0] - '0');
      str.str++, str.len--;
    }

    exponent = pow(10., exponent_negative ? -exponent : exponent);
    if (!isfinite(exponent)) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': exponent overflow occured."), false;
    if (exponent == 0) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': exponent underflow occured."), false;

    if (value) {
      value *= exponent;
      if (!isfinite(value)) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': overflow occured."), false;
      if (value == 0) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': underflow occured."), false;
    }
  }

  // Apply initial minus
  if (negative) value *= -1;

  // Skip spaces
  while (str.len && (str.str[0] == ' ' || str.str[0] == '\f' || str.str[0] == '\n' || str.str[0] == '\r' || str.str[0] == '\t' || str.str[0] == '\v'))
    str.str++, str.len--;

  // Check for remaining characters
  if (str.len) return error.assign("Cannot parse ").append(value_name).append(" double value '").append(original.str, original.len).append("': non-digit character found."), false;

  return true;
}

double parse_double(string_piece str, const char* value_name) {
  double result;
  string error;

  if (!parse_double(str, value_name, result, error))
    runtime_failure(error);

  return result;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
