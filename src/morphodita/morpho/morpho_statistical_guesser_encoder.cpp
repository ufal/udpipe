// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <unordered_map>

#include "morpho_statistical_guesser_encoder.h"
#include "persistent_unordered_map_encoder.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void morpho_statistical_guesser_encoder::encode(istream& is, binary_encoder& enc) {
  unordered_map<string, vector<pair<vector<string>, vector<int>>>> statistical_guesser;
  vector<string> tags;
  unordered_map<string, int> tags_map;

  // Load statistical guesser
  string line;
  vector<string> tokens;
  if (!getline(is, line)) runtime_failure("Missing first line with default tag in statistical guesser file");
  int statistical_guesser_default = tags_map.emplace(line.data(), tags.size()).first->second;
  if (unsigned(statistical_guesser_default) >= tags.size()) tags.emplace_back(line.data());

  while (getline(is, line)) {
    split(line, '\t', tokens);
    if (tokens.size() < 3 || (tokens.size() % 2) != 1) runtime_failure("Cannot parse line " << line << " in statistical guesser file!");

    vector<string> affixes;
    split(tokens[0], ' ', affixes);
    if (affixes.size() != 2) runtime_failure("Cannot parse prefix_suffix '" << tokens[0] << "' in statistical guesser file!");
    reverse(affixes[1].begin(), affixes[1].end());

    auto& rules = statistical_guesser[affixes[1] + ' ' + affixes[0]];
    for (unsigned i = 1; i < tokens.size(); i+= 2) {
      vector<string> replacements;
      split(tokens[i], ' ', replacements);
      if (replacements.size() != 4) runtime_failure("Cannot parse replacement rule '" << tokens[i] << "' in statistical guesser file!");

      vector<string> rule_tags;
      split(tokens[i+1], ' ', rule_tags);
      vector<int> decoded_tags;
      for (auto&& rule_tag : rule_tags) {
        int tag = tags_map.emplace(rule_tag, tags.size()).first->second;
        if (unsigned(tag) >= tags.size()) tags.emplace_back(rule_tag);
        decoded_tags.emplace_back(tag);
      }

      rules.emplace_back(replacements, decoded_tags);
    }
  }

  // Encode statistical guesser
  enc.add_2B(tags.size());
  for (auto&& tag : tags) {
    enc.add_1B(tag.size());
    enc.add_data(tag);
  }
  enc.add_2B(statistical_guesser_default);

  persistent_unordered_map(statistical_guesser, 5, true, false, [](binary_encoder& enc, vector<pair<vector<string>, vector<int>>> rules) {
    binary_encoder e;
    e.add_1B(rules.size());
    for (auto&& rule : rules) {
      if (rule.first.size() != 4) runtime_failure("Replacement rule not of size 4 in statistical guesser!");
      for (auto&& affix : rule.first) {
        e.add_1B(affix.size());
        e.add_data(affix);
      }
      e.add_1B(rule.second.size());
      for (auto&& tag : rule.second)
        e.add_2B(tag);
    }
    enc.add_2B(e.data.size());
    enc.add_data(e.data);
  }).save(enc);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
