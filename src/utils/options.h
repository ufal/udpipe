// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

class options {
 public:
  typedef unordered_map<string, string> map;

  struct value {
    enum allowed_t { NONE, ANY, SET };
    allowed_t allowed;
    unordered_set<string> set;

    value(initializer_list<string> set) : allowed(SET), set(set) {}
    static const value none;
    static const value any;

   private:
    value(allowed_t allowed) : allowed(allowed) {}
  };

  // Parse options according to allowed map. If successful, argv is reordered so
  // that non-option arguments are placed in argv[1] to argv[argc-1]. The '--'
  // indicates end of option arguments (as usual).  The allowed map contains
  // values allowed for every option. If empty, no value is allowed, if it
  // contains just an empty string, any value is allowed.
  static bool parse(const unordered_map<string, value>& allowed, int& argc, char**& argv, map& options);
};


} // namespace utils
} // namespace udpipe
} // namespace ufal
