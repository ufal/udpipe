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
#include "morphodita/tokenizer/tokenizer_ids.h"
#include "morphodita/tokenizer/generic_tokenizer.h"
#include "morphodita/tokenizer/generic_tokenizer_factory_encoder.h"
#include "utils/options.h"
#include "utils/parse_double.h"
#include "utils/parse_int.h"
#include "version/version.h"
#include "sentence/input_format.h"
#include "sentence/sentence.h"
#include "trainer_morphodita_parsito.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {

bool trainer_morphodita_parsito::train(const string& data, const string& tokenizer, const string& tagger, const string& parser, ostream& os, string& error) {
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

  // Check input data
  for (auto&& sentence : conllu)
    for (size_t i = 1; i < sentence.words.size(); i++)
      if (!can_combine_tag(sentence.words[i], error))
        return false;

  if (!train_tokenizer(conllu, tokenizer, os, error)) return false;
  if (!train_tagger(conllu, tagger, os, error)) return false;
  if (!train_parser(conllu, parser, os, error)) return false;

  return true;
}

bool trainer_morphodita_parsito::train_tokenizer(const vector<sentence>& /*data*/, const string& options, ostream& os, string& error) {
  if (options == "none") {
    os.put(0);
  } else if (options == "generic") {
    os.put(1);

    os.put(morphodita::tokenizer_id::GENERIC);
    morphodita::generic_tokenizer_factory_encoder::encode(morphodita::generic_tokenizer::LATEST, os);
  } else {
    // Tokenizer options
    named_values::map tokenizer;
    if (!named_values::parse(options, tokenizer, error)) return false;

    if (tokenizer.count("model")) {
      // Use specified tokenizer model
      cerr << "Using specified tokenizer model." << endl;
      // TODO
    } else {
      error.assign("Trainable tokenizer not implemented yet!");
      return false;
    }
  }

  return true;
}

bool trainer_morphodita_parsito::train_tagger(const vector<sentence>& data, const string& options, ostream& os, string& error) {
  if (options == "none") {
    os.put(0);
  } else {
    // Parse options
    named_values::map tagger;
    if (!named_values::parse(options, tagger, error)) return false;

    if (tagger.count("model")) {
      // Use specified tagger model
      cerr << "Using specified tagger model." << endl;
      // TODO
    } else {
      // Create MorphoDiTa model(s)
      int models = 2; if (!option_int(tagger, "models", models, error)) return false;
      if (models <= 0) return error.assign("Number of tagger models cannot be negative or zero!"), false;
      if (models > 4) return error.assign("Cannot create more than four tagger models!"), false;

      os.put(models);
      for (int model = 0; model < models; model++)
        if (!train_tagger_model(data, model, models, tagger, os, error))
          return false;
    }
  }

  return true;
}

