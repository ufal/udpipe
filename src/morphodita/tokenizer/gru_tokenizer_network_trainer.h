// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>
#include <random>
#include <utility>

#include "common.h"
#include "gru_tokenizer.h"
#include "gru_tokenizer_network.h"
#include "gru_tokenizer_trainer.h"
#include "unilib/utf8.h"
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
  bool train(unsigned url_email_tokenizer, unsigned segment, unsigned epochs, unsigned batch_size,
             float learning_rate, float learning_rate_final, float dropout, bool early_stopping,
             const vector<tokenized_sentence>& data, const vector<tokenized_sentence>& heldout, binary_encoder& enc, string& error);

 private:
  template <int R, int C> using matrix = gru_tokenizer_network::matrix<R, C>;
  using typename gru_tokenizer_network_implementation<D>::cached_embedding;
  using typename gru_tokenizer_network_implementation<D>::gru;

  template <int R, int C> struct matrix_trainer {
    matrix<R, C>& original;
    float w_g[R][C], b_g[R];
    float w_m[R][C], b_m[R];
    float w_v[R][C], b_v[R];

    matrix_trainer(matrix<R, C>& original) : original(original), w_g(), b_g(), w_m(), b_m(), w_v(), b_v() {}
    void update_weights(float learning_rate);
  };
  struct gru_trainer {
    matrix_trainer<D,D> X, X_r, X_z;
    matrix_trainer<D,D> H, H_r, H_z;
    vector<matrix<1, D>> states, updates, resets, resetstates, candidates, dropouts;

    gru_trainer(gru& g, unsigned segment)
        : X(g.X), X_r(g.X_r), X_z(g.X_z), H(g.H), H_r(g.H_r), H_z(g.H_z), states(segment + 1),
        updates(segment), resets(segment), resetstates(segment), candidates(segment), dropouts(segment) {}
    void update_weights(float learning_rate);
  };

  struct f1_info { double precision, recall, f1; };
  void evaluate(unsigned url_email_tokenizer, unsigned segment, const vector<tokenized_sentence>& heldout, f1_info& tokens_f1, f1_info& sentences_f1);
  void evaluate_f1(const vector<token_range>& system, const vector<token_range>& gold, f1_info& f1);

  template <int R, int C> void random_matrix(matrix<R,C>& m, mt19937& generator, float range, float bias);
  void random_gru(gru& g,  mt19937& generator, float range);

  template <int R, int C> void save_matrix(const matrix<R,C>& m, binary_encoder& enc);
  void save_gru(const gru& g, binary_encoder& enc);
};

//
// Definitions
//

