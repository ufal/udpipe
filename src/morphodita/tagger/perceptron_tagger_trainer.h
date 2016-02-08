// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "elementary_features_encoder.h"
#include "feature_sequences_encoder.h"
#include "feature_sequences_optimizer.h"
#include "morphodita/morpho/morpho.h"
#include "tagger_trainer.h"
#include "training_maps.h"
#include "viterbi.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class FeatureSequences>
class perceptron_tagger_trainer {
 public:
  typedef typename tagger_trainer<perceptron_tagger_trainer<FeatureSequences>>::sentence sentence;

  static void train(int decoding_order, int window_size, int iterations, const vector<sentence>& train, const vector<sentence>& heldout, bool early_stopping, bool prune_features, istream& in_feature_templates, ostream& out_tagger);

 private:
  static void train_viterbi(int decoding_order, int window_size, int iterations, const vector<sentence>& train, const vector<sentence>& heldout, bool early_stopping, bool prune_features, FeatureSequences& features);
};


// Definitions
template <class FeatureSequences>
void perceptron_tagger_trainer<FeatureSequences>::train(int decoding_order, int window_size, int iterations, const vector<sentence>& train, const vector<sentence>& heldout, bool early_stopping, bool prune_features, istream& in_feature_templates, ostream& out_tagger) {
  FeatureSequences features;

  cerr << "Parsing feature templates..." << endl;
  features.parse(window_size, in_feature_templates);

  cerr << "Training tagger..." << endl;
  train_viterbi(decoding_order, window_size, iterations, train, heldout, early_stopping, prune_features, features);

  cerr << "Encoding tagger..." << endl;
  typedef feature_sequences_optimizer<FeatureSequences> optimizer;
  typename optimizer::optimized_feature_sequences optimized_features;
  optimizer::optimize(features, optimized_features);
  if (!optimized_features.save(out_tagger)) runtime_failure("Cannot save feature sequences!");
}

