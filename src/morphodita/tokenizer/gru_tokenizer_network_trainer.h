// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <random>
#include <utility>

#include "common.h"
#include "gru_tokenizer.h"
#include "gru_tokenizer_network.h"
#include "gru_tokenizer_trainer.h"
#include "unicode/utf8.h"
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
  bool train(unsigned url_email_tokenizer, unsigned segment, const vector<tokenized_sentence>& data, const vector<tokenized_sentence>& heldout, binary_encoder& enc, string& error);

 private:
  template <int R, int C> using matrix = gru_tokenizer_network::matrix<R, C>;
  using typename gru_tokenizer_network_implementation<D>::gru;

  struct f1_info { double precision, recall, f1; };
  void evaluate(unsigned url_email_tokenizer, unsigned segment, const vector<tokenized_sentence>& heldout, f1_info& tokens_f1, f1_info& sentences_f1);
  void evaluate_f1(const vector<token_range>& system, const vector<token_range>& gold, f1_info& f1);

  template <int R, int C> void random_matrix(matrix<R,C>& m, mt19937& generator, float range, float bias);
  void random_gru(gru& g,  mt19937& generator, float range);

  template <int R, int C> void save_matrix(const matrix<R,C>& m, binary_encoder& enc);
  void save_gru(const gru& g, binary_encoder& enc);
};

//
// Defititions
//

template <int D>
bool gru_tokenizer_network_trainer<D>::train(unsigned url_email_tokenizer, unsigned segment, const vector<tokenized_sentence>& data, const vector<tokenized_sentence>& heldout, binary_encoder& enc, string& /*error*/) {
  mt19937 generator;

  // Generate embeddings
  for (auto&& sentence : data)
    for (auto&& chr : sentence.sentence)
      if (!this->embeddings.count(chr)) {
        matrix<1, D> embedding;
        random_matrix(embedding, generator, 1.f, 0.f);
        this->embeddings.emplace(chr, embedding);
      }
  this->empty_embedding.clear();

  // Initialize weights
  random_gru(this->gru_fwd, generator, 1.f);
  random_gru(this->gru_bwd, generator, 1.f);
  random_matrix(this->projection_fwd, generator, 1.f, 0.f); this->projection_fwd.b[this->NO_SPLIT] = 1.f;
  random_matrix(this->projection_bwd, generator, 1.f, 0.f); this->projection_bwd.b[this->NO_SPLIT] = 1.f;

  // Train the network
  for (int epoch = 0; epoch < 1; epoch++) {
    double logprob = 0;
    int total = 0, correct = 0;

    for (int instance = 0; instance < 10; instance++) {
      // Create input

      // Train the network

      // Update the weights
    }

    // Evaluate
    cerr << "Epoch " << epoch+1 << ", logprob: " << scientific << setprecision(4) << logprob
         << ", training acc: " << fixed << setprecision(2) << 100. * correct / double(total) << "%";
    if (!heldout.empty()) {
      f1_info tokens, sentences;
      evaluate(url_email_tokenizer, segment, heldout, tokens, sentences);
      cerr << ", heldout tokens: " << 100. * tokens.precision << "%P/" << 100. * tokens.recall << "%R/"
           << 100. * tokens.f1 << "%, sentences: " << 100. * sentences.precision << "%P/"
           << 100. * sentences.recall << "%R/" << 100. * sentences.f1 << "%";
    }
    cerr << endl;
  }


  // Encode the network
  enc.add_1B(1);
  enc.add_1B(D);

  enc.add_4B(this->embeddings.size());
  for (auto&& embedding : this->embeddings) {
    enc.add_4B(embedding.first);
    enc.add_data(embedding.second.w[0], D);
  }
  save_gru(this->gru_fwd, enc);
  save_gru(this->gru_bwd, enc);
  save_matrix(this->projection_fwd, enc);
  save_matrix(this->projection_bwd, enc);

  return true;
}

template <int D>
void gru_tokenizer_network_trainer<D>::evaluate(unsigned url_email_tokenizer, unsigned segment, const vector<tokenized_sentence>& heldout, f1_info& tokens_f1, f1_info& sentences_f1) {
  // Generate gold data
  vector<token_range> gold_sentences, gold_tokens;
  u32string text;
  for (auto&& sentence : heldout) {
    if (sentence.tokens.empty()) continue;

    gold_sentences.emplace_back(text.size() + sentence.tokens.front().start, sentence.tokens.back().start + sentence.tokens.back().length - sentence.tokens.front().start);
    for (auto&& token : sentence.tokens)
      gold_tokens.emplace_back(text.size() + token.start, token.length);
    text.append(sentence.sentence).push_back(' ');
  }

  // Generate system data
  vector<token_range> system_sentences, system_tokens, tokens;
  string text_utf8;

  gru_tokenizer tokenizer(url_email_tokenizer, segment, *this);
  unilib::utf8::encode(text, text_utf8);
  tokenizer.set_text(text_utf8);

  while (tokenizer.next_sentence(tokens))
    if (!tokens.empty()) {
      system_sentences.emplace_back(tokens.front().start, tokens.back().start + tokens.back().length - tokens.front().start);
      system_tokens.insert(system_tokens.end(), tokens.begin(), tokens.end());
    }

  evaluate_f1(system_tokens, gold_tokens, tokens_f1);
  evaluate_f1(system_sentences, gold_sentences, sentences_f1);
}

template <int D>
void gru_tokenizer_network_trainer<D>::evaluate_f1(const vector<token_range>& system, const vector<token_range>& gold, f1_info& f1) {
  size_t both = 0;
  for (size_t si = 0, gi = 0; si < system.size() || gi < gold.size(); )
    if (si < system.size() && (gi == gold.size() || system[si].start < gold[gi].start))
      si++;
    else if (gi < gold.size() && (si == system.size() || gold[gi].start < system[si].start))
      gi++;
    else
      both += system[si++].length == gold[gi++].length;

  f1.precision = system.size() ? both / double(system.size()) : 0.;
  f1.recall = gold.size() ? both / double(gold.size()) : 0.;
  f1.f1 = system.size()+gold.size() ? 2 * both / double(system.size() + gold.size()) : 0.;
}

template <int D> template <int R, int C>
void gru_tokenizer_network_trainer<D>::random_matrix(matrix<R,C>& m, mt19937& generator, float range, float bias) {
  uniform_real_distribution<float> uniform(-range, range);
  for (int i = 0; i < R; i++) {
    m.b[i] = bias;
    for (int j = 0; j < C; j++)
      m.w[i][j] = uniform(generator);
  }
}

template <int D>
void gru_tokenizer_network_trainer<D>::random_gru(gru& g, mt19937& generator, float range) {
  random_matrix(g.X, generator, range, 0.f);
  random_matrix(g.X_r, generator, range, 1.f);
  random_matrix(g.X_z, generator, range, 1.f);
  random_matrix(g.H, generator, range, 0.f);
  random_matrix(g.H_r, generator, range, 1.f);
  random_matrix(g.H_z, generator, range, 1.f);
}

template <int D> template <int R, int C>
void gru_tokenizer_network_trainer<D>::save_matrix(const matrix<R,C>& m, binary_encoder& enc) {
  for (int i = 0; i < R; i++)
    enc.add_data(m.w[i], C);
  enc.add_data(m.b, R);
}

template <int D>
void gru_tokenizer_network_trainer<D>::save_gru(const gru& g, binary_encoder& enc) {
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
