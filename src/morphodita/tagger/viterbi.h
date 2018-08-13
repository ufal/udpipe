// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "elementary_features.h"
#include "feature_sequences.h"
#include "morphodita/morpho/morpho.h"
#include "tagger.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class FeatureSequences>
class viterbi {
 public:
  viterbi(const FeatureSequences& features, int decoding_order, int window_size)
      : features(features), decoding_order(decoding_order), window_size(window_size) {}

  struct cache;
  void tag(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, cache& c, vector<int>& tags) const;

 private:
  struct node;

  const FeatureSequences& features;
  int decoding_order, window_size;
};


// Definitions
template <class FeatureSequences>
struct viterbi<FeatureSequences>::cache {
  vector<node> nodes;
  typename FeatureSequences::cache features_cache;

  cache(const viterbi<FeatureSequences>& self) : features_cache(self.features) {}
};

template <class FeatureSequences>
struct viterbi<FeatureSequences>::node {
  int tag;
  int prev;
  feature_sequences_score score;
  typename FeatureSequences::dynamic_features dynamic;
};

template <class FeatureSequences>
void viterbi<FeatureSequences>::tag(const vector<string_piece>& forms,
                                    const vector<vector<tagged_lemma>>& analyses,
                                    cache& c, vector<int>& tags) const {
  if (!forms.size()) return;

  // Count number of nodes and allocate
  unsigned nodes = 0;
  for (unsigned i = 0, states = 1; i < forms.size(); i++) {
    if (analyses[i].empty()) return;
    states = (i+1 >= unsigned(decoding_order) ? states / analyses[i-decoding_order+1].size() : states) * analyses[i].size();
    nodes += states;
  }
  if (nodes > c.nodes.size()) c.nodes.resize(nodes);

  // Init feature sequences
  features.initialize_sentence(forms, analyses, c.features_cache);

  int window_stack[16]; vector<int> window_heap;
  int* window = window_size <= 16 ? window_stack : (window_heap.resize(window_size), window_heap.data());
  typename FeatureSequences::dynamic_features dynamic;
  feature_sequences_score score;

  // Compute all nodes score
  int nodes_prev = -1, nodes_now = 0;
  for (unsigned i = 0; i < forms.size(); i++) {
    int nodes_next = nodes_now;

    for (int j = 0; j < window_size; j++) window[j] = -1;
    for (int tag = 0; tag < int(analyses[i].size()); tag++)
      for (int prev = nodes_prev; prev < nodes_now; prev++) {
        // Compute predecessors and number of unchanges
        int same_tags = window[0] == tag;
        window[0] = tag;
        for (int p = prev, n = 1; p >= 0 && n < window_size; p = c.nodes[p].prev, n++) {
          same_tags += same_tags == n && window[n] == c.nodes[p].tag;
          window[n] = c.nodes[p].tag;
        }

        // Compute dynamic elementary features and score
        features.compute_dynamic_features(i, tag, prev >= 0 ? &c.nodes[prev].dynamic : nullptr, dynamic, c.features_cache);
        score = (nodes_prev + 1 == nodes_now && analyses[i].size() == 1 ?
                0 :
                 features.score(i, window, same_tags, dynamic, c.features_cache)) +
            (prev >= 0 ? c.nodes[prev].score : 0);

        // Update existing node or create a new one
        if (same_tags >= decoding_order-1) {
          if (score <= c.nodes[nodes_next-1].score) continue;
          nodes_next--;
        }
        c.nodes[nodes_next].tag = tag;
        c.nodes[nodes_next].prev = prev;
        c.nodes[nodes_next].score = score;
        c.nodes[nodes_next++].dynamic = dynamic;
      }

    nodes_prev = nodes_now;
    nodes_now = nodes_next;
  }

  // Choose the best ending node
  int best = nodes_prev;
  for (int node = nodes_prev + 1; node < nodes_now; node++)
    if (c.nodes[node].score > c.nodes[best].score)
      best = node;

  for (int i = forms.size() - 1; i >= 0; i--, best = c.nodes[best].prev)
    tags[i] = c.nodes[best].tag;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
