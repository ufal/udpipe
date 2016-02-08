// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "strip_lemma_comment_tagset_converter.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

inline bool strip_lemma_comment_tagset_converter::convert_lemma(string& lemma) const {
  unsigned lemma_id_len = dictionary.lemma_id_len(lemma);
  return lemma_id_len < lemma.size() ? (lemma.resize(lemma_id_len), true) : false;
}

void strip_lemma_comment_tagset_converter::convert(tagged_lemma& tagged_lemma) const {
  convert_lemma(tagged_lemma.lemma);
}

void strip_lemma_comment_tagset_converter::convert_analyzed(vector<tagged_lemma>& tagged_lemmas) const {
  bool lemma_changed = false;

  for (auto&& tagged_lemma : tagged_lemmas)
    lemma_changed |= convert_lemma(tagged_lemma.lemma);

  // If no lemma was changed or there is 1 analysis, no duplicates could be created.
  if (!lemma_changed || tagged_lemmas.size() < 2) return;

  tagset_converter_unique_analyzed(tagged_lemmas);
}

void strip_lemma_comment_tagset_converter::convert_generated(vector<tagged_lemma_forms>& forms) const {
  bool lemma_changed = false;

  for (auto&& tagged_lemma_forms : forms)
    lemma_changed |= convert_lemma(tagged_lemma_forms.lemma);

  // If no lemma was changed or there is 1 analysis, no duplicates could be created.
  if (!lemma_changed || forms.size() < 2) return;

  tagset_converter_unique_generated(forms);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
