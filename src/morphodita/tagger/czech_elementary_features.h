// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "elementary_features.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "viterbi.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class Map>
class czech_elementary_features : public elementary_features<Map> {
 public:
  czech_elementary_features();

  enum features_per_form { FORM, FOLLOWING_VERB_TAG, FOLLOWING_VERB_LEMMA, NUM, CAP, DASH, PREFIX1, PREFIX2, PREFIX3, PREFIX4, SUFFIX1, SUFFIX2, SUFFIX3, SUFFIX4, PER_FORM_TOTAL };
  enum features_per_tag { TAG, TAG3, TAG5, TAG25, LEMMA, PER_TAG_TOTAL };
  enum features_dynamic { PREVIOUS_VERB_TAG, PREVIOUS_VERB_LEMMA, PREVIOUS_OR_CURRENT_VERB_TAG, PREVIOUS_OR_CURRENT_VERB_LEMMA, DYNAMIC_TOTAL };
  enum features_map { MAP_NONE = -1, MAP_FORM, MAP_LEMMA, MAP_PREFIX1, MAP_PREFIX2, MAP_PREFIX3, MAP_PREFIX4, MAP_SUFFIX1, MAP_SUFFIX2, MAP_SUFFIX3, MAP_SUFFIX4, MAP_TAG, MAP_TAG3, MAP_TAG5, MAP_TAG25, MAP_TOTAL } ;

  struct per_form_features { elementary_feature_value values[PER_FORM_TOTAL]; };
  struct per_tag_features { elementary_feature_value values[PER_TAG_TOTAL]; };
  struct dynamic_features { elementary_feature_value values[DYNAMIC_TOTAL]; };

  static vector<elementary_feature_description> descriptions;

