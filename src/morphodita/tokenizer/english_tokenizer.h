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

class english_tokenizer : public ragel_tokenizer {
 public:
  enum { LATEST = 1 };
  english_tokenizer(unsigned version);

  virtual bool next_sentence(vector<token_range>& tokens) override;

 private:
  void split_token(vector<token_range>& tokens);

  static const unordered_set<string> abbreviations;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
