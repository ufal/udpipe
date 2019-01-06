// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2019 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "czech_tokenizer.h"
#include "czech_tokenizer_factory.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tokenizer* czech_tokenizer_factory::new_tokenizer(const morpho* m) const {
  return new czech_tokenizer(language, version, m);
}

bool czech_tokenizer_factory::load(istream& is) {
  language = czech_tokenizer::tokenizer_language(is.get());
  version = is.get();

  return bool(is) && (language == czech_tokenizer::CZECH || language == czech_tokenizer::SLOVAK);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
