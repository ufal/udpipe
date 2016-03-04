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
#include "feature_sequences_optimizer.h"
#include "morphodita/morpho/morpho.h"
#include "training_maps.h"
#include "viterbi.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class TaggerTrainer>
class tagger_trainer {
 public:
  struct sentence {
    vector<string> words;
    vector<string_piece> forms;
    vector<vector<tagged_lemma>> analyses;
    vector<tagged_lemma> gold;
    vector<int> gold_index;
  };

  static void train(int decoding_order, int window_size, int iterations, istream& in_morpho_dict, bool use_guesser, istream& in_feature_templates, bool prune_features, istream& in_train, istream& in_heldout, bool early_stopping, ostream& out_tagger);

 private:
  static double load_data(istream& is, const morpho& d, bool use_guesser, vector<sentence>& sentences, bool add_gold);
};


// Definitions
template <class TaggerTrainer>
void tagger_trainer<TaggerTrainer>::train(int decoding_order, int window_size, int iterations, istream& in_morpho_dict, bool use_guesser, istream& in_feature_templates, bool prune_features, istream& in_train, istream& in_heldout, bool early_stopping, ostream& out_tagger) {
//  cerr << "Loading dictionary: ";
  unique_ptr<morpho> d(morpho::load(in_morpho_dict));
  if (!d) runtime_failure("Cannot load dictionary!");
//  cerr << "done" << endl;
  if (!in_morpho_dict.seekg(0, istream::beg)) runtime_failure("Cannot seek in dictionary file to the beginning!");

  vector<sentence> train_data;
//  cerr << "Loading train data: ";
//  cerr << "done, matched " << fixed << setprecision(2) << 100 * load_data(in_train, *d, use_guesser, train_data, true) << '%' << endl;
  load_data(in_train, *d, use_guesser, train_data, true);

  vector<sentence> heldout_data;
  if (in_heldout) {
//    cerr << "Loading heldout data: ";
//    cerr << "done, matched " << fixed << setprecision(2) << 100 * load_data(in_heldout, *d, use_guesser, heldout_data, false) << '%' << endl;
    load_data(in_heldout, *d, use_guesser, heldout_data, false);
  }

  // Encode morphological dictionary
//  cerr << "Encoding morphological dictionary." << endl;
  out_tagger << in_morpho_dict.rdbuf();
  out_tagger.put(use_guesser);

  // Train and encode the tagger
  TaggerTrainer::train(decoding_order, window_size, iterations, train_data, heldout_data, early_stopping, prune_features, in_feature_templates, out_tagger);
}

template <class TaggerTrainer>
double tagger_trainer<TaggerTrainer>::load_data(istream& is, const morpho& d, bool use_guesser, vector<sentence>& sentences, bool add_gold) {
  sentences.clear();

  int forms = 0, forms_matched = 0;

  string line;
  vector<string> tokens;
  sentences.emplace_back();
  while (getline(is, line)) {
    if (line.empty()) {
      if (!sentences.back().words.empty())
        sentences.emplace_back();
      continue;
    }

    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_failure("The tagger data line '" << line << "' does not contain three columns!");

    // Add form to sentence
    forms++;
    sentence& s = sentences.back();
    s.words.emplace_back(tokens[0]);
    s.forms.emplace_back(string_piece(s.words.back().c_str(), d.raw_form_len(s.words.back())));
    s.gold.emplace_back(tokens[1], tokens[2]);
    s.gold_index.emplace_back(-1);

    // Analyse
    s.analyses.emplace_back();
    d.analyze(tokens[0], use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, s.analyses.back());

    // Locate gold analysis
    for (size_t i = 0; i < s.analyses.back().size(); i++)
      if (s.analyses.back()[i].lemma == s.gold.back().lemma && s.analyses.back()[i].tag == s.gold.back().tag) {
        s.gold_index.back() = i;
        forms_matched++;
        break;
      }
    if (s.gold_index.back() == -1 && add_gold) {
      s.gold_index.back() = s.analyses.back().size();
      s.analyses.back().emplace_back(tokens[1], tokens[2]);
    }
  }
  if (!sentences.empty() && sentences.back().words.empty()) sentences.pop_back();

  return forms_matched / double(forms);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
