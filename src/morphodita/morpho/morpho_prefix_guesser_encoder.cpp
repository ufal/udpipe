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

#include "morpho_prefix_guesser_encoder.h"
#include "persistent_unordered_map_encoder.h"
#include "utils/split.h"
#include "trainer/training_failure.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void morpho_prefix_guesser_encoder::encode(istream& is, binary_encoder& enc) {
  vector<string> filters;
  unordered_map<string, uint32_t> filters_map;
  unordered_map<string, uint32_t> prefixes_initial;
  unordered_map<string, uint32_t> prefixes_middle;
  auto* prefixes_current = &prefixes_initial;

  // Load prefix guesser
  string line;
  vector<string> tokens;
  while (getline(is, line)) {
    if (line.empty() && prefixes_current == &prefixes_initial) {
      prefixes_current = &prefixes_middle;
      continue;
    }
    split(line, '\t', tokens);
    if (tokens.size() != 2) training_failure("Line " << line << " in prefix guesser prefixes file does not contain two columns!");

    auto it = filters_map.emplace(tokens[1], 1<<filters.size());
    if (it.second)
      filters.emplace_back(tokens[1]);
    auto filter = it.first->second;
    if (!filter) training_failure("Too much different tag filters in the prefix guesser when adding tag filter '" << tokens[1] << "'!");

    (*prefixes_current)[tokens[0]] |= filter;
  }

  // Encode prefix guesser
  enc.add_1B(filters.size());
  for (auto&& filter : filters) {
    enc.add_1B(filter.size());
    enc.add_data(filter);
  }

  persistent_unordered_map(prefixes_initial, 5, true, false, [](binary_encoder& enc, uint32_t mask) { enc.add_4B(mask); }).save(enc);
  persistent_unordered_map(prefixes_middle, 5, true, false, [](binary_encoder& enc, uint32_t mask) { enc.add_4B(mask); }).save(enc);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
