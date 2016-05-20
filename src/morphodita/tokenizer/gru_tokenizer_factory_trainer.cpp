// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "gru_tokenizer_factory_trainer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool gru_tokenizer_factory_trainer::train(unsigned version, unsigned url_email_tokenizer, const vector<tokenized_sentence>& /*data*/, ostream& os, string& error) {
  error.clear();

  os.put(version);
  os.put(url_email_tokenizer);

  // TODO

  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
