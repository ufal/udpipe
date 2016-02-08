// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <atomic>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

template <class T>
class threadsafe_stack {
 public:
  inline void push(T* t);
  inline T* pop();

 private:
  vector<unique_ptr<T>> stack;
  atomic_flag lock = ATOMIC_FLAG_INIT;
};

//
// Definitions
//

template <class T>
void threadsafe_stack<T>::push(T* t) {
  while (lock.test_and_set(memory_order_acquire)) {}
  stack.emplace_back(t);
  lock.clear(memory_order_release);
}

template <class T>
T* threadsafe_stack<T>::pop() {
  T* res = nullptr;

  while (lock.test_and_set(memory_order_acquire)) {}
  if (!stack.empty()) {
    res = stack.back().release();
    stack.pop_back();
  }
  lock.clear(memory_order_release);

  return res;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
