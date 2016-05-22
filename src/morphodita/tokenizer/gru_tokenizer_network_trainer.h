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
#include "gru_tokenizer_network.h"
#include "utils/binary_encoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

//
// Declarations
//

template <int D>
class gru_tokenizer_network_trainer : public gru_tokenizer_network_implementation<D> {
 public:
  static bool train(unsigned segment, binary_encoder& enc, string& error);

 private:
  template <int R, int C> static void save_matrix(const gru_tokenizer_network::matrix<R,C>& m, binary_encoder& enc);
  static void save_gru(const typename gru_tokenizer_network_implementation<D>::gru& g, binary_encoder& enc);

};

//
// Defititions
//

template <int D>
bool gru_tokenizer_network_trainer<D>::train(unsigned int /*segment*/, binary_encoder& enc, string& /*error*/) {
  gru_tokenizer_network_trainer<D> network;

  // Train the network

  // TODO

  // Encode the network
  enc.add_1B(1);
  enc.add_1B(D);

  enc.add_4B(network.embeddings.size());
  for (auto&& embedding : network.embeddings) {
    enc.add_4B(embedding.first);
    enc.add_data(embedding.second.w[0], D);
  }
  save_gru(network.gru_fwd, enc);
  save_gru(network.gru_bwd, enc);
  save_matrix(network.projection, enc);

  return true;
}

template <int D> template <int R, int C>
void gru_tokenizer_network_trainer<D>::save_matrix(const gru_tokenizer_network::matrix<R,C>& m, binary_encoder& enc) {
  for (int i = 0; i < R; i++)
    enc.add_data(m.w[i], C);
  enc.add_data(m.b, R);
}

template <int D>
void gru_tokenizer_network_trainer<D>::save_gru(const typename gru_tokenizer_network_implementation<D>::gru& g, binary_encoder& enc) {
  save_matrix(g.X, enc);
  save_matrix(g.X_r, enc);
  save_matrix(g.X_z, enc);
  save_matrix(g.H, enc);
  save_matrix(g.H_r, enc);
  save_matrix(g.H_z, enc);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
