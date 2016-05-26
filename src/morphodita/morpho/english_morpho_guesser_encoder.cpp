// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <map>
#include <set>
#include <unordered_map>

#include "english_morpho_guesser_encoder.h"
#include "persistent_unordered_map_encoder.h"
#include "utils/parse_int.h"
#include "utils/split.h"
#include "trainer/training_failure.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void english_morpho_guesser_encoder::encode(istream& guesser_file, istream& negations_file, binary_encoder& enc) {
  // Guesser exceptions
  typedef map<string, set<string>> lemma_info;
  unordered_map<string, lemma_info> guesser;
  unordered_map<string, unsigned> tags_map;
  vector<string> tags;

  string line;
  vector<string> tokens;

  // Load guesser exceptions
  while (getline(guesser_file, line)) {
    split(line, '\t', tokens);
    if (tokens.size() != 3) training_failure("The line '" << line << "' in english guesser file does not have three columns!");
    guesser[tokens[2]][tokens[0]].insert(tokens[1]);
    if (tags_map.emplace(tokens[1], tags_map.size()).second) tags.emplace_back(tokens[1]);
  }

  // Negations
  struct negation_info {
    unsigned negation_len;
    unsigned to_follow;

    negation_info() : negation_len(0), to_follow(0) {}
    negation_info(unsigned negation_len, unsigned to_follow) : negation_len(negation_len), to_follow(to_follow) {}
  };
  unordered_map<string, negation_info> negations;

  // Load negations
  while (negations_file && getline(negations_file, line)) {
    split(line, '\t', tokens);
    if (tokens.size() != 3) training_failure("The line '" << line << "' in english negation file does not have three columns!");

    unsigned negation_len = parse_int(tokens[2].c_str(), "negation_len in english negation file");
    if (!negation_len) training_failure("Negation len in line '" << line << "' in english negation file is zero!");

    unsigned to_follow = parse_int(tokens[1].c_str(), "to_follow in english negation file");

    if (negations.count(tokens[0])) training_failure("The negation '" << tokens[0] << "' in english negation file is repeated!");
    negations.emplace(tokens[0], negation_info(negation_len, to_follow));
  }

  // Save tags
  enc.add_2B(tags.size());
  for (auto&& tag : tags) {
    enc.add_1B(tag.size());
    enc.add_data(tag);
  }

  // Save guesser exceptions
  persistent_unordered_map(guesser, 5, [&tags_map](binary_encoder& enc, const lemma_info& lemmas) {
    enc.add_1B(lemmas.size());
    for (auto&& lemma : lemmas) {
      enc.add_1B(lemma.first.size());
      enc.add_data(lemma.first);
      enc.add_1B(lemma.second.size());
      for (auto&& tag : lemma.second)
        enc.add_2B(tags_map.at(tag));
    }
  }).save(enc);

  // Save negations
  persistent_unordered_map(negations, 5, true, false, [](binary_encoder& enc, const negation_info& negation) {
    enc.add_1B(negation.negation_len);
    enc.add_1B(negation.to_follow);
  }).save(enc);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
