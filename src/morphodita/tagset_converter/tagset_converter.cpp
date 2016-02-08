// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "identity_tagset_converter.h"
#include "pdt_to_conll2009_tagset_converter.h"
#include "strip_lemma_comment_tagset_converter.h"
#include "strip_lemma_id_tagset_converter.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tagset_converter* tagset_converter::new_identity_converter() {
  return new identity_tagset_converter();
}

tagset_converter* tagset_converter::new_pdt_to_conll2009_converter() {
  return new pdt_to_conll2009_tagset_converter();
}

tagset_converter* tagset_converter::new_strip_lemma_comment_converter(const morpho& dictionary) {
  return new strip_lemma_comment_tagset_converter(dictionary);
}

tagset_converter* tagset_converter::new_strip_lemma_id_converter(const morpho& dictionary) {
  return new strip_lemma_id_tagset_converter(dictionary);
}

tagset_converter* new_tagset_converter(const string& name, const morpho& dictionary) {
  if (name == "pdt_to_conll2009") return tagset_converter::new_pdt_to_conll2009_converter();
  if (name == "strip_lemma_comment") return tagset_converter::new_strip_lemma_comment_converter(dictionary);
  if (name == "strip_lemma_id") return tagset_converter::new_strip_lemma_id_converter(dictionary);
  return nullptr;
}

void tagset_converter_unique_analyzed(vector<tagged_lemma>& tagged_lemmas) {
  // Remove possible lemma-tag pair duplicates
  struct tagged_lemma_comparator {
    inline static bool eq(const tagged_lemma& a, const tagged_lemma& b) { return a.lemma == b.lemma && a.tag == b.tag; }
    inline static bool lt(const tagged_lemma& a, const tagged_lemma& b) { int lemma_compare = a.lemma.compare(b.lemma); return lemma_compare < 0 || (lemma_compare == 0 && a.tag < b.tag); }
  };

  sort(tagged_lemmas.begin(), tagged_lemmas.end(), tagged_lemma_comparator::lt);
  tagged_lemmas.resize(unique(tagged_lemmas.begin(), tagged_lemmas.end(), tagged_lemma_comparator::eq) - tagged_lemmas.begin());
}

void tagset_converter_unique_generated(vector<tagged_lemma_forms>& forms) {
  // Regroup and if needed remove duplicate form-tag pairs for each lemma
  for (unsigned i = 0; i < forms.size(); i++) {
    bool any_merged = false;
    for (unsigned j = forms.size() - 1; j > i; j--)
      if (forms[j].lemma == forms[i].lemma) {
        // Same lemma was found. Merge form-tag pairs
        for (auto&& tagged_form : forms[j].forms)
          forms[i].forms.emplace_back(move(tagged_form));

        // Remove lemma j by moving it to end and deleting
        if (j < forms.size() - 1) {
          forms[j].lemma.swap(forms[forms.size() - 1].lemma);
          forms[j].forms.swap(forms[forms.size() - 1].forms);
        }
        forms.pop_back();
        any_merged = true;
      }

    if (any_merged && forms[i].forms.size() > 1) {
      // Remove duplicate form-tag pairs
      struct tagged_form_comparator {
        inline static bool eq(const tagged_form& a, const tagged_form& b) { return a.tag == b.tag && a.form == b.form; }
        inline static bool lt(const tagged_form& a, const tagged_form& b) { int tag_compare = a.tag.compare(b.tag); return tag_compare < 0 || (tag_compare == 0 && a.form < b.form); }
      };

      sort(forms[i].forms.begin(), forms[i].forms.end(), tagged_form_comparator::lt);
      forms[i].forms.resize(unique(forms[i].forms.begin(), forms[i].forms.end(), tagged_form_comparator::eq) - forms[i].forms.begin());
    }
  }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
