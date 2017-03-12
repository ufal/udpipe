// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <cmath>

#include "parser_nn.h"

namespace ufal {
namespace udpipe {
namespace parsito {

// Versions:
// 1: initial version
// 2: add ReLU activation function

parser_nn::parser_nn(bool versioned) : versioned(versioned) {}

void parser_nn::parse(tree& t, unsigned beam_size) const {
  if (beam_size > 1)
    parse_beam_search(t, beam_size);
  else
    parse_greedy(t);
}

void parser_nn::parse_greedy(tree& t) const {
  assert(system);

  // Retrieve or create workspace
  workspace* w = workspaces.pop();
  if (!w) w = new workspace(single_root);

  // Create configuration
  w->conf.init(&t);

  // Compute embeddings of all nodes
  if (w->embeddings.size() < t.nodes.size()) w->embeddings.resize(t.nodes.size());
  for (size_t i = 0; i < t.nodes.size(); i++) {
    if (w->embeddings[i].size() < embeddings.size()) w->embeddings[i].resize(embeddings.size());
    for (size_t j = 0; j < embeddings.size(); j++) {
      values[j].extract(t.nodes[i], w->word);
      w->embeddings[i][j] = embeddings[j].lookup_word(w->word, w->word_buffer);
    }
  }

  // Compute which transitions to perform and perform them
  while (!w->conf.final()) {
    // Extract nodes from the configuration
    nodes.extract(w->conf, w->extracted_nodes);
    w->extracted_embeddings.resize(w->extracted_nodes.size());
    for (size_t i = 0; i < w->extracted_nodes.size(); i++)
      w->extracted_embeddings[i] = w->extracted_nodes[i] >= 0 ? &w->embeddings[w->extracted_nodes[i]] : nullptr;

    // Classify using neural network
    network.propagate(embeddings, w->extracted_embeddings, w->network_buffer, w->outcomes, &embeddings_cache, false);

    // Find most probable applicable transition
    int best = -1;
    for (unsigned i = 0; i < w->outcomes.size(); i++)
      if (system->applicable(w->conf, i) && (best < 0 || w->outcomes[i] > w->outcomes[best]))
        best = i;

    // Perform the best transition
    int child = system->perform(w->conf, best);

    // If a node was linked, recompute its embeddings as deprel has changed
    if (child >= 0)
      for (size_t i = 0; i < embeddings.size(); i++) {
        values[i].extract(t.nodes[child], w->word);
        w->embeddings[child][i] = embeddings[i].lookup_word(w->word, w->word_buffer);
      }
  }

  // Store workspace
  workspaces.push(w);
}

void parser_nn::parse_beam_search(tree& t, unsigned beam_size) const {
  assert(system);

  // Retrieve or create workspace
  workspace* w = workspaces.pop();
  if (!w) w = new workspace(single_root);

  // Allocate and initialize configuration
  for (int i = 0; i < 2; i++) {
    while (w->bs_confs[i].size() < beam_size) w->bs_confs[i].emplace_back(single_root);
    while (w->bs_confs[i].size() > beam_size) w->bs_confs[i].pop_back();
    w->bs_confs_size[i] = 0;
  }
  w->bs_confs[0][0].cost = 0;
  w->bs_confs[0][0].conf.init(&t);
  w->bs_confs[0][0].save_tree();
  w->bs_confs_size[0] = 1;

  // Compute embeddings of all nodes
  if (w->embeddings.size() < t.nodes.size()) w->embeddings.resize(t.nodes.size());
  if (w->embeddings_values.size() < t.nodes.size()) w->embeddings_values.resize(t.nodes.size());
  for (size_t i = 0; i < t.nodes.size(); i++) {
    if (w->embeddings[i].size() < embeddings.size()) w->embeddings[i].resize(embeddings.size());
    if (w->embeddings_values[i].size() < embeddings.size()) w->embeddings_values[i].resize(embeddings.size());
    for (size_t j = 0; j < embeddings.size(); j++) {
      values[j].extract(t.nodes[i], w->embeddings_values[i][j]);
      w->embeddings[i][j] = embeddings[j].lookup_word(w->embeddings_values[i][j], w->word_buffer);
    }
  }

  // Compute which transitions to perform and perform them
  size_t iteration = 0;
  for (bool all_final = false; !all_final; iteration++) {
    all_final = true;
    w->bs_alternatives.clear();

    for (size_t c = 0; c < w->bs_confs_size[iteration & 1]; c++) {
      auto& bs_conf = w->bs_confs[iteration & 1][c];

      if (bs_conf.conf.final()) {
        if (w->bs_alternatives.size() == beam_size) {
          if (bs_conf.cost <= w->bs_alternatives[0].cost) continue;
          pop_heap(w->bs_alternatives.begin(), w->bs_alternatives.end());
          w->bs_alternatives.pop_back();
        }
        w->bs_alternatives.emplace_back(&bs_conf, -1, bs_conf.cost);
        push_heap(w->bs_alternatives.begin(), w->bs_alternatives.end());
        continue;
      }
      all_final = false;

      bs_conf.refresh_tree();
      // Update embeddings for all nodes
      for (size_t i = 0; i < t.nodes.size(); i++)
        for (size_t j = 0; j < embeddings.size(); j++) {
          values[j].extract(t.nodes[i], w->word);
          if (w->word != w->embeddings_values[i][j]) {
            w->embeddings[i][j] = embeddings[j].lookup_word(w->word, w->word_buffer);
            w->embeddings_values[i][j].assign(w->word);
          }
        }

      // Extract nodes from the configuration
      nodes.extract(bs_conf.conf, w->extracted_nodes);
      w->extracted_embeddings.resize(w->extracted_nodes.size());
      for (size_t i = 0; i < w->extracted_nodes.size(); i++)
        w->extracted_embeddings[i] = w->extracted_nodes[i] >= 0 ? &w->embeddings[w->extracted_nodes[i]] : nullptr;

      // Classify using neural network
      network.propagate(embeddings, w->extracted_embeddings, w->network_buffer, w->outcomes, &embeddings_cache);

      // Store all alternatives
      for (unsigned i = 0; i < w->outcomes.size(); i++)
        if (system->applicable(bs_conf.conf, i)) {
          double cost = (bs_conf.cost * iteration + log(w->outcomes[i])) / (iteration + 1);
          if (w->bs_alternatives.size() == beam_size) {
            if (cost <= w->bs_alternatives[0].cost) continue;
            pop_heap(w->bs_alternatives.begin(), w->bs_alternatives.end());
            w->bs_alternatives.pop_back();
          }
          w->bs_alternatives.emplace_back(&bs_conf, i, cost);
          push_heap(w->bs_alternatives.begin(), w->bs_alternatives.end());
        }
    }

    w->bs_confs_size[(iteration + 1) & 1] = 0;
    for (auto&& alternative : w->bs_alternatives) {
      auto& bs_conf_new = w->bs_confs[(iteration + 1) & 1][w->bs_confs_size[(iteration + 1) & 1]++];
      bs_conf_new = *alternative.bs_conf;
      bs_conf_new.cost = alternative.cost;
      if (alternative.transition >= 0) {
        bs_conf_new.refresh_tree();
        system->perform(bs_conf_new.conf, alternative.transition);
        bs_conf_new.save_tree();
      }
    }
  }

  // Return the best tree
  size_t best = 0;
  for (size_t i = 1; i < w->bs_confs_size[iteration & 1]; i++)
    if (w->bs_confs[iteration & 1][i].cost > w->bs_confs[iteration & 1][best].cost)
      best = i;
  w->bs_confs[iteration & 1][best].refresh_tree();

  // Store workspace
  workspaces.push(w);
}

void parser_nn::workspace::beam_size_configuration::refresh_tree() {
  for (auto&& node : conf.t->nodes) node.children.clear();
  for (size_t i = 0; i < conf.t->nodes.size(); i++) {
    conf.t->nodes[i].head = heads[i];
    conf.t->nodes[i].deprel = deprels[i];
    if (heads[i] >= 0) conf.t->nodes[heads[i]].children.push_back(i);
  }
}

void parser_nn::workspace::beam_size_configuration::save_tree() {
  if (conf.t->nodes.size() > heads.size()) heads.resize(conf.t->nodes.size());
  if (conf.t->nodes.size() > deprels.size()) deprels.resize(conf.t->nodes.size());
  for (size_t i = 0; i < conf.t->nodes.size(); i++) {
    heads[i] = conf.t->nodes[i].head;
    deprels[i] = conf.t->nodes[i].deprel;
  }
}

void parser_nn::load(binary_decoder& data, unsigned cache) {
  string description, error;

  version = versioned ? data.next_1B() : 1;
  if (!(version >= 1 && version <= VERSION_LATEST))
    throw binary_decoder_error("Unrecognized version of the parser_nn model");

  single_root = version >= 2 ? data.next_1B() : false;

  // Load labels
  labels.resize(data.next_2B());
  for (auto&& label : labels)
    data.next_str(label);

  // Load transition system
  string system_name;
  data.next_str(system_name);
  system.reset(transition_system::create(system_name, labels));
  if (!system) throw binary_decoder_error("Cannot load transition system");

  // Load node extractor
  data.next_str(description);
  if (!nodes.create(description, error))
    throw binary_decoder_error(error.c_str());

  // Load value extractors and embeddings
  values.resize(data.next_2B());
  for (auto&& value : values) {
    data.next_str(description);
    if (!value.create(description, error))
      throw binary_decoder_error(error.c_str());
  }

  embeddings.resize(values.size());
  for (auto&& embedding : embeddings)
    embedding.load(data);

  // Load the network
  network.load(data);
  network.generate_tanh_cache();
  network.generate_embeddings_cache(embeddings, embeddings_cache, cache);
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
