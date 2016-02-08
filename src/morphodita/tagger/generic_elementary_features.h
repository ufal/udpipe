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
class generic_elementary_features : public elementary_features<Map> {
 public:
  generic_elementary_features();

  enum features_per_form { FORM, FOLLOWING_VERB_TAG, FOLLOWING_VERB_LEMMA, NUM, CAP, DASH, PREFIX1, PREFIX2, PREFIX3, PREFIX4, PREFIX5, PREFIX6, PREFIX7, PREFIX8, PREFIX9, SUFFIX1, SUFFIX2, SUFFIX3, SUFFIX4, SUFFIX5, SUFFIX6, SUFFIX7, SUFFIX8, SUFFIX9, PER_FORM_TOTAL };
  enum features_per_tag { TAG, TAG1, TAG2, TAG3, TAG4, TAG5, LEMMA, PER_TAG_TOTAL };
  enum features_dynamic { PREVIOUS_VERB_TAG, PREVIOUS_VERB_LEMMA, PREVIOUS_OR_CURRENT_VERB_TAG, PREVIOUS_OR_CURRENT_VERB_LEMMA, DYNAMIC_TOTAL };
  enum features_map { MAP_NONE = -1, MAP_FORM, MAP_PREFIX1, MAP_PREFIX2, MAP_PREFIX3, MAP_PREFIX4, MAP_PREFIX5, MAP_PREFIX6, MAP_PREFIX7, MAP_PREFIX8, MAP_PREFIX9, MAP_SUFFIX1, MAP_SUFFIX2, MAP_SUFFIX3, MAP_SUFFIX4, MAP_SUFFIX5, MAP_SUFFIX6, MAP_SUFFIX7, MAP_SUFFIX8, MAP_SUFFIX9, MAP_TAG, MAP_TAG1, MAP_TAG2, MAP_TAG3, MAP_TAG4, MAP_TAG5, MAP_LEMMA, MAP_TOTAL } ;

  struct per_form_features { elementary_feature_value values[PER_FORM_TOTAL]; };
  struct per_tag_features { elementary_feature_value values[PER_TAG_TOTAL]; };
  struct dynamic_features { elementary_feature_value values[DYNAMIC_TOTAL]; };

  static vector<elementary_feature_description> descriptions;

