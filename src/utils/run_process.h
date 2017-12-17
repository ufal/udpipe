// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2017 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdio>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

// Run given process with given arguments. Optionally pass given standard input,
// and capture standard output and standard error. Currently, only standard output
// is supported, both standard input and standard error are not.
inline bool run_process(const vector<string>& command_and_arguments, const string* input, string* output, string* error);

//
// Definitions
//

inline bool run_process(const vector<string>& command_and_arguments, const string* input, string* output, string* error) {
  // Current limitations; could be lifted if the functionality is required.
  if (input) return false;
  if (error) return false;

  // Clear output parameters
  if (output) output->clear();

  // Construct the command line
  string command;
#ifdef _WIN32
  // Escape so that CommandLineToArgv works as expected, and then escape cmd metacharacters.
  string arg_need_quoting = " \t\n\v\"";
  string cmd_need_quoting = "()%!^\"<>&|";

  for (auto&& str : command_and_arguments) {
    if (command.empty()) {
      // Do not escape the command, it makes things *very* complicated, because
      // cmd then requires at least a set of double quotes around the whole command-line.
      command.assign(str);
    } else {
      command.push_back(' ');

      unsigned backslashes = 0;
      bool quote = str.find_first_of(arg_need_quoting) != string::npos;

      if (quote) command.append("^\"");
      for (auto&& chr : str) {
        if (chr == '\\') {
          backslashes++;
        } else if (chr == '\"') {
          command.append(2 * backslashes + 1, '\\').append("^\"");
          backslashes = 0;
        } else {
          if (backslashes) command.append(backslashes, '\\');
          backslashes = 0;
          if (cmd_need_quoting.find(chr) != string::npos) command.push_back('^');
          command.push_back(chr);
        }
      }
      if (backslashes) command.append(2 * backslashes, '\\');
      if (quote) command.append("^\"");
    }
  }

#else
  // Escape using single quotes, using '"'"' to escape in-argument single quotes
  for (auto&& str : command_and_arguments) {
    if (!command.empty())
      command.push_back(' ');

    command.push_back('\'');
    for (auto&& chr : str)
      if (chr == '\'')
        command.append("'\"'\"'");
      else
        command.push_back(chr);
    command.push_back('\'');
  }
#endif

  // Run popen
#ifdef _WIN32
  struct popen_closer { void operator()(FILE* pipe) { if (pipe) _pclose(pipe); } };
  unique_ptr<FILE, popen_closer> standard_output(_popen(command.c_str(), "r"));
#else
  struct popen_closer { void operator()(FILE* pipe) { if (pipe) pclose(pipe); } };
  unique_ptr<FILE, popen_closer> standard_output(popen(command.c_str(), "r"));
#endif
  if (!standard_output.get()) return false;

  char buffer[128];
  while (fgets(buffer, 128, standard_output.get()))
    if (output)
      output->append(buffer);
  if (!feof(standard_output.get())) return false;

#ifdef _WIN32
  if (_pclose(standard_output.release()) != 0) return false;
#else
  if (pclose(standard_output.release()) != 0) return false;
#endif

  return true;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