template <int D>
bool gru_tokenizer_network_trainer<D>::train(unsigned url_email_tokenizer, unsigned segment, unsigned epochs, unsigned batch_size,
                                             float learning_rate_initial, float learning_rate_final, float dropout,
                                             bool early_stopping, const vector<tokenized_sentence>& data,
                                             const vector<tokenized_sentence>& heldout, binary_encoder& enc, string& error) {
  if (segment < 10) return error.assign("Segment size must be at least 10!"), false;

  unsigned characters = 0;
  for (auto&& sentence : data)
    characters += sentence.sentence.size();
  if (characters < segment) return error.assign("Not enought training data for the gru_tokenizer!"), false;

  mt19937 generator;

  float dropout_multiplier = 1.f / (1.f - dropout);
  bernoulli_distribution dropout_distribution(dropout);

  // Generate embeddings
  for (auto&& sentence : data)
    for (auto&& chr : sentence.sentence)
      if (!this->embeddings.count(chr)) {
        cached_embedding embedding;
        random_matrix(embedding.e, generator, 1.f, 0.f);
        this->embeddings.emplace(chr, embedding);
      }
  this->empty_embedding.e.clear();

  // Initialize weights
  random_gru(this->gru_fwd, generator, 1.f);
  random_gru(this->gru_bwd, generator, 1.f);
  random_matrix(this->projection_fwd, generator, 1.f, 0.f); this->projection_fwd.b[this->NO_SPLIT] = 1.f;
  random_matrix(this->projection_bwd, generator, 1.f, 0.f); this->projection_bwd.b[this->NO_SPLIT] = 1.f;

  // Train the network
  unordered_map<char32_t, matrix_trainer<1, D>> embeddings;
  for (auto&& embedding : this->embeddings)
    embeddings.emplace(embedding.first, embedding.second.e);
  vector<matrix_trainer<1, D>*> chosen_embeddings(segment);
  vector<matrix<1, D>> embedding_dropouts(segment);
  gru_trainer gru_fwd(this->gru_fwd, segment), gru_bwd(this->gru_bwd, segment);
  matrix_trainer<3, D> projection_fwd(this->projection_fwd), projection_bwd(this->projection_bwd);
  float learning_rate = learning_rate_initial, b1t = 1.f, b2t = 1.f;

  float best_sentence_f1 = 0.f; unsigned best_sentence_f1_epoch = 0;
  gru_tokenizer_network_trainer<D> best_sentence_f1_network;

  size_t training_offset = 0, training_shift;
  vector<gru_tokenizer_network::char_info> training_input, instance_input(segment);
  vector<gru_tokenizer_network::outcome_t> training_output, instance_output(segment);
  vector<int> permutation; for (size_t i = 0; i < data.size(); i++) permutation.push_back(permutation.size());
  for (unsigned epoch = 0; epoch < epochs; epoch++) {
    double logprob = 0;
    int total = 0, correct = 0;

    for (int instance = 0, instances = 10000; instance < instances; instance++) {
      // Prepare input instance
      if (training_offset + segment >= training_input.size()) {
        shuffle(permutation.begin(), permutation.end(), generator);
        training_input.clear(); training_output.clear();
        for (auto&& index : permutation) {
          auto& sentence = data[index];
          if (sentence.tokens.empty()) continue;

          training_offset = training_input.size();
          training_input.resize(training_offset + sentence.sentence.size());
          training_output.resize(training_offset + sentence.sentence.size());
          for (size_t i = 0; i < sentence.sentence.size(); i++) {
            training_input[training_offset + i].chr = sentence.sentence[i];
            training_output[training_offset + i].outcome = gru_tokenizer_network::NO_SPLIT;
          }
          for (size_t i = 0; i < sentence.tokens.size(); i++)
            if (i+1 < sentence.tokens.size() && sentence.tokens[i].start + sentence.tokens[i].length == sentence.tokens[i+1].start)
              training_output[training_offset + sentence.tokens[i].start + sentence.tokens[i].length - 1].outcome = gru_tokenizer_network::END_OF_TOKEN;
            else if (i+1 == sentence.tokens.size())
              training_output[training_offset + sentence.tokens[i].start + sentence.tokens[i].length - 1].outcome = gru_tokenizer_network::END_OF_SENTENCE;
        }
        training_offset = 0;
      }
      copy_n(training_input.begin() + training_offset, segment, instance_input.begin());
      copy_n(training_output.begin() + training_offset, segment, instance_output.begin());

      // Shift training_offset
      for (training_shift = segment - 5; training_shift > segment / 2; training_shift--)
        if (instance_output[training_shift-1].outcome != gru_tokenizer_network::NO_SPLIT || instance_input[training_shift-1].chr == ' ')
          break;
      training_offset += training_shift;

      // Forward pass
      for (unsigned i = 0; i < segment; i++) {
        chosen_embeddings[i] = &embeddings.at(instance_input[i].chr);
        for (unsigned k = 0; k < D; k++)
          embedding_dropouts[i].w[0][k] = dropout && dropout_distribution(generator) ? 0.f : dropout_multiplier;
        for (int j = 0; j < 3; j++)
          instance_output[i].w[j] = projection_fwd.original.b[j];
      }

      for (int dir = 0; dir < 2; dir++) {
        auto& gru = dir == 0 ? gru_fwd : gru_bwd;
        auto& projection = dir == 0 ? projection_fwd : projection_bwd;

        gru.states[0].clear();
        for (size_t i = 0; i < segment; i++) {
          auto& embedding = chosen_embeddings[dir == 0 ? i : segment - 1 - i];
          auto& embedding_dropout = embedding_dropouts[dir == 0 ? i : segment - 1 - i];
          auto& output = instance_output[dir == 0 ? i : segment - 1 - i];

          for (int j = 0; j < D; j++) {
            gru.updates[i].w[0][j] = gru.X_z.original.b[j];
            gru.resets[i].w[0][j] = gru.X_r.original.b[j];
            for (int k = 0; k < D; k++) {
              gru.updates[i].w[0][j] += embedding_dropout.w[0][k] * embedding->original.w[0][k] * gru.X_z.original.w[j][k] + gru.states[i].w[0][k] * gru.H_z.original.w[j][k];
              gru.resets[i].w[0][j] += embedding_dropout.w[0][k] * embedding->original.w[0][k] * gru.X_r.original.w[j][k] + gru.states[i].w[0][k] * gru.H_r.original.w[j][k];
            }
            gru.updates[i].w[0][j] = 1.f / (1.f + exp(-gru.updates[i].w[0][j]));
            gru.resets[i].w[0][j] = 1.f / (1.f + exp(-gru.resets[i].w[0][j]));
            gru.resetstates[i].w[0][j] = gru.resets[i].w[0][j] * gru.states[i].w[0][j];
          }
          for (int j = 0; j < D; j++) {
            gru.candidates[i].w[0][j] = gru.X.original.b[j];
            for (int k = 0; k < D; k++)
              gru.candidates[i].w[0][j] += embedding_dropout.w[0][k] * embedding->original.w[0][k] * gru.X.original.w[j][k] + gru.resetstates[i].w[0][k] * gru.H.original.w[j][k];
            gru.candidates[i].w[0][j] = tanh(gru.candidates[i].w[0][j]);
            gru.states[i+1].w[0][j] = gru.updates[i].w[0][j] * gru.states[i].w[0][j] + (1.f - gru.updates[i].w[0][j]) * gru.candidates[i].w[0][j];
          }

          for (int j = 0; j < D; j++)
            gru.dropouts[i].w[0][j] = dropout && dropout_distribution(generator) ? 0.f : dropout_multiplier * gru.states[i+1].w[0][j];

          for (int j = 0; j < 3; j++)
            for (int k = 0; k < D; k++)
              output.w[j] += projection.original.w[j][k] * gru.dropouts[i].w[0][k];
        }
      }

      for (auto&& output : instance_output) {
        int best = output.w[1] > output.w[0];
        if (output.w[2] > output.w[best]) best = 2;
        float maximum = output.w[best], sum = 0;
        for (int j = 0; j < 3; j++) sum += (output.w[j] = exp(output.w[j] - maximum));
        sum = 1.f / sum;
        for (int j = 0; j < 3; j++) output.w[j] *= sum;

        total++;
        correct += best == output.outcome;
        logprob += log(output.w[output.outcome]);
      }

      // Backward pass
      for (auto&& output : instance_output)
        for (int j = 0; j < 3; j++)
          output.w[j] = (output.outcome == j) - output.w[j];

      for (int dir = 0; dir < 2; dir++) {
        auto& gru = dir == 0 ? gru_fwd : gru_bwd;
        auto& projection = dir == 0 ? projection_fwd : projection_bwd;

        matrix<1, D> state_g, update_g, candidate_g, reset_g, resetstate_g;
        state_g.clear();
        for (size_t i = segment; i--; ) {
          auto& embedding = chosen_embeddings[dir == 0 ? i : segment - 1 - i];
          auto& embedding_dropout = embedding_dropouts[dir == 0 ? i : segment - 1 - i];
          auto& output = instance_output[dir == 0 ? i : segment - 1 - i];

          for (int j = 0; j < D; j++) // These for cycles are swapped because
            for (int k = 0; k < 3; k++) // g++-4.8 generates wrong code otherwise.
              projection.w_g[k][j] += gru.dropouts[i].w[0][j] * output.w[k];

          for (int j = 0; j < D; j++)
            if (gru.dropouts[i].w[0][j])
              for (int k = 0; k < 3; k++)
                state_g.w[0][j] += projection.original.w[k][j] * output.w[k];

          resetstate_g.clear();
          for (int j = 0; j < D; j++) {
            update_g.w[0][j] = state_g.w[0][j] * (gru.states[i].w[0][j] - gru.candidates[i].w[0][j]);
            candidate_g.w[0][j] = state_g.w[0][j] * (1.f - gru.updates[i].w[0][j]);
            state_g.w[0][j] = state_g.w[0][j] * gru.updates[i].w[0][j];

            candidate_g.w[0][j] *= 1 - gru.candidates[i].w[0][j] * gru.candidates[i].w[0][j];
            gru.X.b_g[j] += candidate_g.w[0][j];
            for (int k = 0; k < D; k++) {
              gru.X.w_g[j][k] += candidate_g.w[0][j] * embedding_dropout.w[0][k] * embedding->original.w[0][k];
              gru.H.w_g[j][k] += candidate_g.w[0][j] * gru.resetstates[i].w[0][k];
              embedding->w_g[0][k] += embedding_dropout.w[0][k] * candidate_g.w[0][j] * gru.X.original.w[j][k];
              resetstate_g.w[0][k] += candidate_g.w[0][j] * gru.H.original.w[j][k];
            }
          }
          for (int j = 0; j < D; j++) {
            state_g.w[0][j] += resetstate_g.w[0][j] * gru.resets[i].w[0][j];
            reset_g.w[0][j] = resetstate_g.w[0][j] * gru.states[i].w[0][j];

            update_g.w[0][j] *= gru.updates[i].w[0][j] * (1 - gru.updates[i].w[0][j]);
            reset_g.w[0][j] *= gru.resets[i].w[0][j] * (1 - gru.resets[i].w[0][j]);

            gru.X_z.b_g[j] += update_g.w[0][j];
            gru.X_r.b_g[j] += reset_g.w[0][j];
            for (int k = 0; k < D; k++) {
              gru.X_z.w_g[j][k] += update_g.w[0][j] * embedding_dropout.w[0][k] * embedding->original.w[0][k];
              gru.H_z.w_g[j][k] += update_g.w[0][j] * gru.states[i].w[0][k];
              gru.X_r.w_g[j][k] += reset_g.w[0][j] * embedding_dropout.w[0][k] * embedding->original.w[0][k];
              gru.H_r.w_g[j][k] += reset_g.w[0][j] * gru.states[i].w[0][k];
              embedding->w_g[0][k] += embedding_dropout.w[0][k] * (update_g.w[0][j] * gru.X_z.original.w[j][k] +
                                                                   reset_g.w[0][j] * gru.X_r.original.w[j][k]);
              state_g.w[0][k] += update_g.w[0][j] * gru.H_z.original.w[j][k] + reset_g.w[0][j] * gru.H_r.original.w[j][k];
            }
          }
        }
      }

      // Update the weights
      if (batch_size == 1 ||
          instance+1 == instances ||
          (instance+1) % batch_size == 0) {
        b1t *= 0.9f;
        b2t *= 0.999f;
        float learning_rate_biased = learning_rate * sqrt(1-b2t) / (1-b1t);

        if (batch_size == 1)
          for (auto&& chosen_embedding : chosen_embeddings)
            chosen_embedding->update_weights(learning_rate_biased);
        else
          for (auto&& embedding : embeddings)
            embedding.second.update_weights(learning_rate_biased);
        gru_fwd.update_weights(learning_rate_biased);
        gru_bwd.update_weights(learning_rate_biased);
        projection_fwd.update_weights(learning_rate_biased);
        projection_bwd.update_weights(learning_rate_biased);
      }
    }
    if (learning_rate_final && learning_rate_final != learning_rate_initial)
      learning_rate = exp(((epochs - epoch - 2) * log(learning_rate_initial) + (epoch + 1) * log(learning_rate_final)) / (epochs - 1));

    // Evaluate
    cerr << "Epoch " << epoch+1 << ", logprob: " << scientific << setprecision(4) << logprob
         << ", training acc: " << fixed << setprecision(2) << 100. * correct / double(total) << "%";
    if (!heldout.empty()) {
      f1_info tokens, sentences;
      evaluate(url_email_tokenizer, segment, heldout, tokens, sentences);
      cerr << ", heldout tokens: " << 100. * tokens.precision << "%P/" << 100. * tokens.recall << "%R/"
           << 100. * tokens.f1 << "%, sentences: " << 100. * sentences.precision << "%P/"
           << 100. * sentences.recall << "%R/" << 100. * sentences.f1 << "%";

      if (early_stopping && sentences.f1 > best_sentence_f1) {
        best_sentence_f1 = sentences.f1;
        best_sentence_f1_epoch = epoch;
        best_sentence_f1_network = *this;
      }
      if (early_stopping && best_sentence_f1 && epoch - best_sentence_f1_epoch > 30) {
        cerr << endl << "Stopping after 30 iterations of not improving sentence f1." << endl;
        break;
      }
    }
    cerr << endl;
  }

  // Choose best network if desired
  if (early_stopping && best_sentence_f1) {
    cerr << "Choosing parameters from epoch " << best_sentence_f1_epoch+1 << "." << endl;
    this->embeddings = best_sentence_f1_network.embeddings;
    this->gru_fwd = best_sentence_f1_network.gru_fwd;
    this->gru_bwd = best_sentence_f1_network.gru_bwd;
    this->projection_fwd = best_sentence_f1_network.projection_fwd;
    this->projection_bwd = best_sentence_f1_network.projection_bwd;
  }

  // Encode the network
  enc.add_1B(1);
  enc.add_1B(D);

  enc.add_4B(this->embeddings.size());
  for (auto&& embedding : this->embeddings) {
    enc.add_4B(embedding.first);
    enc.add_data(embedding.second.e.w[0], D);
  }
  save_gru(this->gru_fwd, enc);
  save_gru(this->gru_bwd, enc);
  save_matrix(this->projection_fwd, enc);
  save_matrix(this->projection_bwd, enc);

  return true;
}

