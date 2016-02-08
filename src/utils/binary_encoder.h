// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "string_piece.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

class binary_encoder {
 public:
  inline binary_encoder();

  inline void add_1B(unsigned val);
  inline void add_2B(unsigned val);
  inline void add_4B(unsigned val);
  inline void add_float(double val);
  inline void add_double(double val);
  inline void add_str(string_piece str);
  inline void add_data(string_piece data);
  template <class T> inline void add_data(const vector<T>& data);
  template <class T> inline void add_data(const T* data, size_t elements);

  vector<unsigned char> data;
};

//
// Definitions
//

binary_encoder::binary_encoder() {
  data.reserve(16);
}

void binary_encoder::add_1B(unsigned val) {
  if (uint8_t(val) != val) runtime_failure("Should encode value " << val << " in one byte!");
  data.push_back(val);
}

void binary_encoder::add_2B(unsigned val) {
  if (uint16_t(val) != val) runtime_failure("Should encode value " << val << " in one byte!");
  data.insert(data.end(), (unsigned char*) &val, ((unsigned char*) &val) + sizeof(uint16_t));
}

void binary_encoder::add_4B(unsigned val) {
  if (uint32_t(val) != val) runtime_failure("Should encode value " << val << " in one byte!");
  data.insert(data.end(), (unsigned char*) &val, ((unsigned char*) &val) + sizeof(uint32_t));
}

void binary_encoder::add_float(double val) {
  data.insert(data.end(), (unsigned char*) &val, ((unsigned char*) &val) + sizeof(float));
}

void binary_encoder::add_double(double val) {
  data.insert(data.end(), (unsigned char*) &val, ((unsigned char*) &val) + sizeof(double));
}

void binary_encoder::add_str(string_piece str) {
  add_1B(str.len < 255 ? str.len : 255);
  if (!(str.len < 255)) add_4B(str.len);
  add_data(str);
}

void binary_encoder::add_data(string_piece data) {
  this->data.insert(this->data.end(), (const unsigned char*) data.str, (const unsigned char*) (data.str + data.len));
}

template <class T>
void binary_encoder::add_data(const vector<T>& data) {
  this->data.insert(this->data.end(), (const unsigned char*) data.data(), (const unsigned char*) (data.data() + data.size()));
}

template <class T>
void binary_encoder::add_data(const T* data, size_t elements) {
  this->data.insert(this->data.end(), (const unsigned char*) data, (const unsigned char*) (data + elements));
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
