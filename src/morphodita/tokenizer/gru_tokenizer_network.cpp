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

gru_tokenizer_network* gru_tokenizer_network::load(istream& is) {
  char version; if (!is.get(version)) return nullptr;
  char dim; if (!is.get(dim)) return nullptr;

  unique_ptr<gru_tokenizer_network> network;
  if (dim == 24) network.reset(gru_tokenizer_network_implementation<24>::load(is));
  else return nullptr;

  return network.release();
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
