// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <sstream>

#include "trainer.h"
#include "trainer_morphodita_parsito.h"

namespace ufal {
namespace udpipe {

const string trainer::DEFAULT;
const string trainer::NONE = "none";

bool trainer::train(const string& method, const vector<sentence>& training, const vector<sentence>& heldout,
                    const string& tokenizer, const string& tagger, const string& parser, ostream& os, string& error) {
  error.clear();

  stringstream os_buffer;
  os_buffer.put(method.size());
  os_buffer.write(method.c_str(), method.size());

  if (method == "morphodita_parsito") {
    if (!trainer_morphodita_parsito::train(training, heldout, tokenizer, tagger, parser, os_buffer, error))
      return false;
  } else {
    error.assign("Unknown UDPipe method '").append(method).append("'!");
    return false;
  }

  os << os_buffer.rdbuf();
  return true;
}

} // namespace udpipe
} // namespace ufal
