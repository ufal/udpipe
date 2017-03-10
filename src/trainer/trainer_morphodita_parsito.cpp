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

#include "morphodita/morpho/generic_morpho_encoder.h"
#include "morphodita/morpho/morpho.h"
#include "morphodita/morpho/morpho_ids.h"
#include "morphodita/morpho/morpho_statistical_guesser_trainer.h"
#include "morphodita/tagger/conllu_elementary_features.h"
#include "morphodita/tagger/feature_sequences.h"
#include "morphodita/tagger/perceptron_tagger_trainer.h"
#include "morphodita/tagger/tagger_ids.h"
#include "morphodita/tagger/tagger_trainer.h"
#include "morphodita/tokenizer/generic_tokenizer.h"
#include "morphodita/tokenizer/generic_tokenizer_factory_encoder.h"
#include "morphodita/tokenizer/gru_tokenizer_trainer.h"
#include "morphodita/tokenizer/tokenizer_ids.h"
#include "parsito/parser/parser_nn_trainer.h"
#include "sentence/input_format.h"
#include "sentence/sentence.h"
#include "tokenizer/detokenizer.h"
#include "tokenizer/multiword_splitter_trainer.h"
#include "trainer.h"
#include "trainer_morphodita_parsito.h"
#include "unilib/utf8.h"
#include "utils/options.h"
#include "utils/parse_double.h"
#include "utils/parse_int.h"
#include "utils/split.h"
#include "version/version.h"

