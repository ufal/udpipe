// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "unicode_tokenizer.h"
#include "gru_tokenizer_network.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class gru_tokenizer : public unicode_tokenizer {
 public:
  gru_tokenizer(unsigned url_email_tokenizer, unsigned segment, const gru_tokenizer_network& network)
      : unicode_tokenizer(url_email_tokenizer), segment(segment), network_index(0), network_length(0), network(network) {}

  virtual bool next_sentence(vector<token_range>& tokens) override;

 private:
  inline bool is_space(size_t index);
  int next_outcome();

  unsigned segment, network_index, network_length;
  vector<gru_tokenizer_network::char_info> network_chars;
  vector<gru_tokenizer_network::outcome_t> network_outcomes;
  vector<size_t> network_offsets;
  const gru_tokenizer_network& network;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