  void compute_features(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const;
  inline void compute_dynamic_features(const tagged_lemma& tag, const per_form_features& per_form, const per_tag_features& per_tag, const dynamic_features* prev_dynamic, dynamic_features& dynamic) const;

  using elementary_features<Map>::maps;
};

typedef czech_elementary_features<persistent_elementary_feature_map> persistent_czech_elementary_features;


// Definitions
template <class Map>
czech_elementary_features<Map>::czech_elementary_features() {
  maps.resize(MAP_TOTAL);
}

template <class Map>
vector<elementary_feature_description> czech_elementary_features<Map>::descriptions = {
  {"Form", PER_FORM, ANY_OFFSET, FORM, MAP_FORM},
  {"FollowingVerbTag", PER_FORM, ANY_OFFSET, FOLLOWING_VERB_TAG, MAP_TAG},
  {"FollowingVerbLemma", PER_FORM, ANY_OFFSET, FOLLOWING_VERB_LEMMA, MAP_LEMMA },
  {"Num", PER_FORM, ONLY_CURRENT, NUM, MAP_NONE},
  {"Cap", PER_FORM, ONLY_CURRENT, CAP, MAP_NONE},
  {"Dash", PER_FORM, ONLY_CURRENT, DASH, MAP_NONE},
  {"Prefix1", PER_FORM, ONLY_CURRENT, PREFIX1, MAP_PREFIX1},
  {"Prefix2", PER_FORM, ONLY_CURRENT, PREFIX2, MAP_PREFIX2},
  {"Prefix3", PER_FORM, ONLY_CURRENT, PREFIX3, MAP_PREFIX3},
  {"Prefix4", PER_FORM, ONLY_CURRENT, PREFIX4, MAP_PREFIX4},
  {"Suffix1", PER_FORM, ONLY_CURRENT, SUFFIX1, MAP_SUFFIX1},
  {"Suffix2", PER_FORM, ONLY_CURRENT, SUFFIX2, MAP_SUFFIX2},
  {"Suffix3", PER_FORM, ONLY_CURRENT, SUFFIX3, MAP_SUFFIX3},
  {"Suffix4", PER_FORM, ONLY_CURRENT, SUFFIX4, MAP_SUFFIX4},

  {"Tag", PER_TAG, ANY_OFFSET, TAG, MAP_TAG},
  {"Tag3", PER_TAG, ANY_OFFSET, TAG3, MAP_TAG3},
  {"Tag5", PER_TAG, ANY_OFFSET, TAG5, MAP_TAG5},
  {"Tag25", PER_TAG, ANY_OFFSET, TAG25, MAP_TAG25},
  {"Lemma", PER_TAG, ANY_OFFSET, LEMMA, MAP_LEMMA},

  {"PreviousVerbTag", DYNAMIC, ANY_OFFSET, PREVIOUS_VERB_TAG, MAP_TAG},
  {"PreviousVerbLemma", DYNAMIC, ANY_OFFSET, PREVIOUS_VERB_LEMMA, MAP_LEMMA}
};

template <class Map>
void czech_elementary_features<Map>::compute_features(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const {
  using namespace unilib;

  // We process the sentence in reverse order, so that we can compute FollowingVerbTag and FollowingVerbLemma directly.
  elementary_feature_value following_verb_tag = elementary_feature_empty, following_verb_lemma = elementary_feature_empty;
  for (unsigned i = forms.size(); i--;) {
    int verb_candidate = -1;

    // Per_tag features and verb_candidate
    for (unsigned j = 0; j < analyses[i].size(); j++) {
      char tag25[2];
      per_tag[i][j].values[TAG] = maps[MAP_TAG].value(analyses[i][j].tag.c_str(), analyses[i][j].tag.size());
      per_tag[i][j].values[TAG3] = analyses[i][j].tag.size() >= 3 ? maps[MAP_TAG3].value(analyses[i][j].tag.c_str() + 2, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG5] = analyses[i][j].tag.size() >= 5 ? maps[MAP_TAG5].value(analyses[i][j].tag.c_str() + 4, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG25] = analyses[i][j].tag.size() >= 5 ? maps[MAP_TAG25].value((tag25[0] = analyses[i][j].tag[1], tag25[1] = analyses[i][j].tag[4], tag25), 2) : elementary_feature_empty;
      per_tag[i][j].values[LEMMA] = j && analyses[i][j-1].lemma == analyses[i][j].lemma ? per_tag[i][j-1].values[LEMMA] :
          maps[MAP_LEMMA].value(analyses[i][j].lemma.c_str(), analyses[i][j].lemma.size());

      if (analyses[i][j].tag[0] == 'V') {
        int tag_compare;
        verb_candidate = verb_candidate < 0 || (tag_compare = analyses[i][j].tag.compare(analyses[i][verb_candidate].tag), tag_compare < 0) || (tag_compare == 0 && analyses[i][j].lemma < analyses[i][verb_candidate].lemma) ? j : verb_candidate;
      }
    }

    // Per_form features
    per_form[i].values[FORM] = maps[MAP_FORM].value(forms[i].str, forms[i].len);
    per_form[i].values[FOLLOWING_VERB_TAG] = following_verb_tag;
    per_form[i].values[FOLLOWING_VERB_LEMMA] = following_verb_lemma;

    // Update following_verb_{tag,lemma} _after_ filling FOLLOWING_VERB_{TAG,LEMMA}.
    if (verb_candidate >= 0) {
      following_verb_tag = per_tag[i][verb_candidate].values[TAG];
      following_verb_lemma = per_tag[i][verb_candidate].values[LEMMA];
    }

    // Ortographic per_form features if needed
    if (analyses[i].size() == 1) {
      per_form[i].values[NUM] = per_form[i].values[CAP] = per_form[i].values[DASH] = elementary_feature_unknown;
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = per_form[i].values[PREFIX4] = elementary_feature_unknown;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = per_form[i].values[SUFFIX4] = elementary_feature_unknown;
    } else if (forms[i].len <= 0) {
      per_form[i].values[NUM] = per_form[i].values[CAP] = per_form[i].values[DASH] = elementary_feature_empty + 1;
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = per_form[i].values[PREFIX4] = elementary_feature_empty;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = per_form[i].values[SUFFIX4] = elementary_feature_empty;
    } else {
      string_piece form = forms[i];
      const char* form_start = form.str;

      bool num = false, cap = false, dash = false;
      size_t indices[8] = {0, form.len, form.len, form.len, form.len, 0, 0, 0}; // careful here regarding forms shorter than 4 characters
      int index = 0;
      while (form.len) {
        indices[(index++)&7] = form.str - form_start;

        unicode::category_t cat = unicode::category(utf8::decode(form.str, form.len));
        num = num || cat & unicode::N;
        cap = cap || cat & unicode::Lut;
        dash = dash || cat & unicode::Pd;

        if (index == 5 || (!form.len && index < 5)) {
          per_form[i].values[PREFIX1] = maps[MAP_PREFIX1].value(form_start, indices[1]);
          per_form[i].values[PREFIX2] = maps[MAP_PREFIX2].value(form_start, indices[2]);
          per_form[i].values[PREFIX3] = maps[MAP_PREFIX3].value(form_start, indices[3]);
          per_form[i].values[PREFIX4] = maps[MAP_PREFIX4].value(form_start, indices[4]);
        }
      }
      per_form[i].values[SUFFIX1] = maps[MAP_SUFFIX1].value(form_start + indices[(index-1)&7], form.str - form_start - indices[(index-1)&7]);
      per_form[i].values[SUFFIX2] = maps[MAP_SUFFIX2].value(form_start + indices[(index-2)&7], form.str - form_start - indices[(index-2)&7]);
      per_form[i].values[SUFFIX3] = maps[MAP_SUFFIX3].value(form_start + indices[(index-3)&7], form.str - form_start - indices[(index-3)&7]);
      per_form[i].values[SUFFIX4] = maps[MAP_SUFFIX4].value(form_start + indices[(index-4)&7], form.str - form_start - indices[(index-4)&7]);
      per_form[i].values[NUM] = elementary_feature_empty + 1 + num;
      per_form[i].values[CAP] = elementary_feature_empty + 1 + cap;
      per_form[i].values[DASH] = elementary_feature_empty + 1 + dash;
    }
  }
}

template <class Map>
void czech_elementary_features<Map>::compute_dynamic_features(const tagged_lemma& tag, const per_form_features& /*per_form*/, const per_tag_features& per_tag, const dynamic_features* prev_dynamic, dynamic_features& dynamic) const {
  if (prev_dynamic) {
    dynamic.values[PREVIOUS_VERB_TAG] = prev_dynamic->values[PREVIOUS_OR_CURRENT_VERB_TAG];
    dynamic.values[PREVIOUS_VERB_LEMMA] = prev_dynamic->values[PREVIOUS_OR_CURRENT_VERB_LEMMA];
  } else {
    dynamic.values[PREVIOUS_VERB_TAG] = elementary_feature_empty;
    dynamic.values[PREVIOUS_VERB_LEMMA] = elementary_feature_empty;
  }

  if (tag.tag[0] == 'V') {
    dynamic.values[PREVIOUS_OR_CURRENT_VERB_TAG] = per_tag.values[TAG];
    dynamic.values[PREVIOUS_OR_CURRENT_VERB_LEMMA] = per_tag.values[LEMMA];
  } else {
    dynamic.values[PREVIOUS_OR_CURRENT_VERB_TAG] = dynamic.values[PREVIOUS_VERB_TAG];
    dynamic.values[PREVIOUS_OR_CURRENT_VERB_LEMMA] = dynamic.values[PREVIOUS_VERB_LEMMA];
  }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