namespace ufal {
namespace udpipe {

bool trainer_morphodita_parsito::train(const vector<sentence>& training, const vector<sentence>& heldout,
                                       const string& tokenizer, const string& tagger, const string& parser, ostream& os, string& error) {
  error.clear();

  // Save model version info
  os.put(model_morphodita_parsito::VERSION_LATEST);
  // Add sentinel required since version 2
  os.put(0x7F).put(0x7F);

  // Check input data
  for (auto&& sentence : training)
    for (size_t i = 1; i < sentence.words.size(); i++)
      if (!can_combine_tag(sentence.words[i], error))
        return false;
  for (auto&& sentence : heldout)
    for (size_t i = 1; i < sentence.words.size(); i++)
      if (!can_combine_tag(sentence.words[i], error))
        return false;

  if (!train_tokenizer(training, heldout, tokenizer, os, error)) return false;
  string tagger_model;
  {
    ostringstream os_tagger;
    if (!train_tagger(training, heldout, tagger, os_tagger, error)) return false;
    tagger_model.assign(os_tagger.str());
    os.write(tagger_model.data(), tagger_model.size());
  }
  if (!train_parser(training, heldout, parser, tagger_model, os, error)) return false;

  return true;
}

bool trainer_morphodita_parsito::train_tokenizer(const vector<sentence>& training, const vector<sentence>& heldout,
                                                 const string& options, ostream& os, string& error) {
  if (options == NONE) {
    os.put(0);
  } else {
    // Tokenizer options
    named_values::map tokenizer;
    if (!named_values::parse(options, tokenizer, error)) return false;
    int run = 0; if (!option_int(tokenizer, "run", run, error)) return false;

    if (tokenizer.count("from_model")) {
      // Use specified tokenizer model
      string_piece tokenizer_data;
      if (!load_model(tokenizer["from_model"], TOKENIZER_MODEL, tokenizer_data))
        return error.assign("Cannot load model from which the tokenizer should be used!"), false;

      cerr << "Using tokenizer from given model." << endl;
      os.write(tokenizer_data.str, tokenizer_data.len);
    } else {
      os.put(1);
      const string& model = option_str(tokenizer, "model");

      // Tokenizer itself
      if (model == "generic") {
        os.put(morphodita::tokenizer_id::GENERIC);
        morphodita::generic_tokenizer_factory_encoder::encode(morphodita::generic_tokenizer::LATEST, os);
      } else if (model.empty() || model == "gru") {
        // Create a detokenizator if required
        unique_ptr<detokenizer> detokenizer;
        if (tokenizer.count("detokenize")) {
          detokenizer.reset(new udpipe::detokenizer(tokenizer["detokenize"]));
          if (!detokenizer) return error.assign("Cannot create detokenizer!"), false;
        }

        // Prepare training data for the gru_tokenizer
        vector<morphodita::tokenized_sentence> sentences;
        for (size_t training_sentence = 0; training_sentence < training.size(); training_sentence++) {
          sentence s = training[training_sentence];
          if (detokenizer) detokenizer->detokenize(s);

          auto& sentence = (sentences.emplace_back(), sentences.back());

          for (size_t i = 1, j = 0; i < s.words.size(); i++) {
            const string& form = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? s.multiword_tokens[j].form : s.words[i].form;

            sentence.tokens.emplace_back(sentence.sentence.size(), 0);
            for (auto&& chr : unilib::utf8::decoder(form))
              sentence.sentence.push_back(chr);
            sentence.tokens.back().length = sentence.sentence.size() - sentence.tokens.back().start;

            const string& misc = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? s.multiword_tokens[j].misc : s.words[i].misc;
            if (misc.find(space_after_no) == string::npos) sentence.sentence.push_back(' ');

            if (j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i))
              i = s.multiword_tokens[j++].id_last;
          }
          string doc_par_id;
          if (training_sentence + 1 < training.size() &&
              (training[training_sentence + 1].get_new_doc(doc_par_id) || training[training_sentence + 1].get_new_par(doc_par_id)))
            sentence.sentence.append(2, '\n');
        }

        // Heldout data
        vector<morphodita::tokenized_sentence> heldout_sentences;

        bool detokenize_handout = true; if (!option_bool(tokenizer, "detokenize_handout", detokenize_handout, error)) return false;
        for (size_t heldout_sentence = 0; heldout_sentence < heldout.size(); heldout_sentence++) {
          sentence s = heldout[heldout_sentence];
          if (detokenizer && detokenize_handout) detokenizer->detokenize(s);

          auto& sentence = (heldout_sentences.emplace_back(), heldout_sentences.back());

          for (size_t i = 1, j = 0; i < s.words.size(); i++) {
            const string& form = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? s.multiword_tokens[j].form : s.words[i].form;

            sentence.tokens.emplace_back(sentence.sentence.size(), 0);
            for (auto&& chr : unilib::utf8::decoder(form))
              sentence.sentence.push_back(chr);
            sentence.tokens.back().length = sentence.sentence.size() - sentence.tokens.back().start;

            const string& misc = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? s.multiword_tokens[j].misc : s.words[i].misc;
            if (misc.find(space_after_no) == string::npos) sentence.sentence.push_back(' ');

            if (j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i))
              i = s.multiword_tokens[j++].id_last;
          }
          string doc_par_id;
          if (heldout_sentence + 1 < heldout.size() &&
              (heldout[heldout_sentence + 1].get_new_doc(doc_par_id) || heldout[heldout_sentence + 1].get_new_par(doc_par_id)))
            sentence.sentence.append(2, '\n');
        }

        // Options
        bool tokenize_url = true; if (!option_bool(tokenizer, "tokenize_url", tokenize_url, error)) return false;
        int segment_size = 50; // if (!option_int(tokenizer, "segment_size", segment_size, error)) return false;
        int dimension = 16; // if (!option_int(tokenizer, "dimension", dimension, error)) return false;
        int epochs = 100; if (!option_int(tokenizer, "epochs", epochs, error)) return false;
        int batch_size = run <= 1 ? 50 : 50 + 50 * hyperparameter_integer(run, 1, 0, 1);
        if (!option_int(tokenizer, "batch_size", batch_size, error)) return false;
        double learning_rate = run <= 1 ? 0.005 : hyperparameter_logarithmic(run, 2, 0.0005, 0.01);
        if (!option_double(tokenizer, "learning_rate", learning_rate, error)) return false;
        double learning_rate_final = 0.0; // if (!option_double(tokenizer, "learning_rate_final", learning_rate_final, error)) return false;
        double dropout = 0.1; if (!option_double(tokenizer, "dropout", dropout, error)) return false;
        bool early_stopping = !heldout_sentences.empty(); if (!option_bool(tokenizer, "early_stopping", early_stopping, error)) return false;

        if (run >= 1) cerr << "Random search run " << run << ", batch_size=" << batch_size
                           << ", learning_rate=" << fixed << setprecision(8) << learning_rate << endl;

        // Train and encode gru_tokenizer
        os.put(morphodita::tokenizer_ids::GRU);
        if (!morphodita::gru_tokenizer_trainer::train(tokenize_url ? morphodita::gru_tokenizer_trainer::URL_EMAIL_LATEST : 0,
                                                      segment_size, dimension, epochs, batch_size, learning_rate,
                                                      learning_rate_final, dropout, initialization_range, early_stopping,
                                                      sentences, heldout_sentences, os, error))
          return false;
      } else {
        return error.assign("Unknown tokenizer model '").append(model).append("'!"), false;
      }

      // Multiword splitter
      if (!multiword_splitter_trainer::train(training, os, error)) return false;
    }
  }

