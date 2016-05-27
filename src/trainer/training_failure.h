// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <sstream>
#include <stdexcept>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

class training_error : public runtime_error {
 public:
  training_error();

  static ostringstream message_collector;
};

#define training_failure(message) throw (training_error::message_collector << message, training_error())

} // namespace utils
} // namespace udpipe
} // namespace ufal