bool trainer_morphodita_parsito::train_tagger_model(const vector<sentence>& data, unsigned model, unsigned models, const named_values::map& tagger, ostream& os, string& error) {
  unique_ptr<input_format> conllu_input_format(input_format::new_conllu_input_format());

  bool have_lemma = false;
  for (auto&& sentence : data)
    for (size_t i = 1; !have_lemma && i < sentence.words.size(); i++)
      if (!sentence.words[i].lemma.empty() && sentence.words[i].lemma != "_")
        have_lemma = true;

  bool use_lemma = model == 1 || models == 1; if (!option_bool(tagger, "use_lemma", use_lemma, error, model)) return false;
  bool use_xpostag = model == 0; if (!option_bool(tagger, "use_xpostag", use_xpostag, error, model)) return false;
  bool use_feats = model == 0; if (!option_bool(tagger, "use_feats", use_feats, error, model)) return false;
  use_lemma = use_lemma && have_lemma;

  bool provide_lemma = model == 1 || models == 1; if (!option_bool(tagger, "provide_lemma", provide_lemma, error, model)) return false;
  bool provide_xpostag = model == 0; if (!option_bool(tagger, "provide_xpostag", provide_xpostag, error, model)) return false;
  bool provide_feats = model == 0; if (!option_bool(tagger, "provide_feats", provide_feats, error, model)) return false;
  os.put(char(provide_lemma && use_lemma));
  os.put(char(provide_xpostag && use_xpostag));
  os.put(char(provide_feats && use_feats));

  // Start by creating the morphological dictionary
  stringstream morpho_description;
  string combined_tag;

  // Generic options
  const string& dictionary = option_str(tagger, "dictionary_model", model);
  if (!dictionary.empty()) {
    // Use specified morphological dictionary
    cerr << "Using given morphological dictionary for tagger model " << model+1 << "." << endl;
    morpho_description << dictionary;
  } else {
    // Create the morphological dictionary and guesser from data
    cerr << "Creating morphological dictionary for tagger model " << model+1 << "." << endl;

    // Guesser options
    int guesser_suffix_len = 4; if (!option_int(tagger, "guesser_suffix_len", guesser_suffix_len, error, model)) return false;
    int guesser_suffix_rules = 8; if (!option_int(tagger, "guesser_suffix_rules", guesser_suffix_rules, error, model)) return false;
    int guesser_prefixes_max = provide_lemma ? 4 : 0; if (!option_int(tagger, "guesser_prefixes_max", guesser_prefixes_max, error, model)) return false;
    int guesser_prefix_min_count = 10; if (!option_int(tagger, "guesser_prefix_min_count", guesser_prefix_min_count, error, model)) return false;
    int guesser_enrich_dictionary = 6; if (!option_int(tagger, "guesser_enrich_dictionary", guesser_enrich_dictionary, error, model)) return false;

    // Dictionary options
    int dictionary_suffix_len = 8; if (!option_int(tagger, "dictionary_suffix_len", dictionary_suffix_len, error, model)) return false;
    unordered_set<string> drop_lemmas;
    if (!option_str(tagger, "dictionary_drop_lemmas", model).empty()) {
      vector<string> lemmas;
      split(option_str(tagger, "dictionary_drop_lemmas", model), ',', lemmas);
      drop_lemmas.insert(lemmas.begin(), lemmas.end());
    }

    // Start by generating statistical guesser
    stringstream guesser_description;
    {
      stringstream guesser_input;
      for (auto&& sentence : data) {
        for (size_t i = 1; i < sentence.words.size(); i++)
          guesser_input << sentence.words[i].form << '\t' << combine_lemma(sentence.words[i], use_lemma, drop_lemmas) << '\t' << combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag) << '\n';
        guesser_input << '\n';
      }
      morphodita::morpho_statistical_guesser_trainer::train(guesser_input, guesser_suffix_len, guesser_suffix_rules, guesser_prefixes_max, guesser_prefix_min_count, guesser_description);
    }

    // Generate morphological dictionary data from the input
    unordered_set<string> dictionary_entries;
    {
      string entry;
      for (auto&& sentence : data)
        for (size_t i = 1; i < sentence.words.size(); i++)
          dictionary_entries.insert(entry.assign(combine_lemma(sentence.words[i], use_lemma, drop_lemmas)).append("\t").append(combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag)).append("\t").append(sentence.words[i].form));
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
      for (auto&& sentence : data)
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
  const string& dictionary_accuracy = option_str(tagger, "dictionary_accuracy", model);
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
          w.lemma.assign("_");
          model_morphodita_parsito::fill_word_analysis(analysis, true, have_lemma, true, true, w);
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

    cerr << "Dictionary accuracy for tagging model " << model+1 << " - forms: " << words
         << ", analyses per form: " << fixed << setprecision(2) << total_analyses / double(words)
         << ", upostag: " << setprecision(1) << 100. * upostag / words << "%, xpostag: " << 100. * xpostag / words
         << "%, feats: " << 100. * feats / words << "%, all tags: " << 100. * all_tags / words << "%, lemma: " << 100. * lemma / words << '%' << endl;
  }

  // Tagger options
  double tagger_order = 3; if (!option_double(tagger, "order", tagger_order, error, model)) return false;
  morphodita::tagger_id tagger_id;
  if (tagger_order == 2) tagger_id = morphodita::tagger_ids::CONLLU2;
  else if (tagger_order == 2.5) tagger_id = morphodita::tagger_ids::CONLLU2_3;
  else if (tagger_order == 3) tagger_id = morphodita::tagger_ids::CONLLU3;
  else return error.assign("The tagger_order can be only 2, 2.5 or 3!"), false;

  int tagger_iterations = 20; if (!option_int(tagger, "iterations", tagger_iterations, error, model)) return false;
  bool tagger_prune_features = false; if (!option_bool(tagger, "prune_features", tagger_prune_features, error, model)) return false;
  bool tagger_early_stopping = true; if (!option_bool(tagger, "early_stopping", tagger_early_stopping, error, model)) return false;
  const string& tagger_feature_templates =
      option_str(tagger, "templates", model) == "tagger" ? tagger_features_tagger :
      option_str(tagger, "templates", model) == "lemmatizer" ? tagger_features_lemmatizer :
      !option_str(tagger, "templates", model).empty() ? option_str(tagger, "templates", model) :
      model == 1 ? tagger_features_lemmatizer : tagger_features_tagger;

  const string& tagger_heldout = option_str(tagger, "heldout", model);
  if (tagger_heldout.empty()) tagger_early_stopping = false;
  const string& tagger_accuracy = option_str(tagger, "accuracy", model);

  // Train the tagger
  cerr << "Training tagger model " << model+1 << "." << endl;
  stringstream input, heldout_input, feature_templates_input(tagger_feature_templates);
  for (auto&& sentence : data) {
    for (size_t i = 1; i < sentence.words.size(); i++)
      input << sentence.words[i].form << '\t' << combine_lemma(sentence.words[i], use_lemma) << '\t' << combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag) << '\n';
    input << '\n';
  }

  conllu_input_format->set_text(tagger_heldout.c_str());
  for (sentence sentence; conllu_input_format->next_sentence(sentence, error); ) {
    for (size_t i = 1; i < sentence.words.size(); i++)
      heldout_input << sentence.words[i].form << '\t' << combine_lemma(sentence.words[i], use_lemma) << '\t' << combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag) << '\n';
    heldout_input << '\n';
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
        w.lemma.assign("_");
        model_morphodita_parsito::fill_word_analysis(analyses[i], true, have_lemma, true, true, w);
        words++;
        upostag += sentence.words[i+1].upostag == w.upostag;
        xpostag += sentence.words[i+1].xpostag == w.xpostag;
        feats += sentence.words[i+1].feats == w.feats;
        all_tags += sentence.words[i+1].upostag == w.upostag && sentence.words[i+1].xpostag == w.xpostag && sentence.words[i+1].feats == w.feats;
        lemma += sentence.words[i+1].lemma == w.lemma;
      }
    }
    if (!error.empty()) return false;

    cerr << "Tagger accuracy for model " << models+1 << " - forms: " << words
         << ", upostag: " << setprecision(2) << 100. * upostag / words << "%, xpostag: " << 100. * xpostag / words
         << "%, feats: " << 100. * feats / words << "%, all tags: " << 100. * all_tags / words << "%, lemma: " << 100. * lemma / words << '%' << endl;
  }

  os << tagger_description.rdbuf();

  return true;
}