  return true;
}

bool trainer_morphodita_parsito::train_tagger(const vector<sentence>& training, const vector<sentence>& heldout,
                                              const string& options, ostream& os, string& error) {
  if (options == NONE) {
    os.put(0);
  } else {
    // Parse options
    named_values::map tagger;
    if (!named_values::parse(options, tagger, error)) return false;

    if (tagger.count("from_model")) {
      // Use specified tokenizer model(s)
      int model_index = 1, taggers_total = 0;
      string model_name = "from_model";
      vector<string_piece> taggers_data;
      do {
        taggers_data.emplace_back();
        if (!load_model(tagger[model_name], TAGGER_MODEL, taggers_data.back()))
          return error.assign("Cannot load model from which the tagger should be used!"), false;
        if (taggers_data.back().str[0])
          taggers_total += taggers_data.back().str[0];
        else
          taggers_data.pop_back();
        model_name = "from_model_" + to_string(++model_index);
      } while (tagger.count(model_name));
      if (taggers_total < 0 || taggers_total > 4) return error.assign("Cannot create more than four tagger models!"), false;

      cerr << "Using tagger from given model(s)." << endl;
      os.put(taggers_total);
      for (auto&& tagger_data : taggers_data)
        os.write(tagger_data.str + 1, tagger_data.len - 1);
    } else {
      // Create MorphoDiTa model(s)
      int models = 1; if (!option_int(tagger, "models", models, error)) return false;
      if (models <= 0) return error.assign("Number of tagger models cannot be negative or zero!"), false;
      if (models > 4) return error.assign("Cannot create more than four tagger models!"), false;

      os.put(models);
      for (int model = 0; model < models; model++)
        if (!train_tagger_model(training, heldout, model, models, tagger, os, error))
          return false;
    }
  }

  return true;
}

