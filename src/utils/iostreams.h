// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

// Initialize iostream and turn off stdio synchronization.
inline void iostreams_init();

// Use binary mode on cin.
inline void iostreams_init_binary_input();

// Use binary mode on cout.
inline void iostreams_init_binary_output();

// Read paragraph until EOF or end line. All encountered \n are stored.
inline istream& getpara(istream& is, string& para);

//
// Definitions
//

void iostreams_init() {
  iostream::sync_with_stdio(false);
}

void iostreams_init_binary_input() {
#ifdef _WIN32
  _setmode(_fileno(stdin), _O_BINARY);
#endif
}

void iostreams_init_binary_output() {
#ifdef _WIN32
  _setmode(_fileno(stdout), _O_BINARY);
#endif
}

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
