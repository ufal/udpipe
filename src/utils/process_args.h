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

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

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

// Call a given processor on specified arguments. Every argument is name of
// input file; output is written to the file with specified name template,
// which can contain {} for base name of the input file (without directories
// and extension). If no input files are specified, stdin is used; if no output
// template is specified, stdout is used.
template <class T, class... U>
void process_args_with_output_template(bool binary_input, bool binary_output, int argi, int argc, char* argv[],
                                       const string& output_template, T processor, U&&... processor_args) {
  bool output_per_file = output_template.find("{}") != string::npos;
  if (argi == argc && output_per_file) runtime_failure("Cannot use output template with {} when reading standard input!");

  ofstream output_file;
  string input_file_root, output_file_name;

  // Set binary on stdin/stdout if required
  if (binary_input && argi == argc) {
#ifdef _WIN32
      _setmode(_fileno(stdin), _O_BINARY);
#endif
  }

  if (binary_output && output_template.empty()) {
#ifdef _WIN32
      _setmode(_fileno(stdout), _O_BINARY);
#endif
  }

  for (int i = argi; i < argc || i == argi; i++) {
    // Open input file
    ifstream input_file;
    if (i < argc) {
      input_file.open(argv[i], binary_input ? ifstream::binary : ifstream::in);
      if (!input_file.is_open()) runtime_failure("Cannot open input file '" << argv[i] << "'!");

      input_file_root.assign(argv[i]);
      auto directory = input_file_root.find_last_of("/\\");
      if (directory != string :: npos) input_file_root.erase(0, directory + 1);
      auto extension = input_file_root.rfind('.');
      if (extension != string::npos) input_file_root.erase(extension);
    }
    istream& input = i < argc ? input_file : cin;

    // Open output file
    if (!output_template.empty() && (i == argi || output_per_file)) {
      output_file_name.assign(output_template);
      for (auto index = string::npos; (index = output_file_name.find("{}")) != string::npos; )
        output_file_name.replace(index, 2, input_file_root);
      output_file.open(output_file_name.c_str(), binary_output ? ofstream::binary : ofstream::out);
      if (!output_file.is_open()) runtime_failure("Cannot open output file '" << output_file_name << "'!");
    }
    ostream& output = !output_template.empty() ? output_file : cout;

    // Process the data
    processor(input, output, i < argc ? argv[i] : "", output_template, std::forward<U>(processor_args)...);

    // Close the file if needed
    if (output_per_file) {
      output_file.close();
      if (!output_file) runtime_failure("Cannot close output file '" << output_file_name << "'!");
    }
  }
}

template <class T, class... U>
void process_args_with_output_template(int argi, int argc, char* argv[], const string& output_template, T processor, U&&... processor_args) {
  process_args_with_output_template(false, false, argi, argc, argv, output_template, processor, std::forward<U>(processor_args)...);
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
