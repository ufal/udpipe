// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "gru_tokenizer_network.h"
#include "utils/binary_encoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

//
// Declarations
//

template <int D>
class gru_tokenizer_network_trainer : public gru_tokenizer_network_implementation<D> {
 public:

  static bool train(unsigned segment, binary_encoder& enc, string& error);
};

//
// Defititions
//

template <int D>
bool gru_tokenizer_network_trainer<D>::train(unsigned int /*segment*/, binary_encoder& enc, string& /*error*/) {
  enc.add_1B(1);
  enc.add_1B(D);
  return false;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
