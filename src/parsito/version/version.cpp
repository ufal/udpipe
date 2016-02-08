// This file is part of Parsito <http://github.com/ufal/parsito/>.
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
namespace parsito {

// Returns current version.
version version::current() {
  return {1, 1, 1, "devel"};
}

// Returns multi-line formated version and copyright string.
string version::version_and_copyright(const string& other_libraries) {
  ostringstream info;

  auto parsito = version::current();
  auto unilib = unilib::version::current();

  info << "Parsito version " << parsito.major << '.' << parsito.minor << '.' << parsito.patch
       << (parsito.prerelease.empty() ? "" : "-") << parsito.prerelease
       << " (using UniLib " << unilib.major << '.' << unilib.minor << '.' << unilib.patch
       << (other_libraries.empty() ? "" : " and ") << other_libraries << ")\n"
          "Copyright 2015 by Institute of Formal and Applied Linguistics, Faculty of\n"
          "Mathematics and Physics, Charles University in Prague, Czech Republic.";

  return info.str();
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
