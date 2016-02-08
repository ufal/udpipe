// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <atomic>
#include <fstream>
#include <limits>
#include <random>
#include <thread>
#include <unordered_set>

#include "parsito/network/neural_network_trainer.h"
#include "parser_nn.h"
#include "parser_nn_trainer.h"
#include "utils/parse_double.h"
#include "utils/parse_int.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {
namespace parsito {

void parser_nn_trainer::train(const string& transition_system_name, const string& transition_oracle_name,
                              const string& embeddings_description, const string& nodes_description, const network_parameters& parameters,
                              unsigned number_of_threads, const vector<tree>& train, const vector<tree>& heldout, binary_encoder& enc) {
  if (train.empty()) runtime_failure("No training data was given!");

  // Random generator with fixed seed for reproducibility
  mt19937 generator(42);

  // Check that all non-root nodes have heads and nonempty deprel
  for (auto&& tree : train)
    for (auto&& node : tree.nodes)
      if (node.id) {
        if (node.head < 0) runtime_failure("The node '" << node.form << "' with id " << node.id << " has no head set!");
        if (node.deprel.empty()) runtime_failure("The node '" << node.form << "' with id " << node.id << " has no deprel set!");
      }

  // Generate labels for transition system
  parser_nn parser;
  unordered_set<string> labels_set;

  for (auto&& tree : train)
    for (auto&& node : tree.nodes)
      if (node.id && !labels_set.count(node.deprel)) {
        labels_set.insert(node.deprel);
        parser.labels.push_back(node.deprel);
      }

  // Create transition system and transition oracle
  parser.system.reset(transition_system::create(transition_system_name, parser.labels));
  if (!parser.system) runtime_failure("Cannot create transition system '" << transition_system_name << "'!");

  unique_ptr<transition_oracle> oracle(parser.system->oracle(transition_oracle_name));
  if (!oracle) runtime_failure("Cannot create transition oracle '" << transition_oracle_name << "' for transition system '" << transition_system_name << "'!");

  // Create node_extractor
  string error;
  if (!parser.nodes.create(nodes_description, error)) runtime_failure(error);

  // Load value_extractors and embeddings
  vector<string> value_names;
  vector<string_piece> lines, tokens;
  split(embeddings_description, '\n', lines);
  for (auto&& line : lines) {
    // Ignore empty lines and comments
    if (!line.len || line.str[0] == '#') continue;

    split(line, ' ', tokens);
    if (!(tokens.size() >= 3 && tokens.size() <= 6))
      runtime_failure("Expected 3 to 6 columns on embedding description line '" << line << "'!");

    value_names.emplace_back(string(tokens[0].str, tokens[0].len));
    parser.values.emplace_back();
    if (!parser.values.back().create(tokens[0], error)) runtime_failure(error);

    int dimension = parse_int(tokens[1], "embedding dimension");
    int min_count = parse_int(tokens[2], "minimum frequency count");
    unsigned updatable_index = 0;
    unsigned embeddings_from_file = 0;
    string embeddings_from_file_comment;
    vector<pair<string, vector<float>>> weights;
    unordered_set<string> weights_set;

    // Compute words and counts present in the training data
    string word;
    unordered_map<string, int> word_counts;
    for (auto&& tree : train)
      for (auto&& node : tree.nodes)
        if (node.id) {
          parser.values.back().extract(node, word);
          word_counts[word]++;
        }

    // Load embedding if it was given
    if (tokens.size() >= 4) {
      int update_weights = tokens.size() >= 5 ? parse_int(tokens[4], "update weights") : 1;
      int max_embeddings = tokens.size() >= 6 ? parse_int(tokens[5], "maximum embeddings count") : numeric_limits<int>::max();
      ifstream in(string(tokens[3].str, tokens[3].len));
      if (!in.is_open()) runtime_failure("Cannot load '" << tokens[0] << "' embedding from file '" << tokens[3] << "'!");

      // Load first line containing dictionary size and dimensions
      string line;
      vector<string_piece> parts;
      if (!getline(in, line)) runtime_failure("Cannot read first line from embedding file '" << tokens[3] << "'!");
      split(line, ' ', parts);
      if (parts.size() != 2) runtime_failure("Expected two numbers on the first line of embedding file '" << tokens[3] << "'!");
      int file_dimension = parse_int(parts[1], "embedding file dimension");

      if (file_dimension < dimension) runtime_failure("The embedding file '" << tokens[3] << "' has lower dimension than required!");

      // Generate random projection when smaller dimension is required
      vector<vector<float>> projection;
      if (file_dimension > dimension) {
        embeddings_from_file_comment = "[dim" + to_string(file_dimension) + "->" + to_string(dimension) + "]";

        uniform_real_distribution<double> uniform(0, 1);
        projection.resize(dimension);
        for (auto&& row : projection) {
          row.resize(file_dimension);
          for (auto&& weight : row) weight = uniform(generator);

          double sum = 0;
          for (auto&& weight : row) sum += weight;
          for (auto&& weight : row) weight /= sum;
        }
      }

      // Load input embedding
      vector<double> input_weights(file_dimension);
      vector<float> projected_weights(dimension);
      while (getline(in, line) && int(weights.size()) < max_embeddings) {
        split(line, ' ', parts);
        if (!parts.empty() && !parts.back().len) parts.pop_back(); // Ignore space at the end of line
        if (int(parts.size()) != file_dimension + 1) runtime_failure("Wrong number of values on line '" << line << "' of embedding file '" << tokens[3]);
        for (int i = 0; i < file_dimension; i++)
          input_weights[i] = parse_double(parts[1 + i], "embedding weight");

        string word(parts[0].str, parts[0].len);

        // For update_weights == 2, ignore embeddings for unknown words
        if (update_weights == 2 && !word_counts.count(word))
          continue;

        for (int i = 0; i < dimension; i++)
          if (file_dimension == dimension) {
            projected_weights[i] = input_weights[i];
          } else {
            projected_weights[i] = 0;
            for (int j = 0; j < file_dimension; j++)
              projected_weights[i] += projection[i][j] * input_weights[j];
          }

        if (!weights_set.count(word)) {
          weights.emplace_back(word, projected_weights);
          weights_set.insert(word);
        }
      }
      embeddings_from_file = weights.size();
      updatable_index = update_weights ? 0 : embeddings_from_file;
    }

    // Add embedding for non-present word with min_count, sorted by count
    {
      vector<pair<int, string>> count_words;
      for (auto&& word_count : word_counts)
        if (word_count.second >= min_count && !weights_set.count(word_count.first))
          count_words.emplace_back(word_count.second, word_count.first);

      sort(count_words.rbegin(), count_words.rend());

      vector<float> word_weights(dimension);
      uniform_real_distribution<float> uniform(-1, 1);
      for (auto&& count_word : count_words) {
        for (auto&& word_weight : word_weights)
          word_weight = uniform(generator);

        weights.emplace_back(count_word.second, word_weights);
      }
    }

    // If there are unknown words in the training data, create initial embedding
    vector<float> unknown_weights(dimension);
    if (min_count > 1) {
      uniform_real_distribution<float> uniform(-1, 1);

      for (auto&& weight : unknown_weights)
        weight = uniform(generator);
    }

    // Add the embedding
    parser.embeddings.emplace_back();
    parser.embeddings.back().create(dimension, updatable_index, weights, unknown_weights);

    // Count the cover of this embedding
    string buffer;
    unsigned words_total = 0, words_covered = 0, words_covered_from_file = 0;
    for (auto&& tree : train)
      for (auto&& node : tree.nodes)
        if (node.id) {
          parser.values.back().extract(node, word);
          words_total++;
          int word_id = parser.embeddings.back().lookup_word(word, buffer);
          words_covered += word_id != parser.embeddings.back().unknown_word();
          words_covered_from_file += word_id != parser.embeddings.back().unknown_word() && unsigned(word_id) < embeddings_from_file;
        }

    cerr << "Initialized '" << tokens[0] << "' embedding with " << embeddings_from_file << embeddings_from_file_comment
         << "," << weights.size() << " words and " << fixed << setprecision(1) << 100. * words_covered_from_file / words_total
         << "%," << 100. * words_covered / words_total << "% coverage." << endl;
  }

  // Train the network
  unsigned total_dimension = 0, total_nodes = 0;
  for (auto&& embedding : parser.embeddings) total_dimension += embedding.dimension;
  for (auto&& tree : train) total_nodes += tree.nodes.size() - 1;
  auto scaled_parameters = parameters;
  scaled_parameters.l1_regularization /= train.size();
  scaled_parameters.l2_regularization /= total_nodes;
  neural_network_trainer network_trainer(parser.network, total_dimension * parser.nodes.node_count(), parser.system->transition_count(), scaled_parameters, generator);

  vector<int> permutation;
  for (size_t i = 0; i < train.size(); i++)
    permutation.push_back(permutation.size());

  for (int iteration = 1; network_trainer.next_iteration(); iteration++) {
    // Train on training data
    shuffle(permutation.begin(), permutation.end(), generator);

    atomic<unsigned> atomic_index(0);
    atomic<double> atomic_logprob(0);
    auto training = [&]() {
      tree t;
      configuration conf;
      string word, word_buffer;
      vector<vector<int>> nodes_embeddings;
      vector<int> extracted_nodes;
      vector<const vector<int>*> extracted_embeddings;
      neural_network_trainer::workspace workspace;
      double logprob = 0;

      // Data for structured prediction
      tree t_eval;
      configuration conf_eval;
      vector<vector<int>> nodes_embeddings_eval;
      vector<int>  extracted_nodes_eval;
      vector<const vector<int>*>  extracted_embeddings_eval;
      vector<unsigned> transitions_eval;
      vector<float> hidden_layer_eval, outcomes_eval;

      for (unsigned current_index; (current_index = atomic_index++) < permutation.size();) {
        const tree& gold = train[permutation[current_index]];
        t = gold;
        t.unlink_all_nodes();
        conf.init(&t);

        // Compute embeddings
        if (t.nodes.size() > nodes_embeddings.size()) nodes_embeddings.resize(t.nodes.size());
        for (size_t i = 0; i < t.nodes.size(); i++) {
          nodes_embeddings[i].resize(parser.embeddings.size());
          for (size_t j = 0; j < parser.embeddings.size(); j++) {
            parser.values[j].extract(t.nodes[i], word);
            nodes_embeddings[i][j] = parser.embeddings[j].lookup_word(word, word_buffer);
          }
        }

        // Create tree oracle
        auto tree_oracle = oracle->create_tree_oracle(gold);

        // Train the network
        while (!conf.final()) {
          // Extract nodes
          parser.nodes.extract(conf, extracted_nodes);
          extracted_embeddings.resize(extracted_nodes.size());
          for (size_t i = 0; i < extracted_nodes.size(); i++)
            extracted_embeddings[i] = extracted_nodes[i] >= 0 ? &nodes_embeddings[extracted_nodes[i]] : nullptr;

          // Propagate
          network_trainer.propagate(parser.embeddings, extracted_embeddings, workspace);

          // Find most probable applicable transition
          int network_best = -1;
          for (unsigned i = 0; i < workspace.outcomes.size(); i++)
            if (parser.system->applicable(conf, i) && (network_best < 0 || workspace.outcomes[i] > workspace.outcomes[network_best]))
              network_best = i;

          // Apply the oracle
          auto prediction = tree_oracle->predict(conf, network_best, iteration);

          // If the best transition is applicable, train on it
          if (parser.system->applicable(conf, prediction.best)) {
            // Update logprob
            if (workspace.outcomes[prediction.best])
              logprob += log(workspace.outcomes[prediction.best]);

            // Backpropagate the chosen outcome
            network_trainer.backpropagate(parser.embeddings, extracted_embeddings, prediction.best, workspace);
          }

          // Emergency break if the to_follow transition is not applicable
          if (!parser.system->applicable(conf, prediction.to_follow))
            break;

          // Follow the chosen outcome
          int child = parser.system->perform(conf, prediction.to_follow);

          // If a node was linked, recompute its embeddings as deprel has changed
          if (child >= 0)
            for (size_t i = 0; i < parser.embeddings.size(); i++) {
              parser.values[i].extract(t.nodes[child], word);
              nodes_embeddings[child][i] = parser.embeddings[i].lookup_word(word, word_buffer);
            }
        }
        network_trainer.finalize_sentence();

        // Structured prediction
        if (parameters.structured_interval && (current_index % parameters.structured_interval) == 0) {
          uniform_int_distribution<size_t> train_distribution(0, train.size() - 1);
          const tree& gold = train[train_distribution(generator)];
          t = gold;
          t.unlink_all_nodes();
          conf.init(&t);

          // Compute embeddings
          if (t.nodes.size() > nodes_embeddings.size()) nodes_embeddings.resize(t.nodes.size());
          for (size_t i = 0; i < t.nodes.size(); i++) {
            nodes_embeddings[i].resize(parser.embeddings.size());
            for (size_t j = 0; j < parser.embeddings.size(); j++) {
              parser.values[j].extract(t.nodes[i], word);
              nodes_embeddings[i][j] = parser.embeddings[j].lookup_word(word, word_buffer);
            }
          }

          // Create tree oracle
          auto tree_oracle = oracle->create_tree_oracle(gold);

          // Train the network
          while (!conf.final()) {
            // Extract nodes
            parser.nodes.extract(conf, extracted_nodes);
            extracted_embeddings.resize(extracted_nodes.size());
            for (size_t i = 0; i < extracted_nodes.size(); i++)
              extracted_embeddings[i] = extracted_nodes[i] >= 0 ? &nodes_embeddings[extracted_nodes[i]] : nullptr;

            // Find the best transition
            int best = 0;
            int best_uas = -1;
            tree_oracle->interesting_transitions(conf, transitions_eval);
            for (auto&& transition : transitions_eval) {
              t_eval = t;
              conf_eval = conf;
              conf_eval.t = &t_eval;
              nodes_embeddings_eval = nodes_embeddings;

              // Perform probed transition
              int child = parser.system->perform(conf_eval, transition);
              if (child >= 0)
                for (size_t i = 0; i < parser.embeddings.size(); i++) {
                  parser.values[i].extract(t_eval.nodes[child], word);
                  nodes_embeddings_eval[child][i] = parser.embeddings[i].lookup_word(word, word_buffer);
                }

              // Train the network
              while (!conf_eval.final()) {
                // Extract nodes
                parser.nodes.extract(conf_eval, extracted_nodes_eval);
                extracted_embeddings_eval.resize(extracted_nodes_eval.size());
                for (size_t i = 0; i < extracted_nodes_eval.size(); i++)
                  extracted_embeddings_eval[i] = extracted_nodes_eval[i] >= 0 ? &nodes_embeddings_eval[extracted_nodes_eval[i]] : nullptr;

                // Classify using neural network
                parser.network.propagate(parser.embeddings, extracted_embeddings_eval, hidden_layer_eval, outcomes_eval, nullptr, false);

                // Find most probable applicable transition
                int network_best = -1;
                for (unsigned i = 0; i < outcomes_eval.size(); i++)
                  if (parser.system->applicable(conf_eval, i) && (network_best < 0 || outcomes_eval[i] > outcomes_eval[network_best]))
                    network_best = i;

                // Perform the best transition
                int child = parser.system->perform(conf_eval, network_best);

                // If a node was linked, recompute its embeddings as deprel has changed
                if (child >= 0)
                  for (size_t i = 0; i < parser.embeddings.size(); i++) {
                    parser.values[i].extract(t_eval.nodes[child], word);
                    nodes_embeddings_eval[child][i] = parser.embeddings[i].lookup_word(word, word_buffer);
                  }
              }

              int uas = 0;
              for (unsigned i = 1; i < gold.nodes.size(); i++)
                uas += gold.nodes[i].head == t_eval.nodes[i].head;

              if (uas > best_uas) best = transition, best_uas = uas;
            }

            // Propagate
            network_trainer.propagate(parser.embeddings, extracted_embeddings, workspace);

            // Backpropagate for the best transition
            if (workspace.outcomes[best])
              logprob += log(workspace.outcomes[best]);
            network_trainer.backpropagate(parser.embeddings, extracted_embeddings, best, workspace);

            //              // Find most probable applicable transition when following network outcome
            //              int network_best = -1;
            //              for (unsigned i = 0; i < workspace.outcomes.size(); i++)
            //                if (parser.system->applicable(conf, i) && (network_best < 0 || workspace.outcomes[i] > workspace.outcomes[network_best]))
            //                  network_best = i;

            // Follow the best outcome
            int child = parser.system->perform(conf, /*network_*/best);

            // If a node was linked, recompute its embeddings as deprel has changed
            if (child >= 0)
              for (size_t i = 0; i < parser.embeddings.size(); i++) {
                parser.values[i].extract(t.nodes[child], word);
                nodes_embeddings[child][i] = parser.embeddings[i].lookup_word(word, word_buffer);
              }
          }
          network_trainer.finalize_sentence();
        }
      }
      for (double old_atomic_logprob = atomic_logprob; atomic_logprob.compare_exchange_weak(old_atomic_logprob, old_atomic_logprob + logprob); ) {}
    };

    cerr << "Iteration " << iteration << ": ";
    if (number_of_threads > 1) {
      vector<thread> threads;
      for (unsigned i = 0; i < number_of_threads; i++) threads.emplace_back(training);
      for (; !threads.empty(); threads.pop_back()) threads.back().join();
    } else {
      training();
    }
    cerr << "training logprob " << scientific << setprecision(4) << atomic_logprob;

    // Evaluate heldout data if present
    if (!heldout.empty()) {
      network_trainer.finalize_dropout_weights();

      tree t;
      unsigned total = 0, correct_unlabelled = 0, correct_labelled = 0;
      for (auto&& gold : heldout) {
        t = gold;
        t.unlink_all_nodes();
        parser.parse(t);
        for (size_t i = 1; i < t.nodes.size(); i++) {
          total++;
          correct_unlabelled += t.nodes[i].head == gold.nodes[i].head;
          correct_labelled += t.nodes[i].head == gold.nodes[i].head && t.nodes[i].deprel == gold.nodes[i].deprel;
        }
      }

      network_trainer.finalize_dropout_weights(false);

      cerr << ", heldout UAS " << fixed << setprecision(2) << (100. * correct_unlabelled / total) << "%, LAS " << (100. * correct_labelled / total) << "%";
    }

    cerr << endl;
  }

  // Finalize weights for dropout
  network_trainer.finalize_dropout_weights();

  // Encode transition system
  enc.add_2B(parser.labels.size());
  for (auto&& label : parser.labels)
    enc.add_str(label);
  enc.add_str(transition_system_name);

  // Encode nodes selector
  enc.add_str(nodes_description);

  // Encode value extractors and embeddings
  enc.add_2B(value_names.size());
  for (auto&& value_name : value_names)
    enc.add_str(value_name);
  for (auto&& embedding : parser.embeddings)
    embedding.save(enc);

  // Encode the network
  network_trainer.save_network(enc);
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
