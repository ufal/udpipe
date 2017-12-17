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

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

class pointer_decoder {
 public:
  inline pointer_decoder(const unsigned char*& data);
  inline unsigned next_1B();
  inline unsigned next_2B();
  inline unsigned next_4B();
  inline void next_str(string& str);
  template <class T> inline const T* next(unsigned elements);

 private:
  const unsigned char*& data;
};

//
// Definitions
//

pointer_decoder::pointer_decoder(const unsigned char*& data) : data(data) {}

unsigned pointer_decoder::next_1B() {
  return *data++;
}

unsigned pointer_decoder::next_2B() {
  uint16_t result;
  memcpy(&result, data, sizeof(uint16_t));
  data += sizeof(uint16_t);
  return result;
}

unsigned pointer_decoder::next_4B() {
  uint32_t result;
  memcpy(&result, data, sizeof(uint32_t));
  data += sizeof(uint32_t);
  return result;
}

void pointer_decoder::next_str(string& str) {
  unsigned len = next_1B();
  if (len == 255) len = next_4B();
  str.assign(next<char>(len), len);
}

template <class T> const T* pointer_decoder::next(unsigned elements) {
  const T* result = (const T*) data;
  data += sizeof(T) * elements;
  return result;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