  void compute_features(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const;
  inline void compute_dynamic_features(const tagged_lemma& tag, const per_form_features& per_form, const per_tag_features& per_tag, const dynamic_features* prev_dynamic, dynamic_features& dynamic) const;

  using elementary_features<Map>::maps;
};

typedef generic_elementary_features<persistent_elementary_feature_map> persistent_generic_elementary_features;


// Definitions
template <class Map>
generic_elementary_features<Map>::generic_elementary_features() {
  maps.resize(MAP_TOTAL);
}

template <class Map>
vector<elementary_feature_description> generic_elementary_features<Map>::descriptions = {
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
  {"Prefix5", PER_FORM, ONLY_CURRENT, PREFIX5, MAP_PREFIX5},
  {"Prefix6", PER_FORM, ONLY_CURRENT, PREFIX6, MAP_PREFIX6},
  {"Prefix7", PER_FORM, ONLY_CURRENT, PREFIX7, MAP_PREFIX7},
  {"Prefix8", PER_FORM, ONLY_CURRENT, PREFIX8, MAP_PREFIX8},
  {"Prefix9", PER_FORM, ONLY_CURRENT, PREFIX9, MAP_PREFIX9},
  {"Suffix1", PER_FORM, ONLY_CURRENT, SUFFIX1, MAP_SUFFIX1},
  {"Suffix2", PER_FORM, ONLY_CURRENT, SUFFIX2, MAP_SUFFIX2},
  {"Suffix3", PER_FORM, ONLY_CURRENT, SUFFIX3, MAP_SUFFIX3},
  {"Suffix4", PER_FORM, ONLY_CURRENT, SUFFIX4, MAP_SUFFIX4},
  {"Suffix5", PER_FORM, ONLY_CURRENT, SUFFIX5, MAP_SUFFIX5},
  {"Suffix6", PER_FORM, ONLY_CURRENT, SUFFIX6, MAP_SUFFIX6},
  {"Suffix7", PER_FORM, ONLY_CURRENT, SUFFIX7, MAP_SUFFIX7},
  {"Suffix8", PER_FORM, ONLY_CURRENT, SUFFIX8, MAP_SUFFIX8},
  {"Suffix9", PER_FORM, ONLY_CURRENT, SUFFIX9, MAP_SUFFIX9},

  {"Tag", PER_TAG, ANY_OFFSET, TAG, MAP_TAG},
  {"Tag1", PER_TAG, ANY_OFFSET, TAG1, MAP_TAG1},
  {"Tag2", PER_TAG, ANY_OFFSET, TAG2, MAP_TAG2},
  {"Tag3", PER_TAG, ANY_OFFSET, TAG3, MAP_TAG3},
  {"Tag4", PER_TAG, ANY_OFFSET, TAG4, MAP_TAG4},
  {"Tag5", PER_TAG, ANY_OFFSET, TAG5, MAP_TAG5},
  {"Lemma", PER_TAG, ANY_OFFSET, LEMMA, MAP_LEMMA},

  {"PreviousVerbTag", DYNAMIC, ANY_OFFSET, PREVIOUS_VERB_TAG, MAP_TAG},
  {"PreviousVerbLemma", DYNAMIC, ANY_OFFSET, PREVIOUS_VERB_LEMMA, MAP_LEMMA}
};

template <class Map>
void generic_elementary_features<Map>::compute_features(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const {
  using namespace unilib;

  // We process the sentence in reverse order, so that we can compute FollowingVerbTag and FollowingVerbLemma directly.
  elementary_feature_value following_verb_tag = elementary_feature_empty, following_verb_lemma = elementary_feature_empty;
  for (unsigned i = forms.size(); i--;) {
    int verb_candidate = -1;

    // Per_tag features and verb_candidate
    for (unsigned j = 0; j < analyses[i].size(); j++) {
      per_tag[i][j].values[TAG] = maps[MAP_TAG].value(analyses[i][j].tag.c_str(), analyses[i][j].tag.size());
      per_tag[i][j].values[TAG1] = analyses[i][j].tag.size() >= 1 ? maps[MAP_TAG1].value(analyses[i][j].tag.c_str() + 0, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG2] = analyses[i][j].tag.size() >= 2 ? maps[MAP_TAG2].value(analyses[i][j].tag.c_str() + 1, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG3] = analyses[i][j].tag.size() >= 3 ? maps[MAP_TAG3].value(analyses[i][j].tag.c_str() + 2, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG4] = analyses[i][j].tag.size() >= 4 ? maps[MAP_TAG4].value(analyses[i][j].tag.c_str() + 3, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG5] = analyses[i][j].tag.size() >= 5 ? maps[MAP_TAG5].value(analyses[i][j].tag.c_str() + 4, 1) : elementary_feature_empty;
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
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = elementary_feature_unknown;
      per_form[i].values[PREFIX4] = per_form[i].values[PREFIX5] = per_form[i].values[PREFIX6] = elementary_feature_unknown;
      per_form[i].values[PREFIX7] = per_form[i].values[PREFIX8] = per_form[i].values[PREFIX9] = elementary_feature_unknown;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = elementary_feature_unknown;
      per_form[i].values[SUFFIX4] = per_form[i].values[SUFFIX5] = per_form[i].values[SUFFIX6] = elementary_feature_unknown;
      per_form[i].values[SUFFIX7] = per_form[i].values[SUFFIX8] = per_form[i].values[SUFFIX9] = elementary_feature_unknown;
    } else if (forms[i].len <= 0) {
      per_form[i].values[NUM] = per_form[i].values[CAP] = per_form[i].values[DASH] = elementary_feature_empty + 1;
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = elementary_feature_empty;
      per_form[i].values[PREFIX4] = per_form[i].values[PREFIX5] = per_form[i].values[PREFIX6] = elementary_feature_empty;
      per_form[i].values[PREFIX7] = per_form[i].values[PREFIX8] = per_form[i].values[PREFIX9] = elementary_feature_empty;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = elementary_feature_empty;
      per_form[i].values[SUFFIX4] = per_form[i].values[SUFFIX5] = per_form[i].values[SUFFIX6] = elementary_feature_empty;
      per_form[i].values[SUFFIX7] = per_form[i].values[SUFFIX8] = per_form[i].values[SUFFIX9] = elementary_feature_empty;
    } else {
      string_piece form = forms[i];
      const char* form_start = form.str;

      bool num = false, cap = false, dash = false;
      size_t indices[18] = {0, form.len, form.len, form.len, form.len, form.len, form.len, form.len, form.len, form.len, 0, 0, 0, 0, 0, 0, 0, 0}; // careful here regarding forms shorter than 9 characters
      int index = 0;
      while (form.len) {
        indices[(index++) % 18] = form.str - form_start;

        unicode::category_t cat = unicode::category(utf8::decode(form.str, form.len));
        num = num || cat & unicode::N;
        cap = cap || cat & unicode::Lut;
        dash = dash || cat & unicode::Pd;

        if (index == 10 || (!form.len && index < 10)) {
          per_form[i].values[PREFIX1] = maps[MAP_PREFIX1].value(form_start, indices[1]);
          per_form[i].values[PREFIX2] = maps[MAP_PREFIX2].value(form_start, indices[2]);
          per_form[i].values[PREFIX3] = maps[MAP_PREFIX3].value(form_start, indices[3]);
          per_form[i].values[PREFIX4] = maps[MAP_PREFIX4].value(form_start, indices[4]);
          per_form[i].values[PREFIX5] = maps[MAP_PREFIX5].value(form_start, indices[5]);
          per_form[i].values[PREFIX6] = maps[MAP_PREFIX6].value(form_start, indices[6]);
          per_form[i].values[PREFIX7] = maps[MAP_PREFIX7].value(form_start, indices[7]);
          per_form[i].values[PREFIX8] = maps[MAP_PREFIX8].value(form_start, indices[8]);
          per_form[i].values[PREFIX9] = maps[MAP_PREFIX9].value(form_start, indices[9]);
        }
      }
      per_form[i].values[SUFFIX1] = maps[MAP_SUFFIX1].value(form_start + indices[(index+18-1) % 18], form.str - form_start - indices[(index+18-1) % 18]);
      per_form[i].values[SUFFIX2] = maps[MAP_SUFFIX2].value(form_start + indices[(index+18-2) % 18], form.str - form_start - indices[(index+18-2) % 18]);
      per_form[i].values[SUFFIX3] = maps[MAP_SUFFIX3].value(form_start + indices[(index+18-3) % 18], form.str - form_start - indices[(index+18-3) % 18]);
      per_form[i].values[SUFFIX4] = maps[MAP_SUFFIX4].value(form_start + indices[(index+18-4) % 18], form.str - form_start - indices[(index+18-4) % 18]);
      per_form[i].values[SUFFIX5] = maps[MAP_SUFFIX5].value(form_start + indices[(index+18-5) % 18], form.str - form_start - indices[(index+18-5) % 18]);
      per_form[i].values[SUFFIX6] = maps[MAP_SUFFIX6].value(form_start + indices[(index+18-6) % 18], form.str - form_start - indices[(index+18-6) % 18]);
      per_form[i].values[SUFFIX7] = maps[MAP_SUFFIX7].value(form_start + indices[(index+18-7) % 18], form.str - form_start - indices[(index+18-7) % 18]);
      per_form[i].values[SUFFIX8] = maps[MAP_SUFFIX8].value(form_start + indices[(index+18-8) % 18], form.str - form_start - indices[(index+18-8) % 18]);
      per_form[i].values[SUFFIX9] = maps[MAP_SUFFIX9].value(form_start + indices[(index+18-9) % 18], form.str - form_start - indices[(index+18-9) % 18]);
      per_form[i].values[NUM] = elementary_feature_empty + 1 + num;
      per_form[i].values[CAP] = elementary_feature_empty + 1 + cap;
      per_form[i].values[DASH] = elementary_feature_empty + 1 + dash;
    }
  }
}

template <class Map>
void generic_elementary_features<Map>::compute_dynamic_features(const tagged_lemma& tag, const per_form_features& /*per_form*/, const per_tag_features& per_tag, const dynamic_features* prev_dynamic, dynamic_features& dynamic) const {
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
