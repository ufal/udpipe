// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <sstream>

#include "common.h"
#include "model/evaluator.h"
#include "model/model.h"
#include "model/pipeline.h"
#include "sentence/input_format.h"
#include "sentence/output_format.h"
#include "tokenizer/detokenizer.h"
#include "trainer/trainer.h"
#include "utils/getpara.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/process_args.h"
#include "version/version.h"

using namespace ufal::udpipe;

bool append_conllu(istream& is, vector<sentence>& sentences, string& error) {
  unique_ptr<input_format> conllu_input(input_format::new_conllu_input_format());

  string block;
  while (conllu_input->read_block(is, block)) {
    conllu_input->set_text(block);
    while (sentences.emplace_back(), conllu_input->next_sentence(sentences.back(), error)) ;
    sentences.pop_back();
    if (!error.empty()) return false;
  }
  return true;
}

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"accuracy", options::value::none},
                       {"detokenize", options::value::none},
                       {"method", options::value{"morphodita_parsito"}},
                       {"heldout", options::value::any},
                       {"input", options::value::any},
                       {"immediate", options::value::none},
                       {"outfile", options::value::any},
                       {"output", options::value::any},
                       {"parse", options::value::none},
                       {"parser", options::value::any},
                       {"tag", options::value::none},
                       {"tagger", options::value::any},
                       {"tokenize", options::value::none},
                       {"tokenizer", options::value::any},
                       {"train", options::value::none},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [running_opts] model_file [input_files]\n"
                    "       " << argv[0] << " --train [training_opts] model_file [input_files]\n"
                    "       " << argv[0] << " --detokenize [detokenize_opts] raw_text_file [input_files]\n"
                    "Running opts: --accuracy (measure accuracy only)\n"
                    "              --input=[conllu|generic_tokenizer|horizontal|vertical]\n"
                    "              --immediate (process sentences immediately during loading)\n"
                    "              --outfile=output file template\n"
                    "              --output=[conllu|matxin|horizontal|plaintext|vertical]\n"
                    "              --tokenize (perform tokenization)\n"
                    "              --tokenizer=tokenizer options, implies --tokenize\n"
                    "              --tag (perform tagging)\n"
                    "              --tagger=tagger options, implies --tag\n"
                    "              --parse (perform parsing)\n"
                    "              --parser=parser options, implies --parse\n"
                    "Training opts: --method=[morphodita_parsito] which method to use\n"
                    "               --heldout=heldout data file name\n"
                    "               --tokenizer=tokenizer options\n"
                    "               --tagger=tagger options\n"
                    "               --parser=parser options\n"
                    "Detokenize opts: --outfile=output file template\n"
                    "Generic opts: --version\n"
                    "              --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  // TRAINING
  if (options.count("train")) {
    string error;

    // Load training data
    cerr << "Loading training data: " << flush;
    vector<sentence> training;
    for (int i = 2; i < argc; i++) {
      ifstream input(argv[i]);
      if (!input.is_open()) runtime_failure("Cannot open input file '" << argv[i] << "'!");
      if (!append_conllu(input, training, error))
        runtime_failure("Cannot load training data from file '" << argv[i] << "': " << error);
    }
    if (argc == 2 && !append_conllu(cin, training, error)) runtime_failure("Cannot load training data: " << error);
    cerr << "done." << endl;

    // Load heldout data
    vector<sentence> heldout;
    if (options.count("heldout")) {
      ifstream input(options["heldout"]);
      if (!input.is_open()) runtime_failure("Cannot open heldout data file '" << options["heldout"] << "'!");
      if (!append_conllu(input, heldout, error))
        runtime_failure("Cannot load heldout data from file '" << options["heldout"] << "': " << error);
    }

    // Open output file
    ofstream model(argv[1], ofstream::binary);
    if (!model.is_open()) runtime_failure("Cannot open model file '" << argv[1] << "' for writing.");

    // Train the model
    cerr << "Training the UDPipe model." << endl;
    string method = options.count("method") ? options["method"] : "morphodita_parsito";
    if (!trainer::train(method, training, heldout, options["tokenizer"], options["tagger"], options["parser"], model, error))
      runtime_failure("An error occurred during model training: " << error);
    cerr << "The trained UDPipe model was saved." << endl;
  } else
  // DETOKENIZE
  if (options.count("detokenize")) {
    // Detokenize CoNLL-U files
    ifstream raw_text_file(argv[1]);
    if (!raw_text_file.is_open()) runtime_failure("Cannot load raw text from file '" << argv[1] << "'.");

    string raw_text;
    for (char c; raw_text_file.get(c); )
      raw_text.push_back(c);
    raw_text_file.close();

    detokenizer detokenizer(raw_text);
    process_args_with_output_template(2, argc, argv, options["outfile"], [&detokenizer](istream& is, ostream& os, string, string) {
      unique_ptr<input_format> conllu_input(input_format::new_conllu_input_format());
      unique_ptr<output_format> conllu_output(output_format::new_conllu_output_format());

      sentence s;
      string block, error;
      while (conllu_input->read_block(is, block)) {
        conllu_input->set_text(block);
        while (conllu_input->next_sentence(s, error)) {
          detokenizer.detokenize(s);
          conllu_output->write_sentence(s, os);
        }
        if (!error.empty()) runtime_failure("An error occurred during UDPipe execution: " << error);
      }
      conllu_output->finish_document(os);
    });
  } else
  // RUN
  {
    // Load the model if needed
    unique_ptr<model> model;
    if (options.count("tokenizer") || options.count("tokenize") ||
        (options.count("input") && options["input"].compare(0, 8, "tokenize") == 0) ||
        options.count("tagger") || options.count("tag") ||
        options.count("parser") || options.count("parse")) {
      cerr << "Loading UDPipe model: " << flush;
      model.reset(model::load(argv[1]));
      if (!model) runtime_failure("Cannot load UDPipe model '" << argv[1] << "'!");
      cerr << "done." << endl;
    }

    if (options.count("accuracy")) {
      if (options.count("input")) runtime_failure("The --input option is unsupported when --accuracy is used!");
      if (options.count("output")) runtime_failure("The --output option is unsupported when --accuracy is used!");

      // Prepare the evaluator
      evaluator evaluator(model.get(), options.count("tokenizer") ? options["tokenizer"] : options.count("tokenize") ? pipeline::DEFAULT : pipeline::NONE,
                          options.count("tagger") ? options["tagger"] : options.count("tag") ? pipeline::DEFAULT : pipeline::NONE,
                          options.count("parser") ? options["parser"] : options.count("parse") ? pipeline::DEFAULT : pipeline::NONE);

      // Process the data
      process_args_with_output_template(2, argc, argv, options["outfile"], [&evaluator](istream& is, ostream& os, string, string) {
        string error;
        if (!evaluator.evaluate(is, os, error))
            runtime_failure("An error occurred during UDPipe execution: " << error);
      });
    } else {
      // Prepare the pipeline
      pipeline pipeline(model.get(), options.count("tokenizer") ? "tokenizer=" + options["tokenizer"] : options.count("tokenize") ? "tokenizer" : options.count("input") ? options["input"] : "conllu",
                        options.count("tagger") ? options["tagger"] : options.count("tag") ? pipeline::DEFAULT : pipeline::NONE,
                        options.count("parser") ? options["parser"] : options.count("parse") ? pipeline::DEFAULT : pipeline::NONE,
                        options.count("output") ? options["output"] : "conllu");
      pipeline.set_immediate(options.count("immediate"));

      // Process the data
      process_args_with_output_template(2, argc, argv, options["outfile"], [&pipeline](istream& is, ostream& os, string is_name, string) {
        string error;
        pipeline.set_document_id(is_name);
        if (!pipeline.process(is, os, error))
          runtime_failure("An error occurred during UDPipe execution: " << error);
      });
    }
  }

  return 0;
}
