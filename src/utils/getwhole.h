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

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

// Read whole content until EOF. All encountered \n are stored.
inline istream& getwhole(istream& is, string& whole);

//
// Definitions
//

istream& getwhole(istream& is, string& whole) {
  whole.clear();

  for (string line; getline(is, line); )
    whole.append(line).push_back('\n');

  if (is.eof() && !whole.empty()) is.clear(istream::eofbit);
  return is;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
