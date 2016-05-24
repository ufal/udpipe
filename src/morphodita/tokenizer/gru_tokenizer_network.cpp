// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "gru_tokenizer_network.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

gru_tokenizer_network* gru_tokenizer_network::load(binary_decoder& data) {
  if (data.next_1B() != 1) return nullptr;
  switch (data.next_1B()) {
    case 16: return gru_tokenizer_network_implementation<16>::load(data);
    case 20: return gru_tokenizer_network_implementation<20>::load(data);
    case 24: return gru_tokenizer_network_implementation<24>::load(data);
  }
  return nullptr;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
