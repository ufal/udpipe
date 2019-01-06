// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2019 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "czech_tokenizer.h"
#include "tokenizer_factory.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class czech_tokenizer_factory : public tokenizer_factory {
 public:
  // Construct a new tokenizer instance.
  virtual tokenizer* new_tokenizer(const morpho* m) const override;

  bool load(istream& is);
 private:
  czech_tokenizer::tokenizer_language language;
  unsigned version;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
