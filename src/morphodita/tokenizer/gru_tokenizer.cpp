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

bool gru_tokenizer::is_space(size_t index) {
  return (chars[index].cat & unilib::unicode::Zs) || chars[index].chr == '\r' || chars[index].chr == '\n' || chars[index].chr == '\t';
}

bool gru_tokenizer::next_sentence(vector<token_range>& tokens) {
  tokens.clear();

  // Reset tokenizer on new text
  if (current == 0) network_index = network_length = 0;

  // Tokenize until EOS
  for (bool eos = false; !eos && !emergency_sentence_split(tokens); ) {
    while (current < chars.size() - 1 && is_space(current))
      if (next_outcome() == gru_tokenizer_network::END_OF_SENTENCE && !tokens.empty())
        break;

    if (current >= chars.size() - 1) break;

    // We have a beginning of a token. Try if it is an URL.
    if (tokenize_url_email(tokens)) {
      while (network_index < network_length && network_offsets[network_index] < current)
        if (network_outcomes[network_index++].outcome == gru_tokenizer_network::END_OF_SENTENCE && !tokens.empty())
          eos = true;
      continue;
    }

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
  if (network_index >= network_length) {
    // Compute required window
    network_index = 0;
    network_length = 0;
    network_chars.clear();
    network_outcomes.clear();
    network_offsets.clear();

    // Prepare data for the classification
    for (size_t offset = current;
         network_offsets.push_back(offset), offset < chars.size() - 1 && network_length < segment;
         network_length++, offset++) {
      if (is_space(offset)) {
        network_chars.emplace_back(' ', unilib::unicode::Zs);
        while (offset + 1 < chars.size() - 1 && is_space(offset + 1)) offset++;
      } else {
        network_chars.emplace_back(chars[offset].chr, chars[offset].cat);
      }
    }
    // Add a space to the end on the EOD
    if (network_length < segment && network_chars.back().chr != ' ')
      network_chars.emplace_back(' ', unilib::unicode::Zs);
    network_outcomes.resize(network_chars.size());

    // Perform the classification
    network.classify(network_chars, network_outcomes);

    // Add spacing token/sentence breaks
    for (size_t i = 0; i < network_length - 1; i++)
      if (is_space(network_offsets[i+1])) {
        // Detect EOS on the following space or \n\n or \r\n\r\n, or if there is end of text
        bool eos = network_outcomes[i+1].outcome == gru_tokenizer_network::END_OF_SENTENCE;
        if (i + 2 == network_length) eos = true;
        for (size_t j = network_offsets[i+1]; j + 1 < network_offsets[i+2] && !eos; j++)
          eos = (chars[j].chr == '\n' && chars[j+1].chr == '\n') ||
                (j + 3 < network_offsets[i+2] && chars[j].chr == '\r' && chars[j+1].chr == '\n' && chars[j+2].chr == '\r' && chars[j+3].chr == '\n');
        if (eos) network_outcomes[i].outcome = gru_tokenizer_network::END_OF_SENTENCE;

        if (network_outcomes[i].outcome == gru_tokenizer_network::NO_SPLIT)
          // Force EOT if not allowing spaces, and also detect EOT on the following space
          if (!allow_spaces || network_outcomes[i+1].outcome == gru_tokenizer_network::END_OF_TOKEN)
            network_outcomes[i].outcome = gru_tokenizer_network::END_OF_TOKEN;
      }

    // Adjust network_length to suitable break
    if (network_length == segment && network_length >= 10) {
      network_length -= 5;
      while (network_length > segment / 2)
        if (network_outcomes[--network_length].outcome != gru_tokenizer_network::NO_SPLIT)
          break;
    }
  }
  return current = network_offsets[network_index + 1], network_outcomes[network_index++].outcome;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