bool trainer_morphodita_parsito::train_parser(const vector<sentence>& /*data*/, const string& options, ostream& os, string& error) {
  if (options == "none") {
    os.put(0);
  } else {
    // Create Parsito model
    named_values::map parser;
    if (!named_values::parse(options, parser, error)) return false;

    if (parser.count("model")) {
      // Use specified parser model
      cerr << "Using specified parser model." << endl;
      // TODO
    } else {
      os.put(1);

      // Parsito options
      string transition_system = parser.count("transition_system") ? parser["transition_system"] : "projective";
      string transition_oracle = parser.count("transition_oracle") ? parser["transition_oracle"] :
          transition_system == "projective" ? "dynamic" :
          transition_system == "swap" ? "static_lazy" :
          "static";

      int embedding_upostag = 20; if (!option_int(parser, "embedding_upostag", embedding_upostag, error)) return false;
      int embedding_feats = 20; if (!option_int(parser, "embedding_feats", embedding_feats, error)) return false;
      int embedding_xpostag = 0; if (!option_int(parser, "embedding_xpostag", embedding_xpostag, error)) return false;
      int embedding_form = 50; if (!option_int(parser, "embedding_form", embedding_form, error)) return false;
      int embedding_lemma = 0; if (!option_int(parser, "embedding_lemma", embedding_lemma, error)) return false;
      int embedding_deprel = 20; if (!option_int(parser, "embedding_deprel", embedding_deprel, error)) return false;
      string embeddings;
      if (embedding_upostag) embeddings.append("universal_tag ").append(to_string(embedding_upostag)).append(" 1\n");
      if (embedding_feats) embeddings.append("feats ").append(to_string(embedding_feats)).append(" 1\n");
      if (embedding_xpostag) embeddings.append("tag ").append(to_string(embedding_xpostag)).append(" 1\n");
      if (embedding_form) {
        embeddings.append("form ").append(to_string(embedding_form)).append(" 2");
        if (!option_str(parser, "embedding_form_file").empty()) embeddings.append(" ").append(option_str(parser, "embedding_form_file"));
        embeddings.push_back('\n');
      }
      if (embedding_lemma) {
        embeddings.append("lemma ").append(to_string(embedding_lemma)).append(" 2");
        if (!option_str(parser, "embedding_lemma_file").empty()) embeddings.append(" ").append(option_str(parser, "embedding_lemma_file"));
        embeddings.push_back('\n');
      }
      if (embedding_deprel) embeddings.append("deprel ").append(to_string(embedding_deprel)).append(" 1\n");

      int iterations = 10; if (!option_int(parser, "iterations", iterations, error)) return false;
      int hidden_layer = 200; if (!option_int(parser, "hidden_layer", hidden_layer, error)) return false;
      int batch_size = 10; if (!option_int(parser, "batch_size", batch_size, error)) return false;
      int structured_interval = 8; if (!option_int(parser, "structured_interval", structured_interval, error)) return false;
      double learning_rate = 0.01; if (!option_double(parser, "learning_rate", learning_rate, error)) return false;
      double learning_rate_final = 0.001; if (!option_double(parser, "learning_rate_final", learning_rate_final, error)) return false;
      double l2 = 0.5; if (!option_double(parser, "l2", l2, error)) return false;

      // Train the parser
    }
  }

  return true;
}