template <int D> template <int R, int C>
void gru_tokenizer_network_trainer<D>::matrix_trainer<R, C>::update_weights(float learning_rate) {
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < C; j++) {
      w_m[i][j] = 0.9 * w_m[i][j] + (1-0.9) * w_g[i][j];
      w_v[i][j] = 0.999 * w_v[i][j] + (1-0.999) * w_g[i][j] * w_g[i][j];
      original.w[i][j] += learning_rate * w_m[i][j] / (sqrt(w_v[i][j]) + 1e-8);
    }
    b_m[i] = 0.9 * b_m[i] + (1-0.9) * b_g[i];
    b_v[i] = 0.999 * b_v[i] + (1-0.999) * b_g[i] * b_g[i];
    original.b[i] += learning_rate * b_m[i] / (sqrt(b_v[i]) + 1e-8);
  }

  for (int i = 0; i < R; i++) {
    for (int j = 0; j < C; j++)
      w_g[i][j] = 0.f;
    b_g[i] = 0.f;
  }
}

template <int D>
void gru_tokenizer_network_trainer<D>::gru_trainer::update_weights(float learning_rate) {
  X.update_weights(learning_rate);
  X_r.update_weights(learning_rate);
  X_z.update_weights(learning_rate);
  H.update_weights(learning_rate);
  H_r.update_weights(learning_rate);
  H_z.update_weights(learning_rate);
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

  this->cache_embeddings();
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
