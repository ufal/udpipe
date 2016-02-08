// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <utility>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

template<typename T, typename... Args>
unique_ptr<T> new_unique_ptr(Args&&... args) {
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
