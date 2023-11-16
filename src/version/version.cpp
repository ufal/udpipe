// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <sstream>

#include "morphodita/version/version.h"
#include "parsito/version/version.h"
#include "unilib/version.h"
#include "version.h"

namespace ufal {
namespace udpipe {

// Returns current version.
version version::current() {
  return {1, 3, 2, "dev"};
}

// Returns multi-line formated version and copyright string.
string version::version_and_copyright(const string& other_libraries) {
  ostringstream info;

  auto udpipe = version::current();
  auto unilib = unilib::version::current();
  auto morphodita = morphodita::version::current();
  auto parsito = parsito::version::current();

  info << "UDPipe version " << udpipe.major << '.' << udpipe.minor << '.' << udpipe.patch
       << (udpipe.prerelease.empty() ? "" : "-") << udpipe.prerelease
       << " (using UniLib " << unilib.major << '.' << unilib.minor << '.' << unilib.patch
       << (unilib.prerelease.empty() ? "" : "-") << unilib.prerelease
       << ",\nMorphoDiTa " << morphodita.major << '.' << morphodita.minor << '.' << unilib.patch
       << (morphodita.prerelease.empty() ? "" : "-") << morphodita.prerelease
       << ", Parsito " << parsito.major << '.' << parsito.minor << '.' << unilib.patch
       << (parsito.prerelease.empty() ? "" : "-") << parsito.prerelease
       << (other_libraries.empty() ? "" : " and ") << other_libraries << ")\n"
          "Copyright 2016 by Institute of Formal and Applied Linguistics, Faculty of\n"
          "Mathematics and Physics, Charles University in Prague, Czech Republic.";

  return info.str();
}

} // namespace udpipe
} // namespace ufal
