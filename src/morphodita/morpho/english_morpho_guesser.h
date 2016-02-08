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
#include "morpho.h"
#include "persistent_unordered_map.h"
#include "utils/binary_decoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class english_morpho_guesser {
 public:
  void load(binary_decoder& data);
  void analyze(string_piece form, string_piece form_lc, vector<tagged_lemma>& lemmas) const;
  bool analyze_proper_names(string_piece form, string_piece form_lc, vector<tagged_lemma>& lemmas) const;

 private:
  inline void add(const string& tag, const string& form, vector<tagged_lemma>& lemmas) const;
  inline void add(const string& tag, const string& tag2, const string& form, vector<tagged_lemma>& lemmas) const;
  inline void add(const string& tag, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  inline void add(const string& tag, const string& tag2, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  void add_NNS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  void add_NNPS(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_VBG(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_VBD_VBN(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_VBZ(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_JJR_RBR(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  void add_JJS_RBS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;

  enum { NEGATION_LEN = 0, TO_FOLLOW = 1, TOTAL = 2 };
  vector<string> exceptions_tags;
  persistent_unordered_map exceptions;
  persistent_unordered_map negations;
  string CD = "CD", FW = "FW", JJ = "JJ", JJR = "JJR", JJS = "JJS",
         NN = "NN", NNP = "NNP", NNPS = "NNPS", NNS = "NNS", RB = "RB",
         RBR = "RBR", RBS = "RBS", SYM = "SYM", VB = "VB", VBD = "VBD",
         VBG = "VBG", VBN = "VBN", VBP = "VBP", VBZ = "VBZ";
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
