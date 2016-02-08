// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "options.h"

namespace ufal {
namespace udpipe {
namespace utils {

const options::value options::value::none(NONE);
const options::value options::value::any(ANY);

bool options::parse(const unordered_map<string, value>& allowed, int& argc, char**& argv, map& options) {
  int args = 1;
  bool options_allowed = true;
  for (int argi = 1; argi < argc; argi++)
    if (argv[argi][0] == '-' && options_allowed) {
      if (argv[argi][1] == '-' && argv[argi][2] == '\0') {
        options_allowed = false;
        continue;
      }
      const char* option = argv[argi] + 1 + (argv[argi][1] == '-');
      const char* equal_sign = strchr(option, '=');

      string key = equal_sign ? string(option, equal_sign - option) : string(option);
      auto option_info = allowed.find(key);
      if (option_info == allowed.end()) return cerr << "Unknown option '" << argv[argi] << "'." << endl, false;

      string value;
      if (option_info->second.allowed == value::NONE && equal_sign) return cerr << "Option '" << key << "' cannot have value." << endl, false;
      if (option_info->second.allowed != value::NONE) {
        if (equal_sign) {
          value.assign(equal_sign + 1);
        } else {
          if (argi + 1 == argc) return cerr << "Missing value for option '" << key << "'." << endl, false;
          value.assign(argv[++argi]);
        }
        if (!(option_info->second.allowed == value::ANY || (option_info->second.allowed == value::SET && option_info->second.set.count(value))))
          return cerr << "Option '" << key << "' cannot have value '" << value << "'." << endl, false;
      }
      options[key] = value;
    } else {
      argv[args++] = argv[argi];
    }

  argc = args;
  return true;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
