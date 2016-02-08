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
#include "morpho_dictionary.h"
#include "persistent_unordered_map.h"
#include "small_stringops.h"
#include "utils/binary_decoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class MorphoDictionary>
class morpho_prefix_guesser {
 public:
  morpho_prefix_guesser(const MorphoDictionary& dictionary) : dictionary(dictionary) {}

  void load(binary_decoder& data);
  void analyze(string_piece form, vector<tagged_lemma>& lemmas);
  bool generate(string_piece lemma, const tag_filter& filter, vector<tagged_lemma_forms>& lemmas_forms);

 private:
  const MorphoDictionary& dictionary;
  vector<tag_filter> tag_filters;
  persistent_unordered_map prefixes_initial, prefixes_middle;
};


// Definitions
template <class MorphoDictionary>
void morpho_prefix_guesser<MorphoDictionary>::load(binary_decoder& data) {
  // Load and construct tag filters
  for (unsigned tag_filters_len = data.next_1B(); tag_filters_len; tag_filters_len--) {
    unsigned tag_filter_len = data.next_1B();
    string tag_filter(data.next<char>(tag_filter_len), tag_filter_len);

    tag_filters.emplace_back(tag_filter.c_str());
  }

  // Load prefixes
  prefixes_initial.load(data);
  prefixes_middle.load(data);
}

// Analyze can return non-unique lemma-tag pairs.
template <class MorphoDictionary>
void morpho_prefix_guesser<MorphoDictionary>::analyze(string_piece form, vector<tagged_lemma>& lemmas) {
  if (!form.len) return;

  vector<char> form_tmp;
  vector<unsigned> middle_masks;
  middle_masks.reserve(form.len);

  for (unsigned initial = 0; initial < form.len; initial++) {
    // Match the initial prefix.
    unsigned initial_mask = (1<<tag_filters.size()) - 1; // full mask for empty initial prefix
    if (initial) {
      auto found = prefixes_initial.at_typed<uint32_t>(form.str, initial);
      if (!found) break;
      initial_mask = *found;
    }

    // If we have found an initial prefix (including the empty one), match middle prefixes.
    if (initial_mask) {
      middle_masks.resize(initial);
      middle_masks.emplace_back(initial_mask);
      for (unsigned middle = initial; middle < middle_masks.size(); middle++) {
        if (!middle_masks[middle]) continue;
        // Try matching middle prefixes from current index.
        for (unsigned i = middle + 1; i < form.len; i++) {
          auto found = prefixes_middle.at_typed<uint32_t>(form.str + middle, i - middle);
          if (!found) break;
          if (*found) {
            if (i + 1 > middle_masks.size()) middle_masks.resize(i + 1);
            middle_masks[i] |= middle_masks[middle] & *found;
          }
        }

        // Try matching word forms if at least one middle prefix was found.
        if (middle > initial && middle < form.len ) {
          if (initial) {
            if (form_tmp.empty()) form_tmp.assign(form.str, form.str + form.len);
            small_memcpy(form_tmp.data() + middle - initial, form.str, initial);
          }
          unsigned lemmas_ori_size = lemmas.size();
          dictionary.analyze(string_piece((initial ? form_tmp.data() : form.str) + middle - initial, form.len - middle + initial), lemmas);
          unsigned lemmas_new_size = lemmas_ori_size;
          for (unsigned i = lemmas_ori_size; i < lemmas.size(); i++) {
            for (unsigned filter = 0; filter < tag_filters.size(); filter++)
              if ((middle_masks[middle] & (1<<filter)) && tag_filters[filter].matches(lemmas[i].tag.c_str())) {
                if (i == lemmas_new_size) {
                  lemmas[lemmas_new_size].lemma.insert(0, form.str + initial, middle - initial);
                } else {
                  lemmas[lemmas_new_size].lemma.reserve(lemmas[i].lemma.size() + middle - initial);
                  lemmas[lemmas_new_size].lemma.assign(form.str + initial, middle - initial);
                  lemmas[lemmas_new_size].lemma.append(lemmas[i].lemma);
                  lemmas[lemmas_new_size].tag = lemmas[i].tag;
                }
                lemmas_new_size++;
                break;
              }
          }
          if (lemmas_new_size < lemmas.size()) lemmas.erase(lemmas.begin() + lemmas_new_size, lemmas.end());
        }
      }
    }
  }
}

template <class MorphoDictionary>
bool morpho_prefix_guesser<MorphoDictionary>::generate(string_piece /*lemma*/, const tag_filter& /*filter*/, vector<tagged_lemma_forms>& /*lemmas_forms*/) {
  // Not implemented yet. Is it actually needed?
  return false;
}
} // namespace morphodita
} // namespace udpipe
} // namespace ufal
