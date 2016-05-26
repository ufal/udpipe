// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "training_failure.h"

namespace ufal {
namespace udpipe {

training_error::training_error() : runtime_error(message_collector.str()) {
  message_collector.str(string());
}

ostringstream training_error::message_collector;

} // namespace udpipe
} // namespace ufal
