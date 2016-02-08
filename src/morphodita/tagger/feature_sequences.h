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
#include "elementary_features.h"
#include "morphodita/morpho/persistent_unordered_map.h"
#include "morphodita/morpho/small_stringops.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"
#include "vli.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
typedef int32_t feature_sequence_score;
typedef int64_t feature_sequences_score;

struct feature_sequence_element {
  elementary_feature_type type;
  int elementary_index;
  int sequence_index;

  feature_sequence_element() {}
  feature_sequence_element(elementary_feature_type type, int elementary_index, int sequence_index) : type(type), elementary_index(elementary_index), sequence_index(sequence_index) {}
};

struct feature_sequence {
  vector<feature_sequence_element> elements;
  int dependant_range = 1;
};

template <class ElementaryFeatures, class Map>
class feature_sequences {
 public:
  typedef typename ElementaryFeatures::per_form_features per_form_features;
  typedef typename ElementaryFeatures::per_tag_features per_tag_features;
  typedef typename ElementaryFeatures::dynamic_features dynamic_features;

  void parse(int window_size, istream& is);
  bool load(istream& is);
  bool save(ostream& os);

  struct cache;

  inline void initialize_sentence(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, cache& c) const;
  inline void compute_dynamic_features(int form_index, int tag_index, const dynamic_features* prev_dynamic, dynamic_features& dynamic, cache& c) const;
  inline feature_sequences_score score(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, cache& c) const;
  void feature_keys(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, vector<string>& keys, cache& c) const;

  ElementaryFeatures elementary;
  vector<Map> scores;
  vector<feature_sequence> sequences;
};

class persistent_feature_sequence_map : public persistent_unordered_map {
 public:
  persistent_feature_sequence_map() : persistent_unordered_map() {}
  persistent_feature_sequence_map(const persistent_unordered_map&& map) : persistent_unordered_map(map) {}

  feature_sequence_score score(const char* feature, int len) const {
    auto* it = at_typed<feature_sequence_score>(feature, len);
    return it ? *it : 0;
  }
};

template <class ElementaryFeatures> using persistent_feature_sequences = feature_sequences<ElementaryFeatures, persistent_feature_sequence_map>;


