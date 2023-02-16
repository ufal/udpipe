// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
class training_elementary_feature_map {
 public:
  inline elementary_feature_value value(const char* feature, int len) const;
  mutable unordered_map<string, elementary_feature_value> map = {{"", elementary_feature_empty}};
 private:
  mutable string key;
};

class training_feature_sequence_map {
 public:
  struct info {
    // We deliberately use feature_sequence*s*_score to check for overflow
    feature_sequences_score alpha = 0;
    feature_sequences_score gamma = 0;
    int last_gamma_update = 0;
  };

  inline feature_sequence_score score(const char* feature, int len) const;
  mutable unordered_map<string, info> map;
 private:
  mutable string key;
};

template <template <class> class ElementaryFeatures> using train_feature_sequences = feature_sequences<ElementaryFeatures<training_elementary_feature_map>, training_feature_sequence_map>;

// Definitions
elementary_feature_value training_elementary_feature_map::value(const char* feature, int len) const {
  key.assign(feature, len);
  return map.emplace(key, elementary_feature_empty + elementary_feature_value(map.size())).first->second;
}

feature_sequence_score training_feature_sequence_map::score(const char* feature, int len) const {
  key.assign(feature, len);
  auto it = map.find(key);
  return it != map.end() ? it->second.alpha : 0;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