bool trainer_morphodita_parsito::can_combine_tag(const word& w, string& error) {
  error.clear();

  unsigned separator = 0;
  while (separator < tag_separators.size() &&
         (w.upostag.find(tag_separators[separator]) != string::npos || w.xpostag.find(tag_separators[separator]) != string::npos))
    separator++;

  if (separator >= tag_separators.size()) {
    error.assign("Cannot find tag separating character, UPOSTAG and XPOSTAG contain all of '").append(tag_separators).append("'!");
    return false;
  }
  return true;
}

const string& trainer_morphodita_parsito::combine_tag(const word& w, bool xpostag, bool feats, string& combined_tag) {
  unsigned separator = 0;
  while (separator < tag_separators.size() &&
         (w.upostag.find(tag_separators[separator]) != string::npos || w.xpostag.find(tag_separators[separator]) != string::npos))
    separator++;
  if (separator >= tag_separators.size())
    runtime_failure("Cannot find tag separating character, UPOSTAG and XPOSTAG contain all of '" << tag_separators << "'!");

  combined_tag.assign(1, tag_separators[separator]);
  combined_tag.append(w.upostag);
  if (xpostag || feats) {
    combined_tag.push_back(tag_separators[separator]);
    if (xpostag) combined_tag.append(w.xpostag);
    if (feats) combined_tag.push_back(tag_separators[separator]);
    if (feats) combined_tag.append(w.feats);
  }

  return combined_tag;
}

const string& trainer_morphodita_parsito::combine_lemma(const word& w, bool use_lemma, const unordered_set<string>& drop_lemmas) {
  return use_lemma && !drop_lemmas.count(w.lemma) ? w.lemma : w.form;
}