bool trainer_morphodita_parsito::train_parser(const vector<sentence>& training, const vector<sentence>& heldout,
                                              const string& options, const string& tagger_model, ostream& os, string& error) {
  if (options == NONE) {
    os.put(0);
  } else {
    // Create Parsito model
    named_values::map parser;
    if (!named_values::parse(options, parser, error)) return false;
    int run = 0; if (!option_int(parser, "run", run, error)) return false;

    if (parser.count("from_model")) {
      // Use specified parser model
      string_piece parser_data;
      if (!load_model(parser["from_model"], PARSER_MODEL, parser_data))
        return error.assign("Cannot load model from which the parser should be used!"), false;

      cerr << "Using parser from given model." << endl;
      os.write(parser_data.str, parser_data.len);
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
      int structured_interval = run <= 1 ? 8 : hyperparameter_integer(run,1,0,2) == 2 ? 0 : 8 + 2*hyperparameter_integer(run,1,0,2);
      if (!option_int(parser, "structured_interval", structured_interval, error)) return false;
      double learning_rate = run <= 1 ? 0.02 : hyperparameter_logarithmic(run, 2, 0.005, 0.04);
      if (!option_double(parser, "learning_rate", learning_rate, error)) return false;
      double learning_rate_final = 0.001; if (!option_double(parser, "learning_rate_final", learning_rate_final, error)) return false;
      double l2 = run <= 1 ? 0.5 : hyperparameter_uniform(run, 3, 0.2, 0.6);
      if (!option_double(parser, "l2", l2, error)) return false;

      if (run >= 1) cerr << "Random search run " << run << ", structured_interval=" << structured_interval
                         << ", learning_rate=" << fixed << setprecision(8) << learning_rate
                         << ", l2=" << l2 << endl;

      // Prepare data in the correct format
      parsito::network_parameters parameters;
      parameters.iterations = iterations;
      parameters.structured_interval = structured_interval;
      parameters.hidden_layer = hidden_layer;
      parameters.hidden_layer_type = parsito::activation_function::TANH;
      parameters.trainer.algorithm = parsito::network_trainer::SGD;
      parameters.trainer.learning_rate = learning_rate;
      parameters.trainer.learning_rate_final = learning_rate_final;
      parameters.trainer.momentum = 0;
      parameters.trainer.epsilon = 0;
      parameters.batch_size = batch_size;
      parameters.initialization_range = 0.1f;
      parameters.l1_regularization = 0;
      parameters.l2_regularization = l2;
      parameters.maxnorm_regularization = 0;
      parameters.dropout_hidden = 0;
      parameters.dropout_input = 0;

      // Tag the input if required
      unique_ptr<model> tagger;
      bool use_gold_tags = false; if (!option_bool(parser, "use_gold_tags", use_gold_tags, error)) return false;
      if (!use_gold_tags && !tagger_model.empty() && tagger_model[0]) {
        stringstream tagger_description;
        tagger_description.put(model_morphodita_parsito::VERSION_LATEST).put(0x7F).put(0x7F).put(0).write(tagger_model.data(), tagger_model.size()).put(0);
        tagger.reset(model_morphodita_parsito::load(tagger_description));
        if (!tagger) return error.assign("Cannot load the tagger model for parser training data generation!"), false;
      }

      // Training data
      sentence tagged;
      vector<parsito::tree> train_trees;
      for (auto&& sentence : training) {
        tagged = sentence;
        if (tagger && !tagger->tag(tagged, DEFAULT, error)) return false;

        train_trees.emplace_back();
        for (size_t i = 1; i < tagged.words.size(); i++) {
          train_trees.back().add_node(string());
          model_normalize_form(tagged.words[i].form, train_trees.back().nodes.back().form, false);
          train_trees.back().nodes.back().lemma.assign(tagged.words[i].lemma);
          train_trees.back().nodes.back().upostag.assign(tagged.words[i].upostag);
          train_trees.back().nodes.back().xpostag.assign(tagged.words[i].xpostag);
          train_trees.back().nodes.back().feats.assign(tagged.words[i].feats);
        }
        for (size_t i = 1; i < tagged.words.size(); i++)
          train_trees.back().set_head(tagged.words[i].id, tagged.words[i].head, tagged.words[i].deprel);
      }

      // Heldout data
      vector<parsito::tree> heldout_trees;
      for (auto&& sentence : heldout) {
        tagged = sentence;
        if (tagger && !tagger->tag(tagged, DEFAULT, error)) return false;

        heldout_trees.emplace_back();
        for (size_t i = 1; i < tagged.words.size(); i++) {
          heldout_trees.back().add_node(string());
          model_normalize_form(tagged.words[i].form, heldout_trees.back().nodes.back().form, false);
          heldout_trees.back().nodes.back().lemma.assign(tagged.words[i].lemma);
          heldout_trees.back().nodes.back().upostag.assign(tagged.words[i].upostag);
          heldout_trees.back().nodes.back().xpostag.assign(tagged.words[i].xpostag);
          heldout_trees.back().nodes.back().feats.assign(tagged.words[i].feats);
        }
        for (size_t i = 1; i < tagged.words.size(); i++)
          heldout_trees.back().set_head(tagged.words[i].id, tagged.words[i].head, tagged.words[i].deprel);
      }

      // Train the parser
      binary_encoder enc;
      enc.add_str("nn");
      parsito::parser_nn_trainer::train(transition_system, transition_oracle, embeddings, parser_nodes,
                                        parameters, 1, train_trees, heldout_trees, enc);
      compressor::save(os, enc);
    }
  }

  return true;
}

