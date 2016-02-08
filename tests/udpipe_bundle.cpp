// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "udpipe.h"

using namespace ufal::udpipe;
using namespace std;

int main() {
  version udpipe = version::current();

  cout << "UDPipe " << udpipe.major << '.' << udpipe.minor << '.' << udpipe.patch
      << (!udpipe.prerelease.empty() ? "-" : "") << udpipe.prerelease << '.' << endl;

  return 0;
}
