// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "gru_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool gru_tokenizer::is_space() {
  return is_space(current);
}

bool gru_tokenizer::is_space(size_t index) {
  return (chars[index].cat & unilib::unicode::Zs) || chars[index].chr == '\r' || chars[index].chr == '\n' || chars[index].chr == '\t';
}

bool gru_tokenizer::next_sentence(vector<token_range>& tokens) {
  tokens.clear();

  // Reset tokenizer on new text
  if (current == 0) network_start = network_length = 0;

  // Tokenize until EOS
  for (bool eos = false; !eos || !emergency_sentence_split(tokens); ) {
    while (current < chars.size() - 1 && is_space())
      if (next_outcome() == gru_tokenizer_network::END_OF_SENTENCE && !tokens.empty()) {
        eos = true;
        break;
      }
    if (current >= chars.size() - 1 || eos) break;

    // We have a beginning of a token. Try if it is an URL.
    if (tokenize_url_email(tokens))
      continue;

    // Slurp current token
    size_t token_start = current;
    do {
      int outcome = next_outcome();
      eos = outcome == gru_tokenizer_network::END_OF_SENTENCE;
      if (outcome != gru_tokenizer_network::NO_SPLIT) break;
    } while (current < chars.size() - 1);
    tokens.emplace_back(token_start, current - token_start);
  }

  return !tokens.empty();
}

int gru_tokenizer::next_outcome() {
  if (current >= network_start + network_length) {
    // Compute required window
    network_start = current;
    network_length = segment < chars.size() - 1 - current ? segment : chars.size() - 1 - current;
    network_chars.resize(network_length);
    network_outcomes.resize(network_length);

    // Perform the classification
    for (size_t i = 0; i < network_length; i++)
      if (is_space(i + network_start))
        network_chars[i].chr = ' ', network_chars[i].cat = unilib::unicode::Zs;
      else
        network_chars[i].chr = chars[i + network_start].chr, network_chars[i].cat = chars[i + network_start].cat;

    // Add a space to the end on the EOD
    if (network_length < segment) {
      network_chars.emplace_back();
      network_outcomes.emplace_back();
      network_chars.back().chr = ' ';
      network_chars.back().cat = unilib::unicode::Zs;
    }
    network.classify(network_chars, network_outcomes);

    // Add spacing token/sentence breaks
    for (size_t i = 0; i < network_length - 1; i++)
      if ((i + 2 < network_length && chars[network_start+i+1].chr == '\n' && chars[network_start+i+2].chr == '\n') ||
          (i + 4 < network_length && chars[network_start+i+1].chr == '\r' && chars[network_start+i+2].chr == '\n' &&
           chars[network_start+i+3].chr == '\r' && chars[network_start+i+4].chr == '\n'))
        network_outcomes[i].outcome = gru_tokenizer_network::END_OF_SENTENCE;
      else if (network_outcomes[i].outcome == gru_tokenizer_network::NO_SPLIT && is_space(network_start + i + 1) && !is_space(network_start + i))
        network_outcomes[i].outcome = gru_tokenizer_network::END_OF_TOKEN;

    // Adjust network_length to suitable break
    if (network_length == segment && network_length >= 10) {
      network_length -= 5;
      while (network_length > segment / 2)
        if (network_outcomes[--network_length].outcome != gru_tokenizer_network::NO_SPLIT)
          break;
    }
  }
  return network_outcomes[current++ - network_start].outcome;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
