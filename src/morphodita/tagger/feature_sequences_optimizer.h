// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>

#include "common.h"
#include "elementary_features.h"
#include "feature_sequences.h"
#include "training_maps.h"
#include "vli.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class T>
class feature_sequences_optimizer;

template <template <class, class> class FeatureSequences, template <class> class ElementaryFeatures>
class feature_sequences_optimizer<FeatureSequences<ElementaryFeatures<training_elementary_feature_map>, training_feature_sequence_map>> {
 public:
  typedef FeatureSequences<ElementaryFeatures<training_elementary_feature_map>, training_feature_sequence_map> original_feature_sequences;
  typedef FeatureSequences<ElementaryFeatures<persistent_elementary_feature_map>, persistent_feature_sequence_map> optimized_feature_sequences;

  static void optimize(const original_feature_sequences& features, optimized_feature_sequences& optimized_features);
};

// Definitions
template <template <class, class> class FeatureSequences, template <class> class ElementaryFeatures>
void feature_sequences_optimizer<FeatureSequences<ElementaryFeatures<training_elementary_feature_map>, training_feature_sequence_map>>::optimize(const original_feature_sequences& features, optimized_feature_sequences& optimized_features) {
  const ElementaryFeatures<training_elementary_feature_map>& elementary = features.elementary;
  ElementaryFeatures<persistent_elementary_feature_map>& optimized_elementary = optimized_features.elementary;

  // Iterate over feature sequences of non-zero weight and count number of
  // occurences in corresponding elementary feature maps.
  // In order to be able to do so, precompute map_index for elements of features.sequences.
  vector<vector<int>> map_indices(features.sequences.size());
  for (unsigned i = 0; i < map_indices.size(); i++) {
    for (auto&& element : features.sequences[i].elements)
      for (auto&& description : decltype(features.elementary)::descriptions)
        if (element.type == description.type && element.elementary_index == description.index)
          map_indices[i].emplace_back(description.map_index);

    assert(map_indices[i].size() == features.sequences[i].elements.size());
  }

  struct count_info { elementary_feature_value ori = 0; int count = 0; };
  vector<vector<count_info>> counts(elementary.maps.size());
  vector<elementary_feature_value> elementary_ids;
  for (unsigned i = 0; i < features.sequences.size(); i++)
    for (auto&& element : features.scores[i].map)
      if (element.second.gamma) {
        elementary_ids.clear();
        for (const char* key = element.first.c_str(); key != element.first.c_str() + element.first.size(); assert(key <= element.first.c_str() + element.first.size()))
          elementary_ids.emplace_back(vli<elementary_feature_value>::decode(key));

        assert(elementary_ids.size() == features.sequences[i].elements.size());
        for (unsigned j = 0; j < elementary_ids.size(); j++) {
          if (map_indices[i][j] < 0) continue;
          if (elementary_ids[j] >= counts[map_indices[i][j]].size()) counts[map_indices[i][j]].resize(elementary_ids[j] + 1);
          counts[map_indices[i][j]][elementary_ids[j]].count++;
        }
      }

  // Sort counts by sizes decreasing
  for (auto&& count : counts) {
    if (elementary_feature_empty >= count.size()) count.resize(elementary_feature_empty + 1);
    count[elementary_feature_unknown].count = 0;
    count[elementary_feature_empty].count = 1;
    for (elementary_feature_value i = 0; i < count.size(); i++) count[i].ori = i;
    sort(count.begin() + elementary_feature_empty + 1, count.end(), [](const count_info& a, const count_info& b){ return a.count > b.count; });
  }

  // Create an elementary ids map
  vector<vector<elementary_feature_value>> elementary_ids_map(counts.size());
  for (unsigned i = 0; i < counts.size(); i++) {
    elementary_ids_map[i].resize(counts[i].size());
    for (elementary_feature_value j = 0; j < counts[i].size(); j++)
      elementary_ids_map[i][counts[i][j].ori] = counts[i][j].count ? j : elementary_feature_unknown;
  }

  // Make optimized elementary maps by applying elementary ids map
  optimized_elementary.maps.clear();
  for (unsigned i = 0; i < elementary.maps.size(); i++) {
    unordered_map<string, elementary_feature_value> mapped_ids;
    for (auto&& element : elementary.maps[i].map)
      if (element.second < elementary_ids_map[i].size() && elementary_ids_map[i][element.second] != elementary_feature_unknown)
        mapped_ids.emplace(element.first, elementary_ids_map[i][element.second]);

    optimized_elementary.maps.emplace_back(persistent_unordered_map(mapped_ids, 1, [](binary_encoder& enc, int id) {
      enc.add_4B(id);
    }));
  }

  // Remap keys in feature sequences by applying elementary_ids_map to appropriate subkeys
  optimized_features.sequences = features.sequences;
  optimized_features.scores.clear();
  vector<char> key_buffer;
  for (unsigned i = 0; i < features.sequences.size(); i++) {
    decltype(features.scores[i].map) updated_map;
    for (auto&& element : features.scores[i].map)
      if (element.second.gamma) {
        elementary_ids.clear();
        for (const char* key = element.first.c_str(); key < element.first.c_str() + element.first.size(); )
          elementary_ids.emplace_back(vli<elementary_feature_value>::decode(key));

        assert(elementary_ids.size() == features.sequences[i].elements.size());
        for (unsigned j = 0; j < elementary_ids.size(); j++) {
          if (map_indices[i][j] < 0) continue;
          assert(elementary_ids[j] < elementary_ids_map[map_indices[i][j]].size() && elementary_ids_map[map_indices[i][j]][elementary_ids[j]] != elementary_feature_unknown);
          elementary_ids[j] = elementary_ids_map[map_indices[i][j]][elementary_ids[j]];
        }

        key_buffer.resize(elementary_ids.size() * vli<elementary_feature_value>::max_length());
        char* key = key_buffer.data();
        for (unsigned j = 0; j < elementary_ids.size(); j++)
          vli<elementary_feature_value>::encode(elementary_ids[j], key);

        updated_map.emplace(string(key_buffer.data(), key - key_buffer.data()), element.second);
      }

    optimized_features.scores.emplace_back(persistent_unordered_map(updated_map, 1, [](binary_encoder& enc, const training_feature_sequence_map::info& info) {
      assert(feature_sequence_score(info.gamma) == info.gamma);
      enc.add_4B(info.gamma);
    }));
  }

  // Original code which only dropped feature sequences with gamma == 0
  // optimized_elementary.maps.clear();
  // for (auto&& map : elementary.maps)
  //   optimized_elementary.maps.emplace_back(persistent_unordered_map(map.map, 1, [](binary_encoder& enc, elementary_feature_value value) {
  //     enc.add_4B(value);
  //   }));
  //
  // optimized_features.sequences = features.sequences;
  // optimized_features.scores.clear();
  // for (auto&& score : features.scores) {
  //   decltype(score.map) pruned_map;
  //   for (auto&& element : score.map)
  //     if (element.second.gamma)
  //       pruned_map.insert(element);
  //
  //   optimized_features.scores.emplace_back(persistent_unordered_map(pruned_map, 1, [](binary_encoder& enc, const training_feature_sequence_map::info& info) {
  //     enc.add_4B(info.gamma);
  //   }));
  // }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
