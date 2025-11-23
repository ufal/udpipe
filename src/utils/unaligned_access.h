// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2017 Institute of Formal and Applied Linguistics, Faculty of
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

template<class T, class P>
inline T unaligned_load(const P* ptr);

template<class T, class P>
inline T unaligned_load_inc(const P*& ptr);

template<class T, class P>
inline void unaligned_store(P* ptr, T value);

template<class T, class P>
inline void unaligned_store_inc(P*& ptr, T value);

template<class T>
T* unaligned_lower_bound(T* first, size_t size, T val);

template<class T>
T* unaligned_upper_bound(T* first, size_t size, T val);

//
// Definitions
//

template<class T, class P>
inline T unaligned_load(const P* ptr) {
  T value;
  memcpy(&value, (const void*)ptr, sizeof(T));
  return value;
}

template<class T, class P>
inline T unaligned_load_inc(const P*& ptr) {
  T value;
  memcpy(&value, (const void*)ptr, sizeof(T));
  ((const char*&)ptr) += sizeof(T);
  return value;
}

template<class T, class P>
inline void unaligned_store(P* ptr, T value) {
  memcpy((void*)ptr, &value, sizeof(T));
}

template<class T, class P>
inline void unaligned_store_inc(P*& ptr, T value) {
  memcpy((void*)ptr, &value, sizeof(T));
  ((char*&)ptr) += sizeof(T);
}

template<class T>
T* unaligned_lower_bound(T* first, size_t size, T val) {
  while (size) {
    size_t step = size >> 1;
    if (unaligned_load<T>(first + step) < val) {
      first += step + 1;
      size -= step + 1;
    } else {
      size = step;
    }
  }
  return first;
}

template<class T>
T* unaligned_upper_bound(T* first, size_t size, T val) {
  while (size) {
    size_t step = size >> 1;
    if (!(val < unaligned_load<T>(first + step))) {
      first += step + 1;
      size -= step + 1;
    } else {
      size = step;
    }
  }
  return first;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
