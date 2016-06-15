// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class T>
class vli {
 public:
  static int max_length();
  static void encode(T value, char*& where);
  static T decode(const char*& from);
};


// Definitions
template <>
inline int vli<uint32_t>::max_length() {
  return 5;
}

template <>
inline void vli<uint32_t>::encode(uint32_t value, char*& where) {
  if (value < 0x80) *where++ = value;
  else if (value < 0x4000) *where++ = (value >> 7) | 0x80u, *where++ = value & 0x7Fu;
  else if (value < 0x200000) *where++ = (value >> 14) | 0x80u, *where++ = ((value >> 7) & 0x7Fu) | 0x80u, *where++ = value & 0x7Fu;
  else if (value < 0x10000000) *where++ = (value >> 21) | 0x80u, *where++ = ((value >> 14) & 0x7Fu) | 0x80u, *where++ = ((value >> 7) & 0x7Fu) | 0x80u, *where++ = value & 0x7Fu;
  else *where++ = (value >> 28) | 0x80u, *where++ = ((value >> 21) & 0x7Fu) | 0x80u, *where++ = ((value >> 14) & 0x7Fu) | 0x80u, *where++ = ((value >> 7) & 0x7Fu) | 0x80u, *where++ = value & 0x7Fu;
}

template <>
inline uint32_t vli<uint32_t>::decode(const char*& from) {
  uint32_t value = 0;
  while (((unsigned char)(*from)) & 0x80u) value = (value << 7) | (((unsigned char)(*from++)) ^ 0x80u);
  value = (value << 7) | ((unsigned char)(*from++));
  return value;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
