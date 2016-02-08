// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cmath>

#include "neural_network.h"

namespace ufal {
namespace udpipe {
namespace parsito {

void neural_network::load_matrix(binary_decoder& data, vector<vector<float>>& m) {
  unsigned rows = data.next_4B();
  unsigned columns = data.next_4B();

  m.resize(rows);
  for (auto&& row : m) {
    const float* row_ptr = data.next<float>(columns);
    row.assign(row_ptr, row_ptr + columns);
  }
}

void neural_network::load(binary_decoder& data) {
  hidden_layer_activation = activation_function::type(data.next_1B());
  load_matrix(data, weights[0]);
  load_matrix(data, weights[1]);
}

void neural_network::propagate(const vector<embedding>& embeddings, const vector<const vector<int>*>& embedding_ids_sequences,
                               vector<float>& hidden_layer, vector<float>& outcomes, const embeddings_cache* cache, bool softmax) const {
  assert(!weights[0].empty());
  assert(!weights[1].empty());
  for (auto&& embedding_ids : embedding_ids_sequences) if (embedding_ids) assert(embeddings.size() == embedding_ids->size());

  unsigned hidden_layer_size = weights[0].front().size();
  unsigned outcomes_size = weights[1].front().size();

  outcomes.assign(outcomes_size, 0);

  // Hidden layer
  hidden_layer.assign(hidden_layer_size, 0);

  unsigned index = 0;
  for (unsigned sequence = 0; sequence < embedding_ids_sequences.size(); sequence++)
    for (unsigned i = 0; i < embeddings.size(); index += embeddings[i].dimension, i++)
      if (embedding_ids_sequences[sequence] && embedding_ids_sequences[sequence]->at(i) >= 0) {
        unsigned word = embedding_ids_sequences[sequence]->at(i);
        if (cache && i < cache->size() && word < cache->at(i).size()) {
          // Use cache
          const float* precomputed = cache->at(i)[word].data() + sequence * hidden_layer_size;
          for (unsigned j = 0; j < hidden_layer_size; j++)
            hidden_layer[j] += precomputed[j];
        } else {
          // Compute directly
          const float* embedding = embeddings[i].weight(word);
          for (unsigned j = 0; j < embeddings[i].dimension; j++)
            for (unsigned k = 0; k < hidden_layer_size; k++)
              hidden_layer[k] += embedding[j] * weights[0][index + j][k];
        }
      }
  for (unsigned i = 0; i < hidden_layer_size; i++) // Bias
    hidden_layer[i] += weights[0][index][i];

  // Activation function
  switch (hidden_layer_activation) {
    case activation_function::TANH:
      if (!tanh_cache.empty())
        for (auto&& weight : hidden_layer)
          weight = weight <= -10 ? -1 : weight >= 10 ? 1 : tanh_cache[int(weight * 32768 + 10 * 32768)];
      else
        for (auto&& weight : hidden_layer)
          weight = tanh(weight);
      break;
    case activation_function::CUBIC:
      for (auto&& weight : hidden_layer)
        weight = weight * weight * weight;
      break;
  }

  for (unsigned i = 0; i < hidden_layer_size; i++)
    for (unsigned j = 0; j < outcomes_size; j++)
      outcomes[j] += hidden_layer[i] * weights[1][i][j];
  for (unsigned i = 0; i < outcomes_size; i++) // Bias
    outcomes[i] += weights[1][hidden_layer_size][i];

  // Softmax if requested
  if (softmax) {
    float max = outcomes[0];
    for (unsigned i = 1; i < outcomes_size; i++) if (outcomes[i] > max) max = outcomes[i];

    float sum = 0;
    for (unsigned i = 0; i < outcomes_size; i++) sum += (outcomes[i] = exp(outcomes[i] - max));
    sum = 1 / sum;

    for (unsigned i = 0; i < outcomes_size; i++) outcomes[i] *= sum;
  }
}

void neural_network::generate_tanh_cache() {
  tanh_cache.resize(2 * 10 * 32768);
  for (unsigned i = 0; i < tanh_cache.size(); i++)
    tanh_cache[i] = tanh(i / 32768.0 - 10);
}

void neural_network::generate_embeddings_cache(const vector<embedding>& embeddings, embeddings_cache& cache, unsigned max_words) const {
  unsigned embeddings_dim = 0;
  for (auto&& embedding : embeddings) embeddings_dim += embedding.dimension;

  unsigned sequences = weights[0].size() / embeddings_dim;
  assert(sequences * embeddings_dim + 1 == weights[0].size());

  unsigned hidden_layer_size = weights[0].front().size();

  cache.resize(embeddings.size());
  for (unsigned i = 0, weight_index = 0; i < embeddings.size(); weight_index += embeddings[i].dimension, i++) {
    unsigned words = 0;
    while (words < max_words && embeddings[i].weight(words)) words++;

    cache[i].resize(words);
    for (unsigned word = 0; word < words; word++) {
      const float* embedding = embeddings[i].weight(word);

      cache[i][word].assign(sequences * hidden_layer_size, 0);
      for (unsigned sequence = 0, index = weight_index; sequence < sequences; index += embeddings_dim, sequence++)
        for (unsigned j = 0; j < embeddings[i].dimension; j++)
          for (unsigned k = 0; k < hidden_layer_size; k++)
            cache[i][word][sequence * hidden_layer_size + k] += embedding[j] * weights[0][index + j][k];
    }
  }
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
