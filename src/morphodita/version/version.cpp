// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <sstream>

#include "unilib/version.h"
#include "version.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

version version::current() {
  return {1, 9, 1, "devel"};
}

// Returns multi-line formated version and copyright string.
string version::version_and_copyright(const string& other_libraries) {
  ostringstream info;

  auto morphodita = version::current();
  auto unilib = unilib::version::current();

  info << "MorphoDiTa version " << morphodita.major << '.' << morphodita.minor << '.' << morphodita.patch
       << (morphodita.prerelease.empty() ? "" : "-") << morphodita.prerelease
       << " (using UniLib " << unilib.major << '.' << unilib.minor << '.' << unilib.patch
       << (other_libraries.empty() ? "" : " and ") << other_libraries << ")\n"
          "Copyright 2015 by Institute of Formal and Applied Linguistics, Faculty of\n"
          "Mathematics and Physics, Charles University in Prague, Czech Republic.";

  return info.str();
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
