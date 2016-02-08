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
#include "english_lemma_addinfo.h"
#include "english_morpho_guesser.h"
#include "morpho_dictionary.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class english_morpho : public morpho {
 public:
  english_morpho(unsigned version) : version(version) {}

  virtual int analyze(string_piece form, morpho::guesser_mode guesser, vector<tagged_lemma>& lemmas) const override;
  virtual int generate(string_piece lemma, const char* tag_wildcard, guesser_mode guesser, vector<tagged_lemma_forms>& forms) const;
  virtual int raw_lemma_len(string_piece lemma) const override;
  virtual int lemma_id_len(string_piece lemma) const override;
  virtual int raw_form_len(string_piece form) const override;
  virtual tokenizer* new_tokenizer() const override;

  bool load(istream& is);
 private:
  inline void analyze_special(string_piece form, vector<tagged_lemma>& lemmas) const;

  unsigned version;
  morpho_dictionary<english_lemma_addinfo> dictionary;
  english_morpho_guesser morpho_guesser;

  string unknown_tag = "UNK";
  string number_tag = "CD", nnp_tag = "NNP", ls_tag = "LS";
  string open_quotation_tag = "``", close_quotation_tag = "''";
  string open_parenthesis_tag = "(", close_parenthesis_tag = ")";
  string comma_tag = ",", dot_tag = ".", punctuation_tag = ":", hash_tag = "#", dollar_tag = "$";
  string sym_tag = "SYM", jj_tag = "JJ", nn_tag = "NN", nns_tag = "NNS", cc_tag = "CC", pos_tag = "POS", in_tag = "IN";
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