bool trainer_morphodita_parsito::load_model(const string& data, model_type model, string_piece& range) {
  istringstream is(data);

  // Check that it is morphodita_parsito model.
  char len;
  if (!is.get(len)) return false;
  string name(len, ' ');
  if (!is.read(&name[0], len)) return false;
  if (name != "morphodita_parsito") return false;

  char version;
  if (!is.get(version)) return false;

  // Tokenizer
  {
    if (model == TOKENIZER_MODEL) range.str = data.data() + is.tellg();
    char tokenizer; if (!is.get(tokenizer)) return false;
    unique_ptr<morphodita::tokenizer_factory> tokenizer_factory(tokenizer ? morphodita::tokenizer_factory::load(is) : nullptr);
    if (tokenizer && !tokenizer_factory) return false;
    if (model == TOKENIZER_MODEL) return range.len = is.tellg() - streampos(range.str - data.data()), true;
  }

  // Tagger
  {
    if (model == TAGGER_MODEL) range.str = data.data() + is.tellg();
    char taggers; if (!is.get(taggers)) return false;
    for (char i = 0; i < taggers; i++) {
      char lemma; if (!is.get(lemma)) return false;
      char xpostag; if (!is.get(xpostag)) return false;
      char feats; if (!is.get(feats)) return false;
      unique_ptr<morphodita::tagger> tagger(morphodita::tagger::load(is));
      if (!tagger) return false;
    }
    if (model == TAGGER_MODEL) return range.len = is.tellg() - streampos(range.str - data.data()), true;
  }

  // Parser
  {
    if (model == PARSER_MODEL) range.str = data.data() + is.tellg();
    char parser;
    if (!is.get(parser)) return false;
    unique_ptr<parsito::parser> parser_model(parser ? parsito::parser::load(is) : nullptr);
    if (parser && !parser_model) return false;
    if (model == PARSER_MODEL) return range.len = is.tellg() - streampos(range.str - data.data()), true;
  }

  return false;
}

const string& trainer_morphodita_parsito::model_normalize_form(string_piece form, string& output, bool also_spaces) {
  return model_morphodita_parsito(model_morphodita_parsito::VERSION_LATEST).normalize_form(form, output, also_spaces);
}

void trainer_morphodita_parsito::model_fill_word_analysis(const morphodita::tagged_lemma& analysis, bool upostag, int lemma, bool xpostag, bool feats, word& word) {
  model_morphodita_parsito(model_morphodita_parsito::VERSION_LATEST).fill_word_analysis(analysis, upostag, lemma, xpostag, feats, word);
}

// Tagger model helper functions