const string& trainer_morphodita_parsito::option_str(const named_values::map& options, const string& name, int model) {
  string indexed_name(name);
  if (model >= 0 && model < 9) indexed_name.append("_").push_back('1' + model);

  return options.count(indexed_name) ? options.at(indexed_name) : options.count(name) ? options.at(name) : empty_string;
}

bool trainer_morphodita_parsito::option_int(const named_values::map& options, const string& name, int& value, string& error, int model) {
  string indexed_name(name);
  if (model >= 0 && model < 9) indexed_name.append("_").push_back('1' + model);

  if (options.count(indexed_name))
    return parse_int(options.at(indexed_name), name.c_str(), value, error);
  if (options.count(name))
    return parse_int(options.at(name), name.c_str(), value, error);
  return true;
}

bool trainer_morphodita_parsito::option_bool(const named_values::map& options, const string& name, bool& value, string& error, int model) {
  string indexed_name(name);
  if (model >= 0 && model < 9) indexed_name.append("_").push_back('1' + model);

  if (options.count(indexed_name) || options.count(name)) {
    int int_value;
    if (!parse_int(options.count(indexed_name) ? options.at(indexed_name) : options.at(name), name.c_str(), int_value, error))
      return false;
    value = int_value != 0;
  }
  return true;
}

bool trainer_morphodita_parsito::option_double(const named_values::map& options, const string& name, double& value, string& error, int model) {
  string indexed_name(name);
  if (model >= 0 && model < 9) indexed_name.append("_").push_back('1' + model);

  if (options.count(indexed_name))
    return parse_double(options.at(indexed_name), name.c_str(), value, error);
  if (options.count(name))
    return parse_double(options.at(name), name.c_str(), value, error);
  return true;
}

const string trainer_morphodita_parsito::empty_string;

const string trainer_morphodita_parsito::tag_separators = "~!@#$%^&*()/";

const string trainer_morphodita_parsito::tagger_features_tagger =
  "Tag 0\n"
  "Tag 0,Tag -1\n"
  "Tag 0,TagUPos -1\n"
  "Tag 0,Tag -1,Tag -2\n"
  "Tag 0,TagUPos -1,TagUPos -2\n"
  "Tag 0,Tag -2\n"
  "Tag 0,Form 0\n"
  "Tag 0,Form 0,Form -1\n"
  "Tag 0,Form -1\n"
  "Tag 0,Form -2\n"
  "Tag 0,Form -1,Form -2\n"
  "Tag 0,Form 1\n"
  "Tag 0,Form 1,Form 2\n"
  "Tag 0,PreviousVerbTag 0\n"
  "Tag 0,PreviousVerbForm 0\n"
  "Tag 0,FollowingVerbTag 0\n"
  "Tag 0,FollowingVerbForm 0\n"
  "Tag 0,Lemma -1\n"
  "Tag 0,Form 1\n"
  "Lemma 0,Tag -1\n"
  "Tag 0,Prefix1 0\n"
  "Tag 0,Prefix2 0\n"
  "Tag 0,Prefix3 0\n"
  "Tag 0,Prefix4 0\n"
  "Tag 0,Prefix5 0\n"
  "Tag 0,Prefix6 0\n"
  "Tag 0,Prefix7 0\n"
  "Tag 0,Prefix8 0\n"
  "Tag 0,Prefix9 0\n"
  "Tag 0,Suffix1 0\n"
  "Tag 0,Suffix2 0\n"
  "Tag 0,Suffix3 0\n"
  "Tag 0,Suffix4 0\n"
  "Tag 0,Suffix5 0\n"
  "Tag 0,Suffix6 0\n"
  "Tag 0,Suffix7 0\n"
  "Tag 0,Suffix8 0\n"
  "Tag 0,Suffix9 0\n"
  "TagUPos 0\n"
  "TagUPos 0,TagUPos -1\n"
  "TagUPos 0,TagUPos -1,TagUPos -2\n"
  "TagCase 0,TagCase -1\n"
  "TagCase 0,TagCase -1,TagCase -2\n"
  "TagGender 0,TagGender -1\n"
  "TagGender 0,TagGender -1,TagGender -2\n"
  "TagUPos 0,Prefix1 0\n"
  "TagUPos 0,Prefix2 0\n"
  "TagUPos 0,Prefix3 0\n"
  "TagUPos 0,Prefix4 0\n"
  "TagUPos 0,Prefix5 0\n"
  "TagUPos 0,Prefix6 0\n"
  "TagUPos 0,Prefix7 0\n"
  "TagUPos 0,Prefix8 0\n"
  "TagUPos 0,Prefix9 0\n"
  "TagUPos 0,Suffix1 0\n"
  "TagUPos 0,Suffix2 0\n"
  "TagUPos 0,Suffix3 0\n"
  "TagUPos 0,Suffix4 0\n"
  "TagUPos 0,Suffix5 0\n"
  "TagUPos 0,Suffix6 0\n"
  "TagUPos 0,Suffix7 0\n"
  "TagUPos 0,Suffix8 0\n"
  "TagUPos 0,Suffix9 0\n"
  "Tag 0,Num 0\n"
  "Tag 0,Cap 0\n"
  "Tag 0,Dash 0\n"
  "TagNegative 0,Prefix1 0\n"
  "TagNegative 0,Prefix2 0\n"
  "TagNegative 0,Prefix3 0\n"
  "TagCase 0,Suffix1 0\n"
  "TagCase 0,Suffix2 0\n"
  "TagCase 0,Suffix3 0\n"
  "TagCase 0,Suffix4 0\n"
  "TagCase 0,Suffix5 0\n";

