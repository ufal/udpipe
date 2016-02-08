// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "czech_tokenizer.h"
#include "english_tokenizer.h"
#include "generic_tokenizer.h"
#include "vertical_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tokenizer* tokenizer::new_vertical_tokenizer() {
  return new vertical_tokenizer();
}

tokenizer* tokenizer::new_czech_tokenizer() {
  return new czech_tokenizer(czech_tokenizer::CZECH, czech_tokenizer::LATEST);
}

tokenizer* tokenizer::new_english_tokenizer() {
  return new english_tokenizer(english_tokenizer::LATEST);
}

tokenizer* tokenizer::new_generic_tokenizer() {
  return new generic_tokenizer(generic_tokenizer::LATEST);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
