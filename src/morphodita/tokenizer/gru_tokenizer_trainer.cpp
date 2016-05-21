// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "gru_tokenizer_network_trainer.h"
#include "gru_tokenizer_trainer.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool gru_tokenizer_trainer::train(unsigned url_email_tokenizer, unsigned segment, unsigned dimension, const vector<tokenized_sentence>& /*data*/, ostream& os, string& error) {
  error.clear();

  binary_encoder enc;
  enc.add_1B(url_email_tokenizer);
  enc.add_2B(segment);
  if (dimension == 24) {
    if (!gru_tokenizer_network_trainer<24>::train(segment, enc, error)) return false;
  } else {
    return error.assign("Gru tokenizer dimension '").append(to_string(dimension)).append("' is not supported!"), false;
  }

  os.put(1);
  if (!compressor::save(os, enc)) return error.assign("Cannot save gru_tokenizer_factory!"), false;
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