template <class FeatureSequences>
void perceptron_tagger_trainer<FeatureSequences>::train_viterbi(int decoding_order, int window_size, int iterations, const vector<sentence>& train, const vector<sentence>& heldout, bool early_stopping, bool prune_features, FeatureSequences& features) {
  int best_correct = 0, best_iteration = -1;
  FeatureSequences best_features;

  viterbi<FeatureSequences> decoder(features, decoding_order, window_size);
  typename decltype(decoder)::cache decoder_cache(decoder);

  typename FeatureSequences::cache feature_sequences_cache(features);
  typename FeatureSequences::dynamic_features decoded_dynamic_features, gold_dynamic_features;
  vector<string> decoded_feature_sequences_keys, gold_feature_sequences_keys;

  vector<int> window(window_size);

  // Initialize feature sequences for the gold decoding only if requested
  if (prune_features)
    for (unsigned s = 0; s < train.size(); s++) {
      auto& sentence = train[s];
      features.initialize_sentence(sentence.forms, sentence.analyses, feature_sequences_cache);
      for (int i = 0; i < int(sentence.forms.size()); i++) {
        window.assign(window_size, -1);
        for (int j = 0; j < window_size && i - j >= 0; j++) window[j] = sentence.gold_index[i - j];

        features.compute_dynamic_features(i, window[0], &gold_dynamic_features, gold_dynamic_features, feature_sequences_cache);
        features.feature_keys(i, window.data(), 0, gold_dynamic_features, gold_feature_sequences_keys, feature_sequences_cache);

        for (unsigned f = 0; f < features.scores.size(); f++)
          if (!gold_feature_sequences_keys[f].empty())
            features.scores[f].map[gold_feature_sequences_keys[f]];
      }
    }

  // Train for given number of iterations
  for (int i = 0; i < iterations; i++) {
    // Train
    int train_correct = 0, train_total = 0;
    cerr << "Iteration " << i + 1 << ": ";

    vector<int> tags;
    for (unsigned s = 0; s < train.size(); s++) {
      auto& sentence = train[s];

      // Run Viterbi
      if (tags.size() < sentence.forms.size()) tags.resize(2 * sentence.forms.size());
      decoder.tag(sentence.forms, sentence.analyses, decoder_cache, tags);

      // Compute feature sequence keys or decoded result and gold result and update alpha & gamma
      features.initialize_sentence(sentence.forms, sentence.analyses, feature_sequences_cache);
      for (int i = 0; i < int(sentence.forms.size()); i++) {
        train_correct += tags[i] == sentence.gold_index[i];
        train_total++;

        window.assign(window_size, -1);
        for (int j = 0; j < window_size && i - j >= 0; j++) window[j] = tags[i - j];
        features.compute_dynamic_features(i, window[0], &decoded_dynamic_features, decoded_dynamic_features, feature_sequences_cache);
        features.feature_keys(i, window.data(), 0, decoded_dynamic_features, decoded_feature_sequences_keys, feature_sequences_cache);

        for (int j = 0; j < window_size && i - j >= 0; j++) window[j] = sentence.gold_index[i - j];
        features.compute_dynamic_features(i, window[0], &gold_dynamic_features, gold_dynamic_features, feature_sequences_cache);
        features.feature_keys(i, window.data(), 0, gold_dynamic_features, gold_feature_sequences_keys, feature_sequences_cache);

        for (unsigned f = 0; f < features.scores.size(); f++) {
          if (decoded_feature_sequences_keys[f] != gold_feature_sequences_keys[f]) {
            if (!decoded_feature_sequences_keys[f].empty()) {
              auto it = features.scores[f].map.find(decoded_feature_sequences_keys[f]);
              if (it == features.scores[f].map.end() && !prune_features) it = features.scores[f].map.emplace(decoded_feature_sequences_keys[f], typename decltype(features.scores[f].map)::mapped_type()).first;
              if (it != features.scores[f].map.end()) {
                auto& decoded_info = it->second;
                decoded_info.gamma += decoded_info.alpha * (s - decoded_info.last_gamma_update);
                decoded_info.last_gamma_update = s;
                decoded_info.alpha--;
              }
            }

            if (!gold_feature_sequences_keys[f].empty()) {
              auto it = features.scores[f].map.find(gold_feature_sequences_keys[f]);
              if (it == features.scores[f].map.end() && !prune_features) it = features.scores[f].map.emplace(gold_feature_sequences_keys[f], typename decltype(features.scores[f].map)::mapped_type()).first;
              if (it != features.scores[f].map.end()) {
                auto& gold_info = it->second;
                gold_info.gamma += gold_info.alpha * (s - gold_info.last_gamma_update);
                gold_info.last_gamma_update = s;
                gold_info.alpha++;
              }
            }
          }
        }
      }
    }

    // Finalize incremental gamma updates
    for (auto&& score : features.scores)
      for (auto&& element : score.map) {
        element.second.gamma += element.second.alpha * (train.size() - element.second.last_gamma_update);
        element.second.last_gamma_update = 0;
      }
    cerr << "done, accuracy " << fixed << setprecision(2) << train_correct * 100 / double(train_total) << '%';

    // If we have any heldout data, compute accuracy and if requested store best tagger configuration
    if (!heldout.empty()) {
      enum { TAGS, LEMMAS, BOTH, TOTAL };
      int heldout_correct[TOTAL] = {}, heldout_total = 0;

      typedef feature_sequences_optimizer<FeatureSequences> optimizer;
      typename optimizer::optimized_feature_sequences frozen_features;
      optimizer::optimize(features, frozen_features);
      viterbi<decltype(frozen_features)> frozen_decoder(frozen_features, decoding_order, window_size);
      typename decltype(frozen_decoder)::cache frozen_decoder_cache(frozen_decoder);

      for (auto&& sentence : heldout) {
        if (tags.size() < sentence.forms.size()) tags.resize(sentence.forms.size() * 2);
        frozen_decoder.tag(sentence.forms, sentence.analyses, frozen_decoder_cache, tags);

        for (unsigned i = 0; i < sentence.forms.size(); i++) {
          heldout_correct[TAGS] += sentence.gold[i].tag == sentence.analyses[i][tags[i]].tag;
          heldout_correct[LEMMAS] += sentence.gold[i].lemma == sentence.analyses[i][tags[i]].lemma;
          heldout_correct[BOTH] += sentence.gold[i].tag == sentence.analyses[i][tags[i]].tag && sentence.gold[i].lemma == sentence.analyses[i][tags[i]].lemma;
          heldout_total++;
        }
      }

      if (early_stopping && heldout_correct[BOTH] > best_correct) {
        best_correct = heldout_correct[BOTH];
        best_iteration = i;
        best_features = features;
      }

      cerr << ", heldout accuracy " << fixed << setprecision(2)
          << 100 * heldout_correct[TAGS] / double(heldout_total) << "%t/"
          << 100 * heldout_correct[LEMMAS] / double(heldout_total) << "%l/"
          << 100 * heldout_correct[BOTH] / double(heldout_total) << "%b";
    }
    cerr << endl;
  }

  if (early_stopping && best_iteration >= 0) {
    cerr << "Chosen tagger model from iteration " << best_iteration + 1 << endl;
    features = best_features;
  }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
