// This file is part of UniLib <http://github.com/ufal/unilib/>.
//
// Copyright 2014 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// UniLib version: 3.1.0
// Unicode version: 8.0.0

#include "version.h"

namespace ufal {
namespace udpipe {
namespace unilib {

// Returns current version.
version version::current() {
  return {3, 1, 0, ""};
}

} // namespace unilib
} // namespace udpipe
} // namespace ufal
