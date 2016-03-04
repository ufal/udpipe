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

// Read paragraph until EOF or end line. All encountered \n are stored.
inline istream& getpara(istream& is, string& para);

//
// Definitions
//

istream& getpara(istream& is, string& para) {
  para.clear();

  for (string line; getline(is, line); ) {
    para.append(line);
    para.push_back('\n');

    if (line.empty()) break;
  }

  if (is.eof() && !para.empty()) is.clear(istream::eofbit);
  return is;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
