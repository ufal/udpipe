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
#include "generic_lemma_addinfo.h"
#include "morpho_dictionary.h"
#include "morpho_statistical_guesser.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class generic_morpho : public morpho {
 public:
  virtual int analyze(string_piece form, morpho::guesser_mode guesser, vector<tagged_lemma>& lemmas) const override;
  virtual int generate(string_piece lemma, const char* tag_wildcard, guesser_mode guesser, vector<tagged_lemma_forms>& forms) const;
  virtual int raw_lemma_len(string_piece lemma) const override;
  virtual int lemma_id_len(string_piece lemma) const override;
  virtual int raw_form_len(string_piece form) const override;
  virtual tokenizer* new_tokenizer() const override;

  bool load(istream& is);
 private:
  inline void analyze_special(string_piece form, vector<tagged_lemma>& lemmas) const;

  morpho_dictionary<generic_lemma_addinfo> dictionary;
  unique_ptr<morpho_statistical_guesser> statistical_guesser;

  string unknown_tag, number_tag, punctuation_tag, symbol_tag;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
