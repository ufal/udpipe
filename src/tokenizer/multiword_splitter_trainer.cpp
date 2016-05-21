// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "multiword_splitter_trainer.h"

namespace ufal {
namespace udpipe {

bool multiword_splitter_trainer::train(const vector<sentence>& /*data*/, ostream& os, string& error) {
  error.clear();

  // TODO: Train

  os.put(1);
  // TODO: Encode

  return true;
}

} // namespace udpipe
} // namespace ufal

