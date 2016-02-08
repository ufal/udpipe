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
inline bool small_memeq(const void* a, const void* b, size_t len);
inline void small_memcpy(void* dest, const void* src, size_t len);

// Definitions
bool small_memeq(const void* a_void, const void* b_void, size_t len) {
  const char* a = (const char*)a_void;
  const char* b = (const char*)b_void;

  while (len--)
    if (*a++ != *b++)
      return false;
  return true;
}

void small_memcpy(void* dest_void, const void* src_void, size_t len) {
  char* dest = (char*)dest_void;
  const char* src = (const char*)src_void;

  while (len--)
    *dest++ = *src++;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
