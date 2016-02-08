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
#include "feature_sequences.h"
#include "morphodita/morpho/persistent_unordered_map_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"
#include "utils/parse_int.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::parse(int window_size, istream& is) {
  unordered_map<string, elementary_feature_description> elementary_map;
  for (auto&& description : ElementaryFeatures::descriptions)
    if (!elementary_map.emplace(description.name, description).second)
      runtime_failure("Repeated elementary feature with name " << description.name << '!');

  string line;
  vector<string> tokens;
  while (getline(is, line)) {
    split(line, ',', tokens);
    if (tokens.empty()) runtime_failure("Feature sequence cannot be empty!");

    bool contains_only_current = false;
    sequences.emplace_back();
    for (auto&& token : tokens) {
      vector<string> parts;
      split(token, ' ', parts);
      if (parts.size() != 2) runtime_failure("Cannot parse feature sequence element '" << token << "'!");
      auto it = elementary_map.find(parts[0]);
      if (it == elementary_map.end()) runtime_failure("Unknown elementary feature '" << parts[0] << "' used in feature sequence '" << token << "'!");

      auto& desc = it->second;
      int sequence_index = parse_int(parts[1].c_str(), "sequence_index");
      if (desc.type == DYNAMIC && sequence_index != 0) runtime_failure("Nonzero sequence index " << sequence_index << " of dynamic elementary feature '" << desc.name << "'!");
      if (desc.type == PER_TAG && (sequence_index > 0 || sequence_index <= -window_size)) runtime_failure("Wrong sequence index " << sequence_index << " of per-tag elementary feature '" << desc.name << "'!");
      if (desc.range == ONLY_CURRENT && sequence_index != 0) runtime_failure("Nonzero sequence index " << sequence_index << " of elementary feature '" << desc.name << "' requiring zero offset!");

      sequences.back().elements.emplace_back(it->second.type, it->second.index, sequence_index);
      if (desc.type == DYNAMIC) sequences.back().dependant_range = max(sequences.back().dependant_range, window_size + 1);
      if (desc.type == PER_TAG) sequences.back().dependant_range = max(sequences.back().dependant_range, 1 - sequence_index);
      contains_only_current |= desc.range == ONLY_CURRENT;
    }
    if (contains_only_current && sequences.back().dependant_range > 1) runtime_failure("Feature sequence '" << line << "' contains both a non-local elementary feature and exclusively-local elementary feature!");
  }

  stable_sort(sequences.begin(), sequences.end(), [](const feature_sequence& a, const feature_sequence& b) { return a.dependant_range > b.dependant_range; });
  scores.resize(sequences.size());
}

template <class ElementaryFeatures, class Map>
inline bool feature_sequences<ElementaryFeatures, Map>::save(ostream& os) {
  if (!elementary.save(os)) return false;

  binary_encoder enc;
  enc.add_1B(sequences.size());
  for (auto&& sequence : sequences) {
    enc.add_4B(sequence.dependant_range);
    enc.add_1B(sequence.elements.size());
    for (auto&& element : sequence.elements) {
      enc.add_4B(element.type);
      enc.add_4B(element.elementary_index);
      enc.add_4B(element.sequence_index);
    }
  }

  enc.add_1B(scores.size());
  for (auto&& score : scores)
    score.save(enc);

  return compressor::save(os, enc);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
