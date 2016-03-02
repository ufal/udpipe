// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "named_options.h"

namespace ufal {
namespace udpipe {

bool named_options::parse(const string& /*options*/, map& /*parsed_options*/, string& error) {
  error.clear();

  // TODO
  return false;
}

} // namespace udpipe
} // namespace ufal
