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
#include "model/model.h"
#include "model/pipeline.h"
#include "trainer/trainer.h"
#include "utils/getpara.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/process_args.h"
#include "version/version.h"

using namespace ufal::udpipe;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"accuracy", options::value::none},
                       {"method", options::value{"morphodita_parsito"}},
                       {"input_format", options::value::any},
                       {"output", options::value::any},
                       {"output_format", options::value::any},
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
                    "                 --input_format=[conllu|horizontal|vertical]\n"
                    "                 --output=output file template\n"
                    "                 --output_format=[conllu|horizontal|vertical]\n"
                    "                 --tokenize (perform tokenization)\n"
                    "                 --tokenizer=tokenizer options, implies --tokenize\n"
                    "                 --tag (perform tagging)\n"
                    "                 --tagger=tagger options, implies --tag\n"
                    "                 --parse (perform parsing)\n"
                    "                 --parser=parser options, implies --parse\n"
                    "Training options: --method=[morphodita_parsito] which method to use\n"
                    "                  --tokenizer=tokenizer options\n"
                    "                  --tagger=tagger options\n"
                    "                  --parser=parser options\n"
                    "Generic options: --version\n"
                    "                 --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  if (options.count("train")) {
    // Load all data
    cerr << "Loading training data: " << flush;
    ostringstream data;
    for (int i = 2; i < argc; i++) {
      ifstream input(argv[i]);
      if (!input.is_open()) runtime_failure("Cannot open input file '" << argv[i] << "'!");
      data << input.rdbuf() << '\n';
    }
    if (argc == 2) data << cin.rdbuf();
    cerr << "done." << endl;

    // Open output file
    ofstream model(argv[1], ofstream::binary);
    if (!model.is_open()) runtime_failure("Cannot open model file '" << argv[1] << "' for writing.");

    // Train the model
    cerr << "Training the UDPipe model." << endl;
    string method = options.count("method") ? options["method"] : "morphodita_parsito";
    string error;
    if (!trainer::train(method, data.str(), options["tokenizer"], options["tagger"], options["parser"], model, error))
      runtime_failure("An error occurred during model training: " << error);
    cerr << "The trained UDPipe model was saved." << endl;
  } else {
    // Load the model
    cerr << "Loading UDPipe model: " << flush;
    unique_ptr<model> model(model::load(argv[1]));
    if (!model) runtime_failure("Cannot load UDPipe model '" << argv[1] << "'!");
    cerr << "done." << endl;

    string input_format =
        options.count("tokenizer") ? "tokenizer=" + options["tokenizer"] :
        options.count("tokenize") ? "tokenizer" :
        options.count("input_format") ? options["input_format"] : "conllu";

    string output_format = options.count("output_format") ? options["output_format"] : "conllu";

    // Prepare the pipeline
    pipeline pipeline(model.get(), input_format,
                      options.count("tagger") ? options["tagger"] : options.count("tag") ? string() : "none",
                      options.count("parser") ? options["parser"] : options.count("parse") ? string() : "none",
                      output_format);

    // Process the data
    if (options.count("accuracy"))
      process_args_with_output_template(2, argc, argv, options["output"], [&pipeline](istream& is, ostream& os) {
        string error;
        if (!pipeline.evaluate(is, os, error))
            runtime_failure("An error occurred during UDPipe execution: " << error);
      });
    else
      process_args_with_output_template(2, argc, argv, options["output"], [&pipeline](istream& is, ostream& os) {
        string error;
        if (!pipeline.process(is, os, error))
          runtime_failure("An error occurred during UDPipe execution: " << error);
      });
  }

  return 0;
}
