// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;

#include "tagger.h"
#include "utils/threadsafe_stack.h"
#include "viterbi.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template<class FeatureSequences>
class perceptron_tagger : public tagger {
 public:
  perceptron_tagger(int decoding_order, int window_size);

  bool load(istream& is);
  virtual const morpho* get_morpho() const override;
  virtual void tag(const vector<string_piece>& forms,
                   vector<tagged_lemma>& tags,
                   vector<string>& correct_analyses,
                   vector<string>& all_analyses,
                   bool provide_all_analyses,
                   morpho::guesser_mode guesser = morpho::guesser_mode(-1)) const override;
  virtual void tag_analyzed(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<int>& tags) const override;

 private:
  int decoding_order, window_size;

  unique_ptr<morpho> dict;
  bool use_guesser;
  FeatureSequences features;
  typedef viterbi<FeatureSequences> viterbi_decoder;
  viterbi_decoder decoder;
  struct cache {
    vector<string_piece> forms;
    vector<vector<tagged_lemma>> analyses;
    vector<int> tags;
    typename viterbi_decoder::cache decoder_cache;

    cache(const perceptron_tagger<FeatureSequences>& self) : decoder_cache(self.decoder) {}
  };

  mutable threadsafe_stack<cache> caches;
};


// Definitions

template<class FeatureSequences>
perceptron_tagger<FeatureSequences>::perceptron_tagger(int decoding_order, int window_size)
  : decoding_order(decoding_order), window_size(window_size), decoder(features, decoding_order, window_size) {}

template<class FeatureSequences>
bool perceptron_tagger<FeatureSequences>::load(istream& is) {
  if (dict.reset(morpho::load(is)), !dict) return false;
  use_guesser = is.get();
  if (!features.load(is)) return false;
  return true;
}

template<class FeatureSequences>
const morpho* perceptron_tagger<FeatureSequences>::get_morpho() const {
  return dict.get();
}

template<class FeatureSequences>
void perceptron_tagger<FeatureSequences>::tag(const vector<string_piece>& forms,
                                              vector<tagged_lemma>& tags,
                                              vector<string>& correct_analyses,
                                              vector<string>& all_analyses,
                                              bool provide_all_analyses,
                                              morpho::guesser_mode guesser) const {
    tags.clear();
    if (!dict) return;

    cache *c = caches.pop();
    if (!c) c = new cache(*this);

    c->forms.resize(forms.size());
    if (c->analyses.size() < forms.size()) c->analyses.resize(forms.size());
    for (unsigned i = 0; i < forms.size(); i++) {
        c->forms[i] = forms[i];
        c->forms[i].len = dict->raw_form_len(forms[i]);
        dict->analyze(forms[i], guesser >= 0 ? guesser : use_guesser ? morpho::GUESSER : morpho::NO_GUESSER,
                      c->analyses[i]);
    }

    if (c->tags.size() < forms.size()) c->tags.resize(forms.size() * 2);
    decoder.tag(c->forms, c->analyses, c->decoder_cache, c->tags);

    for (unsigned i = 0; i < forms.size(); i++)
        tags.emplace_back(c->analyses[i][c->tags[i]]);

    if (provide_all_analyses) {
//        json root_json;
//        json all_analyses_json = json::array();
        for (unsigned i = 0; i < forms.size(); i++) {
//            string all_analyses_for_position_i;
            json all_analyses_for_position_i_json = json::array();
            for (unsigned j = 0; j < c->analyses[i].size(); j++) {
                // take c->analyses[i][j]
                // replace | with &
                // join with ~ and add to the all_analyses vector
                //        c->analyses[i][j].lemma;
                //          c->analyses[i][j].;
                string analysis_at_i_j;
                string * target_string;
                target_string = new string(c->analyses[i][j].tag);

//                replace(target_string->begin(), target_string->end(), '|', '&');
//                replace(target_string->begin(), target_string->end(), '=', '>');
                analysis_at_i_j = c->analyses[i][j].lemma + "|" + *target_string;
//                if (j == 0) {
//                    all_analyses_for_position_i = analysis_at_i_j;
//                } else {
//                    all_analyses_for_position_i += "!" + analysis_at_i_j;
//                }

                all_analyses_for_position_i_json.push_back(analysis_at_i_j);
            }

//            all_analyses[i] = all_analyses_for_position_i;
//            all_analyses_json.push_back(all_analyses_for_position_i_json)
            all_analyses[i] = all_analyses_for_position_i_json.dump();

            correct_analyses[i] = c->analyses[i][c->tags[i]].lemma + "|" + *(new string(c->analyses[i][c->tags[i]].tag));
        }
//        root_json["ALL_ANALYSES"] = all_analyses_json;
    }

  caches.push(c);
}

template<class FeatureSequences>
void perceptron_tagger<FeatureSequences>::tag_analyzed(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<int>& tags) const {
  tags.clear();

  cache* c = caches.pop();
  if (!c) c = new cache(*this);

  tags.resize(forms.size());
  decoder.tag(forms, analyses, c->decoder_cache, tags);

  caches.push(c);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
