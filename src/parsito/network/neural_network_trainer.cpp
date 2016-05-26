// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "neural_network_trainer.h"
#include "trainer/training_failure.h"

namespace ufal {
namespace udpipe {
namespace parsito {

neural_network_trainer::neural_network_trainer(neural_network& network, unsigned input_size, unsigned output_size,
                                               const network_parameters& parameters, mt19937& generator) : network(network), generator(generator) {
  uniform_real_distribution<float> uniform(-parameters.initialization_range, parameters.initialization_range);

  // Initialize hidden layer
  network.hidden_layer_activation = parameters.hidden_layer_type;
  if (parameters.hidden_layer) {
    network.weights[0].resize(input_size + 1/*bias*/);
    for (auto&& row : network.weights[0]) {
      row.resize(parameters.hidden_layer);
      for (auto&& weight : row)
        weight = uniform(generator);
    }

    network.weights[1].resize(parameters.hidden_layer + 1/*bias*/);
    for (auto&& row : network.weights[1]) {
      row.resize(output_size);
      for (auto&& weight : row)
        weight = uniform(generator);
    }
  }

  // Store the network_parameters
  iteration = 0;
  iterations = parameters.iterations;
  trainer = parameters.trainer;
  batch_size = parameters.batch_size;
  l1_regularization = parameters.l1_regularization;
  l2_regularization = parameters.l2_regularization;
  maxnorm_regularization = parameters.maxnorm_regularization;
  dropout_hidden = parameters.dropout_hidden;
  dropout_input = parameters.dropout_input;

  // Maxnorm regularize the created weights
  if (maxnorm_regularization) maxnorm_regularize();
}

bool neural_network_trainer::next_iteration() {
  if (iteration++ >= iterations) return false;

  if (trainer.algorithm == network_trainer::SGD)
    if (trainer.learning_rate != trainer.learning_rate_final && iteration > 1)
      trainer.learning_rate =
          exp(((iterations - iteration) * log(trainer.learning_rate) + log(trainer.learning_rate_final)) / (iterations - iteration + 1));

  return true;
}

void neural_network_trainer::propagate(const vector<embedding>& embeddings, const vector<const vector<int>*>& embedding_ids_sequences, workspace& w) const {
  // Initialize dropout if requested
  if (dropout_input) {
    w.input_dropout.resize(network.weights[0].size());
    bernoulli_distribution dropout(1 - dropout_input);
    for (auto&& flag : w.input_dropout)
      flag = dropout(generator);
  }

  if (dropout_hidden) {
    w.hidden_dropout.resize(network.weights[1].size());
    bernoulli_distribution dropout(1 - dropout_hidden);
    for (auto&& flag : w.hidden_dropout)
      flag = dropout(generator);
  }
  w.hidden_kept.clear();
  for (unsigned i = 0; i < network.weights[0].front().size(); i++)
    if (w.hidden_dropout.empty() || !w.hidden_dropout[i])
      w.hidden_kept.push_back(i);

  // Propagate
  unsigned hidden_layer_size = network.weights[0].front().size();
  unsigned outcomes_size = network.weights[1].front().size();

  w.outcomes.assign(outcomes_size, 0);

  // Hidden layer
  w.hidden_layer.assign(hidden_layer_size, 0);

  unsigned index = 0;
  for (auto&& embedding_ids : embedding_ids_sequences)
    for (unsigned i = 0; i < embeddings.size(); i++)
      if (embedding_ids && (*embedding_ids)[i] >= 0) {
        const float* embedding = embeddings[i].weight((*embedding_ids)[i]);
        for (unsigned dimension = embeddings[i].dimension; dimension; dimension--, embedding++, index++)
          if (w.input_dropout.empty() || !w.input_dropout[index])
            for (auto&& j : w.hidden_kept)
              w.hidden_layer[j] += *embedding * network.weights[0][index][j];
      } else {
        index += embeddings[i].dimension;
      }
  if (w.input_dropout.empty() || !w.input_dropout[index]) // Bias
    for (auto&& i : w.hidden_kept)
      w.hidden_layer[i] += network.weights[0][index][i];

  // Activation function
  switch (network.hidden_layer_activation) {
    case activation_function::TANH:
      for (auto&& weight : w.hidden_layer)
        weight = tanh(weight);
      break;
    case activation_function::CUBIC:
      for (auto&& weight : w.hidden_layer)
        weight = weight * weight * weight;
      break;
  }

  for (auto&& i : w.hidden_kept)
    for (unsigned j = 0; j < outcomes_size; j++)
      w.outcomes[j] += w.hidden_layer[i] * network.weights[1][i][j];
  if (w.hidden_dropout.empty() || !w.hidden_dropout[hidden_layer_size]) // Bias
    for (unsigned i = 0; i < outcomes_size; i++)
      w.outcomes[i] += network.weights[1][hidden_layer_size][i];

  // Softmax
  float max = w.outcomes[0];
  for (unsigned i = 1; i < outcomes_size; i++) if (w.outcomes[i] > max) max = w.outcomes[i];

  float sum = 0;
  for (unsigned i = 0; i < outcomes_size; i++) sum += (w.outcomes[i] = exp(w.outcomes[i] - max));
  sum = 1 / sum;

  for (unsigned i = 0; i < outcomes_size; i++) w.outcomes[i] *= sum;
}

// SGD
bool neural_network_trainer::trainer_sgd::need_trainer_data = false;
float neural_network_trainer::trainer_sgd::delta(float gradient, const network_trainer& trainer, workspace::trainer_data& /*data*/) {
  return trainer.learning_rate * gradient;
}

// SGD with momentum
bool neural_network_trainer::trainer_sgd_momentum::need_trainer_data = true;
float neural_network_trainer::trainer_sgd_momentum::delta(float gradient, const network_trainer& trainer, workspace::trainer_data& data) {
  data.delta = trainer.momentum * data.delta + trainer.learning_rate * gradient;
  return data.delta;
}

// AdaGrad
bool neural_network_trainer::trainer_adagrad::need_trainer_data = true;
float neural_network_trainer::trainer_adagrad::delta(float gradient, const network_trainer& trainer, workspace::trainer_data& data) {
  data.gradient += gradient * gradient;
  return trainer.learning_rate / sqrt(data.gradient + trainer.epsilon) * gradient;
}

// AdaDelta
bool neural_network_trainer::trainer_adadelta::need_trainer_data = true;
float neural_network_trainer::trainer_adadelta::delta(float gradient, const network_trainer& trainer, workspace::trainer_data& data) {
  data.gradient = trainer.momentum * data.gradient + (1 - trainer.momentum) * gradient * gradient;
  float delta = sqrt(data.delta + trainer.epsilon) / sqrt(data.gradient + trainer.epsilon) * gradient;
  data.delta = trainer.momentum * data.delta + (1 - trainer.momentum) * delta * delta;
  return delta;
}

// Backpropagation
template <class TRAINER>
void neural_network_trainer::backpropagate_template(vector<embedding>& embeddings, const vector<const vector<int>*>& embedding_ids_sequences, unsigned required_outcome, workspace& w) {
  size_t hidden_layer_size = network.weights[0].front().size();
  size_t outcomes_size = network.weights[1].front().size();

  // Allocate space for delta accumulators
  if (network.weights[0].size() > w.weights_batch[0].size()) w.weights_batch[0].resize(network.weights[0].size());
  if (network.weights[1].size() > w.weights_batch[1].size()) w.weights_batch[1].resize(network.weights[1].size());
  if (embeddings.size() > w.error_embedding.size()) w.error_embedding.resize(embeddings.size());
  if (embeddings.size() > w.error_embedding_nonempty.size()) w.error_embedding_nonempty.resize(embeddings.size());

  // Allocate space for trainer_data if required)
  workspace::trainer_data none_trainer_data;
  if (TRAINER::need_trainer_data) {
    while (network.weights[0].size() > w.weights_trainer[0].size()) w.weights_trainer[0].emplace_back(network.weights[0].front().size());
    while (network.weights[1].size() > w.weights_trainer[1].size()) w.weights_trainer[1].emplace_back(outcomes_size);
  }

  // Compute error vector
  w.error_outcomes.resize(outcomes_size);
  for (unsigned i = 0; i < outcomes_size; i++)
    w.error_outcomes[i] = (i == required_outcome) - w.outcomes[i];

  // Backpropagate error_outcomes to error_hidden
  w.error_hidden.assign(hidden_layer_size, 0);
  for (auto&& i : w.hidden_kept)
    for (unsigned j = 0; j < outcomes_size; j++)
      w.error_hidden[i] += network.weights[1][i][j] * w.error_outcomes[j];

  // Perform activation function derivation
  switch (network.hidden_layer_activation) {
    case activation_function::TANH:
      for (auto&& i : w.hidden_kept)
        w.error_hidden[i] *= 1 - w.hidden_layer[i] * w.hidden_layer[i];
      break;
    case activation_function::CUBIC:
      for (auto&& i : w.hidden_kept) {
        float hidden_layer = cbrt(w.hidden_layer[i]);
        w.error_hidden[i] *= 3 * hidden_layer * hidden_layer;
      }
      break;
  }

  // Update weights[1]
  for (auto&& i : w.hidden_kept) {
    if (w.weights_batch[1][i].empty()) w.weights_batch[1][i].resize(outcomes_size);
    for (unsigned j = 0; j < outcomes_size; j++)
      w.weights_batch[1][i][j] += w.hidden_layer[i] * w.error_outcomes[j];
  }
  // Bias
  if (w.hidden_dropout.empty() || !w.hidden_dropout[hidden_layer_size]) {
    if (w.weights_batch[1][hidden_layer_size].empty()) w.weights_batch[1][hidden_layer_size].resize(outcomes_size);
    for (unsigned i = 0; i < outcomes_size; i++)
      w.weights_batch[1][hidden_layer_size][i] += w.error_outcomes[i];
  }

  // Update weights[0] and backpropagate to error_embedding
  unsigned index = 0;
  for (auto&& embedding_ids : embedding_ids_sequences)
    for (unsigned i = 0; i < embeddings.size(); i++)
      if (embedding_ids && (*embedding_ids)[i] >= 0) {
        int embedding_id = (*embedding_ids)[i];

        float* error_embedding = nullptr; // Accumulate embedding error if required
        if (embeddings[i].can_update_weights(embedding_id)) {
          if (w.error_embedding[i].size() <= unsigned(embedding_id)) w.error_embedding[i].resize(embedding_id + 1);
          if (w.error_embedding[i][embedding_id].empty()) {
            w.error_embedding[i][embedding_id].assign(embeddings[i].dimension, 0);
            w.error_embedding_nonempty[i].emplace_back(embedding_id);
          }
          error_embedding = w.error_embedding[i][embedding_id].data();
        }

        const float* embedding = embeddings[i].weight(embedding_id);
        for (unsigned dimension = embeddings[i].dimension; dimension; dimension--, index++, embedding++, error_embedding += !!error_embedding)
          if (w.input_dropout.empty() || !w.input_dropout[index]) {
            if (error_embedding)
              for (auto&& j : w.hidden_kept)
                *error_embedding += network.weights[0][index][j] * w.error_hidden[j];
            if (w.weights_batch[0][index].empty()) w.weights_batch[0][index].resize(hidden_layer_size);
            for (auto&& j : w.hidden_kept)
              w.weights_batch[0][index][j] += *embedding * w.error_hidden[j];
          }
      } else {
        index += embeddings[i].dimension;
      }
  // Bias
  if (w.input_dropout.empty() || !w.input_dropout[index]) {
    if (w.weights_batch[0][index].empty()) w.weights_batch[0][index].resize(hidden_layer_size);
    for (auto&& i : w.hidden_kept)
      w.weights_batch[0][index][i] += w.error_hidden[i];
  }

  // End if not at the end of the batch
  if (++w.batch < batch_size) return;
  w.batch = 0;

  // Update hidden weights
  if (!network.weights[0].empty())
    for (int i = 0; i < 2; i++) {
      for (unsigned j = 0; j < w.weights_batch[i].size(); j++)
        if (!w.weights_batch[i][j].empty()) {
          for (unsigned k = 0; k < w.weights_batch[i][j].size(); k++)
            network.weights[i][j][k] += TRAINER::delta(w.weights_batch[i][j][k], trainer, TRAINER::need_trainer_data ? w.weights_trainer[i][j][k] : none_trainer_data) - l2_regularization * network.weights[i][j][k];
          w.weights_batch[i][j].clear();
        }
    }

  // Update embedding weights using error_embedding
  for (unsigned i = 0; i < embeddings.size(); i++) {
    for (auto&& id : w.error_embedding_nonempty[i]) {
      if (TRAINER::need_trainer_data) {
        if (w.embedding_trainer.size() <= i) w.embedding_trainer.resize(i + 1);
        if (w.embedding_trainer[i].size() <= id) w.embedding_trainer[i].resize(id + 1);
        if (w.embedding_trainer[i][id].size() < embeddings[i].dimension) w.embedding_trainer[i][id].resize(embeddings[i].dimension);
      }
      float* embedding = embeddings[i].weight(id);
      for (unsigned j = 0; j < embeddings[i].dimension; j++)
        embedding[j] += TRAINER::delta(w.error_embedding[i][id][j], trainer, TRAINER::need_trainer_data ? w.embedding_trainer[i][id][j] : none_trainer_data) - l2_regularization * embedding[j];
      w.error_embedding[i][id].clear();
    }
    w.error_embedding_nonempty[i].clear();
  }

  // Maxnorm regularize the updated weights
  if (maxnorm_regularization) maxnorm_regularize();
}


void neural_network_trainer::backpropagate(vector<embedding>& embeddings, const vector<const vector<int>*>& embedding_ids_sequences, unsigned required_outcome, workspace& w) {
  switch (trainer.algorithm) {
    case network_trainer::SGD:
      backpropagate_template<trainer_sgd>(embeddings, embedding_ids_sequences, required_outcome, w);
      return;
    case network_trainer::SGD_MOMENTUM:
      backpropagate_template<trainer_sgd_momentum>(embeddings, embedding_ids_sequences, required_outcome, w);
      return;
    case network_trainer::ADAGRAD:
      backpropagate_template<trainer_adagrad>(embeddings, embedding_ids_sequences, required_outcome, w);
      return;
    case network_trainer::ADADELTA:
      backpropagate_template<trainer_adadelta>(embeddings, embedding_ids_sequences, required_outcome, w);
      return;
  }

  training_failure("Internal error, unsupported trainer!");
}

void neural_network_trainer::l1_regularize() {
  if (!l1_regularization) return;

  for (auto&& weights : network.weights)
    for (auto&& row : weights)
      for (auto&& weight : row)
        if (weight < l1_regularization) weight += l1_regularization;
        else if (weight > l1_regularization) weight -= l1_regularization;
        else weight = 0;
}

void neural_network_trainer::maxnorm_regularize() {
  if (!maxnorm_regularization) return;

  for (unsigned i = 0; i < 2; i++)
    for (unsigned j = 0; j < network.weights[i].front().size(); j++) {
      float length = 0;
      for (auto&& row : network.weights[i])
        length += row[j] * row[j];

      if (length > 0 && length > maxnorm_regularization * maxnorm_regularization) {
        float factor = 1 / sqrt(length / (maxnorm_regularization * maxnorm_regularization));
        for (auto&& row : network.weights[i])
          row[j] *= factor;
      }
    }
}

void neural_network_trainer::finalize_sentence() {
  if (l1_regularization) l1_regularize();
}

void neural_network_trainer::finalize_dropout_weights(bool finalize) {
  if (dropout_input) {
    bool factor = finalize ? dropout_input : 1 / dropout_input;
    for (auto&& row : network.weights[0])
      for (auto&& weight : row)
        weight *= factor;
  }

  if (dropout_hidden) {
    bool factor = finalize ? dropout_hidden : 1 / dropout_hidden;
    for (auto&& row : network.weights[1])
      for (auto&& weight : row)
        weight *= factor;
  }
}

void neural_network_trainer::save_matrix(const vector<vector<float>>& m, binary_encoder& enc) const {
  enc.add_4B(m.size());
  enc.add_4B(m.empty() ? 0 : m.front().size());

  for (auto&& row : m) {
    assert(row.size() == m.front().size());
    enc.add_data(row);
  }
}

void neural_network_trainer::save_network(binary_encoder& enc) const {
  enc.add_1B(network.hidden_layer_activation);
  save_matrix(network.weights[0], enc);
  save_matrix(network.weights[1], enc);
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
