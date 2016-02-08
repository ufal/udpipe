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
  static void encode(T value, unsigned char*& where);
  static T decode(const unsigned char*& from);
};


// Definitions
template <>
inline int vli<uint32_t>::max_length() {
  return 5;
}

template <>
inline void vli<uint32_t>::encode(uint32_t value, unsigned char*& where) {
  if (value < 0x80) *where++ = value;
  else if (value < 0x4000) *where++ = (value >> 7) | 0x80, *where++ = value & 0x7F;
  else if (value < 0x200000) *where++ = (value >> 14) | 0x80, *where++ = ((value >> 7) & 0x7F) | 0x80, *where++ = value & 0x7F;
  else if (value < 0x10000000) *where++ = (value >> 21) | 0x80, *where++ = ((value >> 14) & 0x7F) | 0x80, *where++ = ((value >> 7) & 0x7F) | 0x80, *where++ = value & 0x7F;
  else *where++ = (value >> 28) | 0x80, *where++ = ((value >> 21) & 0x7F) | 0x80, *where++ = ((value >> 14) & 0x7F) | 0x80, *where++ = ((value >> 7) & 0x7F) | 0x80, *where++ = value & 0x7F;
}

template <>
inline uint32_t vli<uint32_t>::decode(const unsigned char*& from) {
  uint32_t value = 0;
  while (*from & 0x80) value = (value << 7) | ((*from++) ^ 0x80);
  value = (value << 7) | (*from++);
  return value;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
