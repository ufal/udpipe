// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "unicode_tokenizer.h"
#include "gru_tokenizer_network.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class gru_tokenizer : public unicode_tokenizer {
 public:
  typedef unordered_map<unilib::unicode::category_t, char32_t> unknown_chars_map;

  gru_tokenizer(unsigned url_email_tokenizer, unsigned segment, const gru_tokenizer_network& network, const unknown_chars_map& unknown_chars)
      : unicode_tokenizer(url_email_tokenizer), segment(segment), network_chars(segment), network_outcomes(segment), network(network), unknown_chars(unknown_chars) {}

  virtual bool next_sentence(vector<token_range>& tokens) override;

 private:
  unsigned segment;
  vector<char32_t> network_chars;
  vector<gru_tokenizer_network::outcome_t> network_outcomes;
  const gru_tokenizer_network& network;
  const unknown_chars_map& unknown_chars;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
