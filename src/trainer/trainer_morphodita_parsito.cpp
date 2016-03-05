// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <sstream>
#include <unordered_set>

#include "model/model_morphodita_parsito.h"
#include "morphodita/morpho/generic_morpho_encoder.h"
#include "morphodita/morpho/morpho.h"
#include "morphodita/morpho/morpho_statistical_guesser_trainer.h"
#include "morphodita/morpho/morpho_ids.h"
#include "morphodita/tagger/conllu_elementary_features.h"
#include "morphodita/tagger/feature_sequences.h"
#include "morphodita/tagger/perceptron_tagger_trainer.h"
#include "morphodita/tagger/tagger_ids.h"
#include "morphodita/tagger/tagger_trainer.h"
#include "utils/options.h"
#include "utils/parse_double.h"
#include "utils/parse_int.h"
#include "version/version.h"
#include "sentence/input_format.h"
#include "sentence/sentence.h"
#include "trainer_morphodita_parsito.h"
#include "utils/named_values.h"

namespace ufal {
namespace udpipe {

bool trainer_morphodita_parsito::train(const string& data, const string& /*tokenizer*/, const string& tagger, const string& parser, ostream& os, string& error) {
  error.clear();

  // Save model version info
  os.put(1);

  // Load input data
  vector<sentence> conllu;

  unique_ptr<input_format> conllu_input_format(input_format::new_conllu_input_format());
  conllu_input_format->set_text(data.c_str());
  while (conllu.emplace_back(), conllu_input_format->next_sentence(conllu.back(), error)) ;
  conllu.pop_back();
  if (!error.empty()) return false;

  bool have_lemmas = false;
  for (auto&& sentence : conllu)
    for (size_t i = 1; !have_lemmas && i < sentence.words.size(); i++)
      if (!sentence.words[i].lemma.empty() && sentence.words[i].lemma != "_")
        have_lemmas = true;

  if (tagger == "none") {
    os.put(0);
  } else {
    os.put(1);
    os.put(have_lemmas);

    // Create MorphoDiTa model
    named_values::map tagger_options;
    if (!named_values::parse(tagger, tagger_options, error)) return false;

    if (tagger_options.count("tagger_model")) {
      // Use specified tagger model
      cerr << "Using specified tagger model." << endl;
      os << tagger_options["tagger_model"];
    } else {
      // Start by creating the morphological dictionary
      stringstream morpho_description;
      string combined_tag;

      if (tagger_options.count("dictionary_model")) {
        // Use specified morphological dictionary
        cerr << "Using specified morphological dictionary model." << endl;
        morpho_description << tagger_options["dictionary_model"];
      } else {
        // Create the morphological dictionary and guesser from data
        cerr << "Creating morphological dictionary from training data." << endl;

        // Guesser options
        int guesser_suffix_len = 4, guesser_suffix_rules = 10, guesser_prefixes_max = 4, guesser_prefix_min_count = 10, guesser_enrich_dictionary = 4;
        if (tagger_options.count("guesser_suffix_len")) if (!parse_int(tagger_options["guesser_suffix_len"], "guesser_suffix_len", guesser_suffix_len, error)) return false;
        if (tagger_options.count("guesser_suffix_rules")) if (!parse_int(tagger_options["guesser_suffix_rules"], "guesser_suffix_rules", guesser_suffix_rules, error)) return false;
        if (tagger_options.count("guesser_prefixes_max")) if (!parse_int(tagger_options["guesser_prefixes_max"], "guesser_prefixes_max", guesser_prefixes_max, error)) return false;
        if (tagger_options.count("guesser_prefix_min_count")) if (!parse_int(tagger_options["guesser_prefix_min_count"], "guesser_prefix_min_count", guesser_prefix_min_count, error)) return false;
        if (tagger_options.count("guesser_enrich_dictionary")) if (!parse_int(tagger_options["guesser_enrich_dictionary"], "guesser_enrich_dictionary", guesser_enrich_dictionary, error)) return false;

        // Dictionary options
        int dictionary_suffix_len = 8;
        if (tagger_options.count("dictionary_suffix_len")) if (!parse_int(tagger_options["dictionary_suffix_len"], "dictionary_suffix_len", dictionary_suffix_len, error)) return false;

        // Start by generating statistical guesser
        stringstream guesser_description;
        {
          stringstream guesser_input;
          for (auto&& sentence : conllu) {
            for (size_t i = 1; i < sentence.words.size(); i++)
              guesser_input << sentence.words[i].form << '\t' << combine_lemma(sentence.words[i].form, sentence.words[i].lemma, have_lemmas) << '\t' << combine_tags(sentence.words[i], combined_tag) << '\n';
            guesser_input << '\n';
          }
          morphodita::morpho_statistical_guesser_trainer::train(guesser_input, guesser_suffix_len, guesser_suffix_rules, guesser_prefixes_max, guesser_prefix_min_count, guesser_description);
        }

        // Generate morphological dictionary data from the input
        unordered_set<string> dictionary_entries;
        {
          string entry;
          for (auto&& sentence : conllu)
            for (size_t i = 1; i < sentence.words.size(); i++)
              dictionary_entries.insert(entry.assign(combine_lemma(sentence.words[i].form, sentence.words[i].lemma, have_lemmas)).append("\t").append(combine_tags(sentence.words[i], combined_tag)).append("\t").append(sentence.words[i].form));
        }

        morphodita::generic_morpho_encoder::tags dictionary_special_tags;
        dictionary_special_tags.unknown_tag = "X";
        dictionary_special_tags.number_tag = "NUM";
        dictionary_special_tags.punctuation_tag = "PUNCT";
        dictionary_special_tags.symbol_tag = "SYM";

        // Enrich the dictionary if required
        if (guesser_enrich_dictionary) {
          // Create temporary morphology using only the guesser
          stringstream empty_data, guesser_description_copy(guesser_description.str()), guesser_only_morphology;
          guesser_only_morphology.put(morphodita::morpho_ids::GENERIC);
          morphodita::generic_morpho_encoder::encode(empty_data, dictionary_suffix_len, dictionary_special_tags, guesser_description_copy, guesser_only_morphology);

          unique_ptr<morphodita::morpho> guesser_only_morpho(morphodita::morpho::load(guesser_only_morphology));
          if (!guesser_only_morpho) return error.assign("Cannot create temporary guesser-only morphology!"), false;

          string entry;
          unordered_set<string> analyzed_forms;
          vector<morphodita::tagged_lemma> analyses;
          for (auto&& sentence : conllu)
            for (size_t i = 1; i < sentence.words.size(); i++) {
              const auto& form = sentence.words[i].form;
              if (!analyzed_forms.count(form)) {
                guesser_only_morpho->analyze(form, morphodita::morpho::GUESSER, analyses);

                int to_add = guesser_enrich_dictionary;
                for (auto&& analyse : analyses) {
                  entry.assign(analyse.lemma).push_back('\t');
                  entry.append(analyse.tag).push_back('\t');
                  entry.append(form);
                  if (dictionary_entries.insert(entry).second)
                    if (!--to_add)
                      break;
                }
                analyzed_forms.insert(form);
              }
            }
        }

        // Create the dictionary
        vector<string> sorted_dictionary(dictionary_entries.begin(), dictionary_entries.end());
        sort(sorted_dictionary.begin(), sorted_dictionary.end());

        stringstream morpho_input;
        for (auto&& entry : sorted_dictionary)
          morpho_input << entry << '\n';

        morpho_description.put(morphodita::morpho_ids::GENERIC);
        morphodita::generic_morpho_encoder::encode(morpho_input, dictionary_suffix_len, dictionary_special_tags, guesser_description, morpho_description);
      }

      // Measure dictionary accuracy if required
      const string& dictionary_accuracy = tagger_options["dictionary_accuracy"];
      if (!dictionary_accuracy.empty()) {
        unique_ptr<morphodita::morpho> morpho(morphodita::morpho::load(morpho_description));
        if (!morpho) return error.assign("Cannot create temporary morphology for evaluating accuracy!"), false;
        morpho_description.seekg(0, ios::beg);

        // Measure dictionary accuracy on given data
        unsigned words = 0, total_analyses = 0, upostag = 0, xpostag = 0, feats = 0, all_tags = 0, lemma = 0;

        word w;
        vector<morphodita::tagged_lemma> analyses;
        conllu_input_format->set_text(dictionary_accuracy.c_str());
        for (sentence sentence; conllu_input_format->next_sentence(sentence, error); )
          for (size_t i = 1; i < sentence.words.size(); i++) {
            morpho->analyze(sentence.words[i].form, morphodita::morpho::GUESSER, analyses);
            unsigned upostag_ok = 0, xpostag_ok = 0, feats_ok = 0, all_tags_ok = 0, lemma_ok = 0;
            for (auto&& analysis : analyses) {
              model_morphodita_parsito::fill_word_analysis(analysis, have_lemmas, w);
              upostag_ok |= sentence.words[i].upostag == w.upostag;
              xpostag_ok |= sentence.words[i].xpostag == w.xpostag;
              feats_ok |= sentence.words[i].feats == w.feats;
              all_tags_ok |= sentence.words[i].upostag == w.upostag && sentence.words[i].xpostag == w.xpostag && sentence.words[i].feats == w.feats;
              lemma_ok |= sentence.words[i].lemma == w.lemma;
            }
            words++;
            total_analyses += analyses.size();
            upostag += upostag_ok;
            xpostag += xpostag_ok;
            feats += feats_ok;
            all_tags += all_tags_ok;
            lemma += lemma_ok;
          }
        if (!error.empty()) return false;

        cerr << "Dictionary accuracy - forms: " << words << ", analyses per form: " << fixed << setprecision(2) << total_analyses / double(words)
             << ", upostag: " << setprecision(1) << 100. * upostag / words << "%, xpostag: " << 100. * xpostag / words
             << "%, feats: " << 100. * feats / words << "%, all tags: " << 100. * all_tags / words << "%, lemma: " << 100. * lemma / words << '%' << endl;
      }

      // Tagger options
      morphodita::tagger_id tagger_id = morphodita::tagger_ids::CONLLU3;
      if (tagger_options.count("order")) {
        double tagger_order;
        if (!parse_double(tagger_options["order"], "tagger_order", tagger_order, error)) return false;
        if (tagger_order == 2) tagger_id = morphodita::tagger_ids::CONLLU2;
        else if (tagger_order == 2.5) tagger_id = morphodita::tagger_ids::CONLLU2_3;
        else if (tagger_order == 3) tagger_id = morphodita::tagger_ids::CONLLU3;
        else return error.assign("The tagger_order can be only 2, 2.5 or 3, not '").append(tagger_options["order"]).append("'!"), false;
      }
      int tagger_iterations = 15, tagger_prune_features = 0, tagger_early_stopping = 1;
      if (tagger_options.count("iterations")) if (!parse_int(tagger_options["iterations"], "tagger_iterations", tagger_iterations, error)) return false;
      if (tagger_options.count("prune_features")) if (!parse_int(tagger_options["prune_features"], "tagger_prune_features", tagger_prune_features, error)) return false;
      if (tagger_options.count("early_stopping")) if (!parse_int(tagger_options["early_stopping"], "tagger_early_stopping", tagger_early_stopping, error)) return false;
      const string& tagger_feature_templates = tagger_options["templates"];
      const string& tagger_heldout = tagger_options["heldout"];
      const string& tagger_accuracy = tagger_options["accuracy"];

      // Train the tagger
      cerr << "Training tagger." << endl;
      stringstream input, heldout_input, feature_templates_input(tagger_feature_templates);
      for (auto&& sentence : conllu) {
        for (size_t i = 1; i < sentence.words.size(); i++)
          input << sentence.words[i].form << '\t' << combine_lemma(sentence.words[i].form, sentence.words[i].lemma, have_lemmas) << '\t' << combine_tags(sentence.words[i], combined_tag) << '\n';
        input << '\n';
      }

      int ho = 0;
      conllu_input_format->set_text(tagger_heldout.c_str());
      for (sentence sentence; conllu_input_format->next_sentence(sentence, error); ) {
        for (size_t i = 1; i < sentence.words.size(); i++)
          heldout_input << sentence.words[i].form << '\t' << combine_lemma(sentence.words[i].form, sentence.words[i].lemma, have_lemmas) << '\t' << combine_tags(sentence.words[i], combined_tag) << '\n';
        heldout_input << '\n';
        ho += sentence.words.size() - 1;
      }

      stringstream tagger_description;
      tagger_description.put(tagger_id);
      morphodita::tagger_trainer<morphodita::perceptron_tagger_trainer<morphodita::train_feature_sequences<morphodita::conllu_elementary_features>>>::train(morphodita::tagger_ids::decoding_order(tagger_id), morphodita::tagger_ids::window_size(tagger_id), tagger_iterations, morpho_description, true, feature_templates_input, tagger_prune_features, input, heldout_input, tagger_early_stopping, tagger_description);

      // Measure tagger accuracy if required
      if (!tagger_accuracy.empty()) {
        unique_ptr<morphodita::tagger> tagger(morphodita::tagger::load(tagger_description));
        if (!tagger) return error.assign("Cannot create temporary tagger for evaluating accuracy!"), false;
        tagger_description.seekg(0, ios::beg);

        word w;
        vector<string_piece> forms;
        vector<morphodita::tagged_lemma> analyses;
        int words = 0, upostag = 0, xpostag = 0, feats = 0, all_tags = 0, lemma = 0;
        conllu_input_format->set_text(tagger_accuracy.c_str());
        for (sentence sentence; conllu_input_format->next_sentence(sentence, error); ) {
          forms.clear();
          for (size_t i = 1; i < sentence.words.size(); i++)
            forms.emplace_back(sentence.words[i].form);

          tagger->tag(forms, analyses);

          for (size_t i = 0; i < analyses.size(); i++) {
            model_morphodita_parsito::fill_word_analysis(analyses[i], have_lemmas, w);
            words++;
            upostag += sentence.words[i+1].upostag == w.upostag;
            xpostag += sentence.words[i+1].xpostag == w.xpostag;
            feats += sentence.words[i+1].feats == w.feats;
            all_tags += sentence.words[i+1].upostag == w.upostag && sentence.words[i+1].xpostag == w.xpostag && sentence.words[i+1].feats == w.feats;
            lemma += sentence.words[i+1].lemma == w.lemma;
          }
        }
        if (!error.empty()) return false;

        cerr << "Tagger accuracy - forms: " << words << ", upostag: " << setprecision(2) << 100. * upostag / words << "%, xpostag: " << 100. * xpostag / words
             << "%, feats: " << 100. * feats / words << "%, all tags: " << 100. * all_tags / words << "%, lemma: " << 100. * lemma / words << '%' << endl;
      }

      os << tagger_description.rdbuf();
    }
  }

  if (parser == "none") {
    os.put(0);
  } else {
    os.put(1);
  }

  return true;
}

const string& trainer_morphodita_parsito::combine_tags(const word& w, string& combined_tag) {
  combined_tag.assign(w.upostag).push_back('|');
  combined_tag.append(w.xpostag).push_back('|');
  combined_tag.append(w.feats);

  return combined_tag;
}

const string& trainer_morphodita_parsito::combine_lemma(const string& form, const string& lemma, bool have_lemmas) {
  return have_lemmas ? lemma : form;
}

} // namespace udpipe
} // namespace ufal
