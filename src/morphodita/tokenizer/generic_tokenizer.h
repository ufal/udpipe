// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "ragel_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class generic_tokenizer : public ragel_tokenizer {
 public:
  enum { LATEST = 1 };
  generic_tokenizer(unsigned /*version*/) : ragel_tokenizer(1) {}

  virtual bool next_sentence(vector<token_range>& tokens) override;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
