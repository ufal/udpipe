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

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations

class gru_tokenizer_network {
 public:
  virtual ~gru_tokenizer_network() {}

  enum outcome_t { NO_SPLIT, END_OF_TOKEN, END_OF_SENTENCE, OUTCOMES };
  virtual void compute(const vector<char32_t>& chars, vector<outcome_t>& outcomes) const = 0;

  static gru_tokenizer_network* load(istream& is);
};

template <int D>
class gru_tokenizer_network_implementation : public gru_tokenizer_network {
 public:
  virtual void compute(const vector<char32_t>& chars, vector<outcome_t>& outcomes) const override;

  static gru_tokenizer_network_implementation<D>* load(istream& is);

 protected:
  struct array { float w[D]; };
  struct matrix { float w[D][D]; float b[D]; };

  unordered_map<char32_t, array> embeddings;
  matrix X, X_r, X_z, H, H_r, H_z;
};

// Definitions

template <int D>
void gru_tokenizer_network_implementation<D>::compute(const vector<char32_t>& chars, vector<outcome_t>& outcomes) const {
}

template <int D>
gru_tokenizer_network_implementation<D>* gru_tokenizer_network_implementation<D>::load(istream& /*is*/) {
  // TODO

  return nullptr;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
