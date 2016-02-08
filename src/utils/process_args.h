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
#include <fstream>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

// Call a given processor on specified arguments. Every argument can be
// either input_file or input_file:output_file. If not output_file is specified,
// stdout is used. If there are no arguments at all, stdin and stdout are used.
template <class T, class... U>
void process_args(int argi, int argc, char* argv[], T processor, U&&... processor_args) {
  if (argi >= argc) {
    processor(cin, cout, std::forward<U>(processor_args)...);
  } else for (; argi < argc; argi++) {
    char* file_in = argv[argi];
    char* file_out = strchr(file_in
#ifdef _WIN32
                            + ((((file_in[0] >= 'A' && file_in[0] <= 'Z') || (file_in[0] >= 'a' && file_in[0] <= 'z')) &&
                                file_in[1] == ':' &&
                                (file_in[2] == '/' || file_in[2] == '\\')
                               ) ? 3 : 0)
#endif
                            , ':');
    if (file_out) *file_out++ = '\0';

    ifstream in(file_in);
    if (!in) runtime_failure("Cannot open file '" << file_in << "' for reading!");

    ofstream out;
    if (file_out) {
      out.open(file_out);
      if (!out) runtime_failure("Cannot open file '" << file_out << "' for writing!");
    }

    processor(in, file_out ? out : cout, std::forward<U>(processor_args)...);
  }
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