// Definitions
template <class ElementaryFeatures, class Map>
inline bool feature_sequences<ElementaryFeatures, Map>::load(istream& is) {
  if (!elementary.load(is)) return false;

  binary_decoder data;
  if (!compressor::load(is, data)) return false;

  try {
    sequences.resize(data.next_1B());
    for (auto&& sequence : sequences) {
      sequence.dependant_range = data.next_4B();
      sequence.elements.resize(data.next_1B());
      for (auto&& element : sequence.elements) {
        element.type = elementary_feature_type(data.next_4B());
        element.elementary_index = data.next_4B();
        element.sequence_index = data.next_4B();
      }
    }

    scores.resize(data.next_1B());
    for (auto&& score : scores)
      score.load(data);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

template <class ElementaryFeatures, class Map>
struct feature_sequences<ElementaryFeatures, Map>::cache {
  const vector<string_piece>* forms;
  const vector<vector<tagged_lemma>>* analyses;
  vector<per_form_features> elementary_per_form;
  vector<vector<per_tag_features>> elementary_per_tag;

  struct cache_element {
    vector<char> key;
    int key_size;
    feature_sequence_score score;

    cache_element(int elements) : key(vli<elementary_feature_value>::max_length() * elements), key_size(0), score(0) {}
  };
  vector<cache_element> caches;
  vector<const per_tag_features*> window;
  vector<char> key;
  feature_sequences_score score;

  cache(const feature_sequences<ElementaryFeatures, Map>& self) : score(0) {
    caches.reserve(self.sequences.size());
    int max_sequence_elements = 0, max_window_size = 1;
    for (auto&& sequence : self.sequences) {
      caches.emplace_back(sequence.elements.size());
      if (int(sequence.elements.size()) > max_sequence_elements) max_sequence_elements = sequence.elements.size();
      for (auto&& element : sequence.elements)
        if (element.type == PER_TAG && 1 - element.sequence_index > max_window_size)
          max_window_size = 1 - element.sequence_index;
    }
    key.resize(max_sequence_elements * vli<elementary_feature_value>::max_length());
    window.resize(max_window_size);
  }
};

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::initialize_sentence(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, cache& c) const {
  // Store forms and forms_size
  c.forms = &forms;
  c.analyses = &analyses;

  // Enlarge elementary features vectors if needed
  if (forms.size() > c.elementary_per_form.size()) c.elementary_per_form.resize(forms.size() * 2);
  if (forms.size() > c.elementary_per_tag.size()) c.elementary_per_tag.resize(forms.size() * 2);
  for (unsigned i = 0; i < forms.size(); i++)
    if (analyses[i].size() > c.elementary_per_tag[i].size())
      c.elementary_per_tag[i].resize(analyses[i].size() * 2);

  // Compute elementary features
  elementary.compute_features(forms, analyses, c.elementary_per_form, c.elementary_per_tag);

  // Clear score cache, because scores may have been modified
  c.score = 0;
  for (auto&& cache : c.caches)
    cache.key_size = cache.score = 0;
}

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::compute_dynamic_features(int form_index, int tag_index, const dynamic_features* prev_dynamic, dynamic_features& dynamic, cache& c) const {
  elementary.compute_dynamic_features((*c.analyses)[form_index][tag_index], c.elementary_per_form[form_index], c.elementary_per_tag[form_index][tag_index], form_index > 0 ? prev_dynamic : nullptr, dynamic);
}

template <class ElementaryFeatures, class Map>
feature_sequences_score feature_sequences<ElementaryFeatures, Map>::score(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, cache& c) const {
  // Start by creating a window of per_tag_features*
  for (int i = 0; i < int(c.window.size()) && form_index - i >= 0; i++)
    c.window[i] = &c.elementary_per_tag[form_index - i][tags_window[i]];

  // Compute the score
  feature_sequences_score result = c.score;
  for (unsigned i = 0; i < sequences.size(); i++) {
    if (tags_unchanged >= sequences[i].dependant_range)
      break;

    char* key = c.key.data();
    for (unsigned j = 0; j < sequences[i].elements.size(); j++) {
      auto& element = sequences[i].elements[j];
      elementary_feature_value value;

      switch (element.type) {
        case PER_FORM:
          value = form_index + element.sequence_index < 0 || unsigned(form_index + element.sequence_index) >= c.forms->size() ? elementary_feature_empty : c.elementary_per_form[form_index + element.sequence_index].values[element.elementary_index];
          break;
        case PER_TAG:
          value = form_index + element.sequence_index < 0 ? elementary_feature_empty : c.window[-element.sequence_index]->values[element.elementary_index];
          break;
        case DYNAMIC:
        default:
          value = dynamic.values[element.elementary_index];
      }

      if (value == elementary_feature_unknown) {
        key = c.key.data();
        break;
      }
      vli<elementary_feature_value>::encode(value, (unsigned char*&) key);
    }

    result -= c.caches[i].score;
    int key_size = key - c.key.data();
    if (!key_size) {
      c.caches[i].score = 0;
      c.caches[i].key_size = 0;
    } else if (key_size != c.caches[i].key_size || !small_memeq(c.key.data(), c.caches[i].key.data(), key_size)) {
      c.caches[i].score = scores[i].score(c.key.data(), key_size);
      c.caches[i].key_size = key_size;
      small_memcpy(c.caches[i].key.data(), c.key.data(), key_size);
    }
    result += c.caches[i].score;
  }

  c.score = result;
  return result;
}

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::feature_keys(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, vector<string>& keys, cache& c) const {
  score(form_index, tags_window, tags_unchanged, dynamic, c);

  keys.resize(c.caches.size());
  for (unsigned i = 0; i < c.caches.size(); i++)
    keys[i].assign(c.caches[i].key.data(), c.caches[i].key_size);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
