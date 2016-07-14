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
                       {"method", options::value{"morphodita_parsito"}},
                       {"heldout", options::value::any},
                       {"input", options::value::any},
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
    runtime_failure("Usage: " << argv[0] << " --train [training_options] model_file [input_files]\n"
                    "       " << argv[0] << " [running_options] model_file [input_files]\n"
                    "Running options: --accuracy (measure accuracy only)\n"
                    "                 --input=[conllu|horizontal|vertical]\n"
                    "                 --outfile=output file template\n"
                    "                 --output=[conllu|matxin|horizontal|vertical]\n"
                    "                 --tokenize (perform tokenization)\n"
                    "                 --tokenizer=tokenizer options, implies --tokenize\n"
                    "                 --tag (perform tagging)\n"
                    "                 --tagger=tagger options, implies --tag\n"
                    "                 --parse (perform parsing)\n"
                    "                 --parser=parser options, implies --parse\n"
                    "Training options: --method=[morphodita_parsito] which method to use\n"
                    "                  --heldout=heldout data file name\n"
                    "                  --tokenizer=tokenizer options\n"
                    "                  --tagger=tagger options\n"
                    "                  --parser=parser options\n"
                    "Generic options: --version\n"
                    "                 --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

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
  } else {
    // Load the model
    cerr << "Loading UDPipe model: " << flush;
    unique_ptr<model> model(model::load(argv[1]));
    if (!model) runtime_failure("Cannot load UDPipe model '" << argv[1] << "'!");
    cerr << "done." << endl;

    if (options.count("accuracy")) {
      if (options.count("input")) runtime_failure("The --input option is unsupported when --accuracy is used!");
      if (options.count("output")) runtime_failure("The --output option is unsupported when --accuracy is used!");

      // Prepare the evaluator
      evaluator evaluator(model.get(), options.count("tokenizer") ? options["tokenizer"] : options.count("tokenize") ? pipeline::DEFAULT : pipeline::NONE,
                          options.count("tagger") ? options["tagger"] : options.count("tag") ? pipeline::DEFAULT : pipeline::NONE,
                          options.count("parser") ? options["parser"] : options.count("parse") ? pipeline::DEFAULT : pipeline::NONE);

      // Process the data
      process_args_with_output_template(2, argc, argv, options["outfile"], [&evaluator](istream& is, ostream& os) {
        string error;
        if (!evaluator.evaluate(is, os, error))
            runtime_failure("An error occurred during UDPipe execution: " << error);
      });
    } else {
      // Prepare the pipeline
      if(options["output"] == "matxin") {
        cout << "<corpus>" << '\n';
      }
      pipeline pipeline(model.get(), options.count("tokenizer") ? "tokenizer=" + options["tokenizer"] : options.count("tokenize") ? "tokenizer" : options.count("input") ? options["input"] : "conllu",
                        options.count("tagger") ? options["tagger"] : options.count("tag") ? pipeline::DEFAULT : pipeline::NONE,
                        options.count("parser") ? options["parser"] : options.count("parse") ? pipeline::DEFAULT : pipeline::NONE,
                        options.count("output") ? options["output"] : "conllu");

      // Process the data
      process_args_with_output_template(2, argc, argv, options["outfile"], [&pipeline](istream& is, ostream& os) {
        string error;
        if (!pipeline.process(is, os, error))
          runtime_failure("An error occurred during UDPipe execution: " << error);
      });
      if(options["output"] == "matxin") {
        cout << "</corpus>" << '\n';
      }
    }
  }

  return 0;
}
