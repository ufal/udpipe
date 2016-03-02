// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"

namespace ufal {
namespace udpipe {

class named_options {
 public:
  typedef unordered_map<string, string> map;

  static bool parse(const string& options, map& parsed_options, string& error);
};

} // namespace udpipe
} // namespace ufal
