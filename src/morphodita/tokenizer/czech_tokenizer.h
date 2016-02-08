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
#include "morphodita/morpho/morpho.h"
#include "ragel_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class czech_tokenizer : public ragel_tokenizer {
 public:
  enum tokenizer_language { CZECH = 0, SLOVAK = 1 };
  enum { LATEST = 1 };
  czech_tokenizer(tokenizer_language language, unsigned version, const morpho* m = nullptr);

  virtual bool next_sentence(vector<token_range>& tokens) override;

 private:
  const morpho* m;
  const unordered_set<string>* abbreviations;
  vector<tagged_lemma> lemmas;

  void merge_hyphenated(vector<token_range>& tokens);

  static const unordered_set<string> abbreviations_czech;
  static const unordered_set<string> abbreviations_slovak;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
