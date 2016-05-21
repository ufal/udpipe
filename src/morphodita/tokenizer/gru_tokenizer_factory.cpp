// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "gru_tokenizer.h"
#include "gru_tokenizer_factory.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tokenizer* gru_tokenizer_factory::new_tokenizer() const {
  return new gru_tokenizer(url_email_tokenizer, *network);
}

bool gru_tokenizer_factory::load(istream& is) {
  char version;
  if (!is.get(version)) return false;

  char url_email;
  if (!is.get(url_email)) return false;
  this->url_email_tokenizer = url_email;

  network.reset(gru_tokenizer_network::load(is));
  if (!network) return false;

  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
