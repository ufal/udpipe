// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <unordered_map>

#include "gru_tokenizer_network_trainer.h"
#include "gru_tokenizer_trainer.h"
#include "unilib/unicode.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool gru_tokenizer_trainer::train(unsigned url_email_tokenizer, unsigned segment, bool allow_spaces, unsigned dimension, unsigned epochs,
                                  unsigned batch_size, float learning_rate, float learning_rate_final, float dropout,
                                  float initialization_range, bool early_stopping, const vector<tokenized_sentence>& data,
                                  const vector<tokenized_sentence>& heldout, ostream& os, string& error) {
  using namespace unilib;

  error.clear();

  // Start encoding the tokenizer
  os.put(2);

  binary_encoder enc;
  enc.add_1B(url_email_tokenizer);
  enc.add_2B(segment);
  enc.add_1B(allow_spaces);

  // Train the GRU network
  if (dimension == 16) {
    gru_tokenizer_network_trainer<16> network;
    if (!network.train(url_email_tokenizer, segment, allow_spaces, epochs, batch_size, learning_rate, learning_rate_final,
                       dropout, initialization_range, early_stopping, data, heldout, enc, error)) return false;
  } else if (dimension == 24) {
    gru_tokenizer_network_trainer<24> network;
    if (!network.train(url_email_tokenizer, segment, allow_spaces, epochs, batch_size, learning_rate, learning_rate_final,
                       dropout, initialization_range, early_stopping, data, heldout, enc, error)) return false;
  } else if (dimension == 64) {
    gru_tokenizer_network_trainer<64> network;
    if (!network.train(url_email_tokenizer, segment, allow_spaces, epochs, batch_size, learning_rate, learning_rate_final,
                       dropout, initialization_range, early_stopping, data, heldout, enc, error)) return false;
  } else {
    return error.assign("Gru tokenizer dimension '").append(to_string(dimension)).append("' is not supported!"), false;
  }

  // Compute best substitutions for every category
  unordered_map<unicode::category_t, unordered_map<char32_t, unsigned>> counts;
  for (auto&& sentence : data)
    for (auto&& chr : sentence.sentence)
      counts[unicode::category(chr)][chr]++;

  unordered_map<unicode::category_t, char32_t> unknown_chars;
  for (auto&& count : counts) {
    char32_t best_chr = 0;
    unsigned best = 0;
    for (auto&& chr : count.second)
      if (chr.second > best)
        best = chr.second, best_chr = chr.first;
    if (best_chr)
      unknown_chars.emplace(count.first, best_chr);
  }
  enc.add_1B(unknown_chars.size());
  for (auto&& unknown_char : unknown_chars) {
    enc.add_4B(unknown_char.first);
    enc.add_4B(unknown_char.second);
  }

  if (!compressor::save(os, enc)) return error.assign("Cannot save gru_tokenizer_factory!"), false;
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
