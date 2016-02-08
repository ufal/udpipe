// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
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
namespace morphodita {

class version {
 public:
  unsigned major;
  unsigned minor;
  unsigned patch;
  string prerelease;

  // Returns current MorphoDiTa version.
  static version current();

  // Returns multi-line formated version and copyright string.
  static string version_and_copyright(const string& other_libraries = string());
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