bool trainer_morphodita_parsito::train_tagger_model(const vector<sentence>& training, const vector<sentence>& heldout,
                                                    unsigned model, unsigned models, const named_values::map& tagger,
                                                    ostream& os, string& error) {
  unique_ptr<input_format> conllu_input_format(input_format::new_conllu_input_format());

  int run = 0; if (!option_int(tagger, "run", run, error, model)) return false;

  bool have_lemma = false;
  for (auto&& sentence : training)
    for (size_t i = 1; !have_lemma && i < sentence.words.size(); i++)
      if (!sentence.words[i].lemma.empty() && sentence.words[i].lemma != "_")
        have_lemma = true;
  bool use_lemma_flag = model == 1 || models == 1; if (!option_bool(tagger, "use_lemma", use_lemma_flag, error, model)) return false;
  int lemma_encoding = 2; if (!option_int(tagger, "dictionary_lemma_encoding", lemma_encoding, error, model)) return false;
  int use_lemma = have_lemma && use_lemma_flag ? lemma_encoding : 0;
  bool use_xpostag = model == 0; if (!option_bool(tagger, "use_xpostag", use_xpostag, error, model)) return false;
  bool use_feats = model == 0; if (!option_bool(tagger, "use_feats", use_feats, error, model)) return false;

  bool provide_lemma = model == 1 || models == 1; if (!option_bool(tagger, "provide_lemma", provide_lemma, error, model)) return false;
  bool provide_xpostag = model == 0; if (!option_bool(tagger, "provide_xpostag", provide_xpostag, error, model)) return false;
  bool provide_feats = model == 0; if (!option_bool(tagger, "provide_feats", provide_feats, error, model)) return false;
  os.put(char(provide_lemma ? use_lemma : 0));
  os.put(char(provide_xpostag && use_xpostag));
  os.put(char(provide_feats && use_feats));

  // Start by creating the morphological dictionary
  stringstream morpho_description;
  string normalized_form, combined_tag, combined_lemma;

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
    int guesser_suffix_rules = run <= 1 ? 8 : 5 + hyperparameter_integer(run, 1, 0, 7);
    if (!option_int(tagger, "guesser_suffix_rules", guesser_suffix_rules, error, model)) return false;
    int guesser_prefixes_max = provide_lemma ? 4 : 0; if (!option_int(tagger, "guesser_prefixes_max", guesser_prefixes_max, error, model)) return false;
    int guesser_prefix_min_count = 10; if (!option_int(tagger, "guesser_prefix_min_count", guesser_prefix_min_count, error, model)) return false;
    int guesser_enrich_dictionary = run <= 1 ? 6 : 3 + hyperparameter_integer(run, 2, 0, 7);
    if (!option_int(tagger, "guesser_enrich_dictionary", guesser_enrich_dictionary, error, model)) return false;

    if (run >= 1) cerr << "Random search run " << run << ", guesser_suffix_rules=" << guesser_suffix_rules
                       << ", guesser_enrich_dictionary=" << guesser_enrich_dictionary << endl;

    // Dictionary options
    int dictionary_suffix_len = 8; if (!option_int(tagger, "dictionary_suffix_len", dictionary_suffix_len, error, model)) return false;
    unordered_set<string> flat_lemmas;
    if (!option_str(tagger, "dictionary_flat_lemmas", model).empty()) {
      vector<string> lemmas;
      split(option_str(tagger, "dictionary_flat_lemmas", model), ',', lemmas);
      for (auto&& lemma : lemmas) {
        if (lemma.find('~') != string::npos)
          return error.assign("Dictionary_flat_lemmas cannot contain '~' character!"), false;
        flat_lemmas.insert(lemma);
      }
    } else {
      flat_lemmas.insert("greek.expression");
    }

    // Start by generating statistical guesser
    stringstream guesser_description;
    {
      stringstream guesser_input;
      for (auto&& sentence : training) {
        for (size_t i = 1; i < sentence.words.size(); i++)
          guesser_input << model_normalize_form(sentence.words[i].form, normalized_form, true) << '\t'
              << combine_lemma(sentence.words[i], use_lemma, combined_lemma, flat_lemmas) << '\t'
              << combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag) << '\n';
        guesser_input << '\n';
      }
      morphodita::morpho_statistical_guesser_trainer::train(guesser_input, guesser_suffix_len, guesser_suffix_rules, guesser_prefixes_max, guesser_prefix_min_count, guesser_description);
    }

    // Generate morphological dictionary data from the input
    int max_form_analyses = 0; if (!option_int(tagger, "dictionary_max_form_analyses", max_form_analyses, error, model)) return false;

    unordered_set<string> dictionary_entries;
    {
      unordered_map<string, unordered_map<string, int>> entries;
      string entry;
      for (auto&& sentence : training)
        for (size_t i = 1; i < sentence.words.size(); i++) {
          model_normalize_form(sentence.words[i].form, normalized_form, true);
          entry.assign(combine_lemma(sentence.words[i], use_lemma, combined_lemma, flat_lemmas))
              .append("\t").append(combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag))
              .append("\t").append(normalized_form);
          entries[normalized_form][entry]++;
        }

      vector<pair<int, string>> analyses;
      for (auto&& form_analyses : entries) {
        analyses.clear();
        for (auto&& analysis : form_analyses.second)
          analyses.emplace_back(analysis.second, analysis.first);
        if (max_form_analyses && int(analyses.size()) > max_form_analyses) {
          sort(analyses.begin(), analyses.end(), greater<pair<int, string>>());
          analyses.resize(max_form_analyses);
        }
        for (auto&& analysis : analyses)
          dictionary_entries.insert(analysis.second);
      }
    }

    morphodita::generic_morpho_encoder::tags dictionary_special_tags;
    dictionary_special_tags.unknown_tag = "~X";
    dictionary_special_tags.number_tag = most_frequent_tag(training, "NUM", use_xpostag, use_feats, combined_tag);
    dictionary_special_tags.punctuation_tag = most_frequent_tag(training, "PUNCT", use_xpostag, use_feats, combined_tag);
    dictionary_special_tags.symbol_tag = most_frequent_tag(training, "SYM", use_xpostag, use_feats, combined_tag);

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
      for (auto&& sentence : training)
        for (size_t i = 1; i < sentence.words.size(); i++) {
          const auto& form = model_normalize_form(sentence.words[i].form, normalized_form, true);
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
        morpho->analyze(model_normalize_form(sentence.words[i].form, normalized_form, true), morphodita::morpho::GUESSER, analyses);
        unsigned upostag_ok = 0, xpostag_ok = 0, feats_ok = 0, all_tags_ok = 0, lemma_ok = 0;
        for (auto&& analysis : analyses) {
          w.lemma.assign("_");
          model_fill_word_analysis(analysis, true, use_lemma, true, true, w);
          upostag_ok |= int(sentence.words[i].upostag == w.upostag);
          xpostag_ok |= int(sentence.words[i].xpostag == w.xpostag);
          feats_ok |= int(sentence.words[i].feats == w.feats);
          all_tags_ok |= int(sentence.words[i].upostag == w.upostag && sentence.words[i].xpostag == w.xpostag && sentence.words[i].feats == w.feats);
          lemma_ok |= int(sentence.words[i].lemma == w.lemma);
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
  if (heldout.empty()) tagger_early_stopping = false;

  // Train the tagger
  cerr << "Training tagger model " << model+1 << "." << endl;
  stringstream input, heldout_input, feature_templates_input(tagger_feature_templates);
  for (auto&& sentence : training) {
    for (size_t i = 1; i < sentence.words.size(); i++)
      input << model_normalize_form(sentence.words[i].form, normalized_form, true) << '\t'
          << combine_lemma(sentence.words[i], use_lemma, combined_lemma) << '\t'
          << combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag) << '\n';
    input << '\n';
  }

  for (auto&& sentence : heldout) {
    for (size_t i = 1; i < sentence.words.size(); i++)
      heldout_input << model_normalize_form(sentence.words[i].form, normalized_form, true) << '\t'
          << combine_lemma(sentence.words[i], use_lemma, combined_lemma) << '\t'
          << combine_tag(sentence.words[i], use_xpostag, use_feats, combined_tag) << '\n';
    heldout_input << '\n';
  }

  os.put(tagger_id);
  morphodita::tagger_trainer<morphodita::perceptron_tagger_trainer<morphodita::train_feature_sequences<morphodita::conllu_elementary_features>>>::train(morphodita::tagger_ids::decoding_order(tagger_id), morphodita::tagger_ids::window_size(tagger_id), tagger_iterations, morpho_description, true, feature_templates_input, tagger_prune_features, input, heldout_input, tagger_early_stopping, os);

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
    // Should not happen, as can_combine_tag was called before
    separator = 0;

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

const string& trainer_morphodita_parsito::most_frequent_tag(const vector<sentence>& data, const string& upostag, bool xpostag, bool feats, string& combined_tag) {
  unordered_map<string, unsigned> counts;

  for (auto&& sentence : data)
    for (size_t i = 1; i < sentence.words.size(); i++)
      if (sentence.words[i].upostag == upostag)
        counts[combine_tag(sentence.words[i], xpostag, feats, combined_tag)]++;

  combined_tag.assign("~").append(upostag);
  unsigned best = 0;
  for (auto&& tags : counts)
    if (tags.second > best) {
      best = tags.second;
      combined_tag.assign(tags.first);
    }
  return combined_tag;
}

const string& trainer_morphodita_parsito::combine_lemma(const word& w, int use_lemma, string& combined_lemma, const unordered_set<string>& flat_lemmas) {
  switch (use_lemma) {
    case 0:
      return model_normalize_form(w.form, combined_lemma, true);
    case 1:
      if (flat_lemmas.count(w.lemma))
        return model_normalize_form(w.form, combined_lemma, true);
      else
        return combined_lemma.assign(w.lemma), replace(combined_lemma.begin(), combined_lemma.end(), ' ', '\001'), combined_lemma;
    default: /*2*/
      if (w.lemma == "")
        return model_normalize_form(w.form, combined_lemma, true), combined_lemma.insert(0, "~~");
      else if (w.lemma == "_")
        return model_normalize_form(w.form, combined_lemma, true), combined_lemma.insert(0, "~_~");
      else if (flat_lemmas.count(w.lemma))
        return model_normalize_form(w.form, combined_lemma, true), combined_lemma.insert(0, "~").insert(0, w.lemma).insert(0, "~"),
               replace(combined_lemma.begin(), combined_lemma.end(), ' ', '\001'), combined_lemma;
      else
        return combined_lemma.assign(w.lemma), replace(combined_lemma.begin(), combined_lemma.end(), ' ', '\001'), combined_lemma;
  }
}

// Generic options handling

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

// Various string data

const string trainer_morphodita_parsito::empty_string;

const string trainer_morphodita_parsito::space_after_no = "SpaceAfter=No";

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
