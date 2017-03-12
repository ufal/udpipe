// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <list>

#include "common.h"
#include "parsito/configuration/node_extractor.h"
#include "parsito/configuration/value_extractor.h"
#include "parsito/embedding/embedding.h"
#include "parsito/network/neural_network.h"
#include "parser.h"
#include "parsito/transition/transition_system.h"
#include "utils/threadsafe_stack.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class parser_nn : public parser {
 public:
  parser_nn(bool versioned);

  virtual void parse(tree& t, unsigned beam_size = 0) const override;

 protected:
  virtual void load(binary_decoder& data, unsigned cache) override;

 private:
  friend class parser_nn_trainer;
  void parse_greedy(tree& t) const;
  void parse_beam_search(tree& t, unsigned beam_size) const;

  bool versioned;
  unsigned version;
  bool single_root;
  enum { VERSION_LATEST = 2 };

  vector<string> labels;
  unique_ptr<transition_system> system;

  node_extractor nodes;

  vector<value_extractor> values;
  vector<embedding> embeddings;

  neural_network network;
  neural_network::embeddings_cache embeddings_cache;

  struct workspace {
    workspace(bool single_root) : conf(single_root) {}

    configuration conf;

    string word, word_buffer;
    vector<vector<int>> embeddings;
    vector<vector<string>> embeddings_values;

    vector<int> extracted_nodes;
    vector<const vector<int>*> extracted_embeddings;

    vector<float> outcomes, network_buffer;

    // Beam-size structures
    struct beam_size_configuration {
      beam_size_configuration(bool single_root) : conf(single_root) {}

      configuration conf;
      vector<int> heads;
      vector<string> deprels;
      double cost;

      void refresh_tree();
      void save_tree();
    };
    struct beam_size_alternative {
      const beam_size_configuration* bs_conf;
      int transition;
      double cost;
      bool operator<(const beam_size_alternative& other) const { return cost > other.cost; }

      beam_size_alternative(const beam_size_configuration* bs_conf, int transition, double cost)
          : bs_conf(bs_conf), transition(transition), cost(cost) {}
    };
    vector<beam_size_configuration> bs_confs[2]; size_t bs_confs_size[2];
    vector<beam_size_alternative> bs_alternatives;
  };
  mutable threadsafe_stack<workspace> workspaces;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
