// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "generic_tokenizer.h"
#include "generic_tokenizer_factory.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tokenizer* generic_tokenizer_factory::new_tokenizer() const {
  return new generic_tokenizer(version);
}

bool generic_tokenizer_factory::load(istream& is) {
  version = is.get();

  return bool(is);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
