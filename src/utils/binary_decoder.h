// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstring>
#include <stdexcept>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

class binary_decoder_error : public runtime_error {
 public:
  explicit binary_decoder_error(const char* description) : runtime_error(description) {}
};

class binary_decoder {
 public:
  inline unsigned char* fill(unsigned len);

  inline unsigned next_1B() throw (binary_decoder_error);
  inline unsigned next_2B() throw (binary_decoder_error);
  inline unsigned next_4B() throw (binary_decoder_error);
  inline void next_str(string& str) throw (binary_decoder_error);
  template <class T> inline const T* next(unsigned elements) throw (binary_decoder_error);

  inline bool is_end();
  inline unsigned tell();
  inline void seek(unsigned pos) throw (binary_decoder_error);

 private:
  vector<unsigned char> buffer;
  const unsigned char* data;
  const unsigned char* data_end;
};

//
// Definitions
//

unsigned char* binary_decoder::fill(unsigned len) {
  buffer.resize(len);
  data = buffer.data();
  data_end = buffer.data() + len;

  return buffer.data();
}

unsigned binary_decoder::next_1B() throw (binary_decoder_error) {
  if (data + 1 > data_end) throw binary_decoder_error("No more data in binary_decoder");
  return *data++;
}

unsigned binary_decoder::next_2B() throw (binary_decoder_error) {
  if (data + sizeof(uint16_t) > data_end) throw binary_decoder_error("No more data in binary_decoder");
  uint16_t result;
  memcpy(&result, data, sizeof(uint16_t));
  data += sizeof(uint16_t);
  return result;
}

unsigned binary_decoder::next_4B() throw (binary_decoder_error) {
  if (data + sizeof(uint32_t) > data_end) throw binary_decoder_error("No more data in binary_decoder");
  uint32_t result;
  memcpy(&result, data, sizeof(uint32_t));
  data += sizeof(uint32_t);
  return result;
}

void binary_decoder::next_str(string& str) throw (binary_decoder_error) {
  unsigned len = next_1B();
  if (len == 255) len = next_4B();
  str.assign(next<char>(len), len);
}

template <class T> const T* binary_decoder::next(unsigned elements) throw (binary_decoder_error) {
  if (data + sizeof(T) * elements > data_end) throw binary_decoder_error("No more data in binary_decoder");
  const T* result = (const T*) data;
  data += sizeof(T) * elements;
  return result;
}

bool binary_decoder::is_end() {
  return data >= data_end;
}

unsigned binary_decoder::tell() {
  return data - buffer.data();
}

void binary_decoder::seek(unsigned pos) throw (binary_decoder_error) {
  if (pos > buffer.size()) throw binary_decoder_error("Cannot seek past end of binary_decoder");
  data = buffer.data() + pos;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
