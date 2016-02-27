// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "trainer.h"

namespace ufal {
namespace udpipe {

bool trainer::train(const string& /*data*/, const string& /*tokenizer*/, const string& /*tagger*/, const string& /*parser*/, ostream& os, string& error) {
  error.clear();

  // Save model ID
  string id = "morphodita_parsito";
  os.put(id.size());
  os.write(id.c_str(), id.size());

  // TODO train the model

  return true;
}

} // namespace udpipe
} // namespace ufal