const string trainer_morphodita_parsito::tagger_features_lemmatizer =
  "Tag 0\n"
  "Tag 0,Tag -1\n"
  "Tag 0,Tag -1,Tag -2\n"
  "Tag 0,Tag -2\n"
  "Tag 0,Form 0\n"
  "Tag 0,Form 0,Form -1\n"
  "Tag 0,Form -1\n"
  "Tag 0,Form -2\n"
  "Tag 0,PreviousVerbTag 0\n"
  "Tag 0,PreviousVerbForm 0\n"
  "Tag 0,FollowingVerbTag 0\n"
  "Tag 0,FollowingVerbForm 0\n"
  "Tag 0,Lemma -1\n"
  "Tag 0,Form 1\n"
  "Lemma 0\n"
  "Lemma 0,Tag -1\n"
  "Lemma 0,Tag -1,Tag -2\n"
  "Lemma 0,Tag -2\n"
  "Lemma 0,Form -1\n"
  "Lemma 0,Form -1,Form -2\n"
  "Lemma 0,Form -2\n"
  "Lemma 0,PreviousVerbTag 0\n"
  "Lemma 0,PreviousVerbForm 0\n"
  "Lemma 0,FollowingVerbTag 0\n"
  "Lemma 0,FollowingVerbForm 0\n"
  "Lemma 0,Form 1\n"
  "Tag 0,Prefix1 0\n"
  "Tag 0,Prefix2 0\n"
  "Tag 0,Prefix3 0\n"
  "Tag 0,Prefix4 0\n"
  "Tag 0,Prefix5 0\n"
  "Tag 0,Suffix1 0\n"
  "Tag 0,Suffix2 0\n"
  "Tag 0,Suffix3 0\n"
  "Tag 0,Suffix4 0\n"
  "Tag 0,Suffix5 0\n"
  "Tag 0,Num 0\n"
  "Tag 0,Cap 0\n"
  "Tag 0,Dash 0\n";

const string trainer_morphodita_parsito::parser_nodes =
  "stack 0\n"
  "stack 1\n"
  "stack 2\n"
  "buffer 0\n"
  "buffer 1\n"
  "buffer 2\n"
  "stack 0,child 0\n"
  "stack 0,child 1\n"
  "stack 0,child -2\n"
  "stack 0,child -1\n"
  "stack 1,child 0\n"
  "stack 1,child 1\n"
  "stack 1,child -2\n"
  "stack 1,child -1\n"
  "stack 0,child 0,child 0\n"
  "stack 0,child -1,child -1\n"
  "stack 1,child 0,child 0\n"
  "stack 1,child -1,child -1\n";

} // namespace udpipe
} // namespace ufal
