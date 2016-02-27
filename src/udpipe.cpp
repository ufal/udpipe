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
#include "utils/iostreams.h"
#include "utils/options.h"
#include "version/version.h"

using namespace ufal::udpipe;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"output", options::value::any},
                       {"parse", options::value::any},
                       {"parser", options::value::any},
                       {"tag", options::value::any},
                       {"tagger", options::value::any},
                       {"tokenize", options::value::any},
                       {"tokenizer", options::value::any},
                       {"train", options::value::none},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " --train [training_options] model_file [input_files]\n"
                    "       " << argv[0] << " [running_options] model_file [input_files]\n"
                    "Running options: --output=output file template\n"
                    "                 --tokenize (perform tokenization)\n"
                    "                 --tokenizer=tokenizer options, implies --tokenize\n"
                    "                 --tag (perform tagging)\n"
                    "                 --tagger=tagger options, implies --tag\n"
                    "                 --parse (perform parsing)\n"
                    "                 --parser=parser options, implies --parse\n"
                    "Training options: --tokenizer=tokenizer options\n"
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
    string error;
    if (!trainer::train(data.str(), options["tokenizer"], options["tagger"], options["parser"], model, error))
      runtime_failure("An error occurred during model training: " << error);
    cerr << "Training done." << endl;
  } else {
    // Load the model
    cerr << "Loading UDPipe model: " << flush;
    unique_ptr<model> model(model::load(argv[1]));
    if (!model) runtime_failure("Cannot load UDPipe model '" << argv[1] << "'!");
    cerr << "done." << endl;

    // Prepare the pipeline
    pipeline pipeline(model.get(), options.count("tokenizer") ? options["tokenizer"] : options.count("tokenize") ? string() : "none",
                      options.count("tagger") ? options["tagger"] : options.count("tag") ? string() : "none",
                      options.count("parser") ? options["parser"] : options.count("parse") ? string() : "none");

    // Process the data
    bool output_per_file = options.count("output") && options["output"].find("{}") != string::npos;
    if (argc == 2 && output_per_file) runtime_failure("Cannot use output template with {} when reading standard input!");
    ofstream output_file;
    string para, error, input_file_root, output_file_name;
    for (int i = 2; i < argc || i == 2; i++) {
      // Open input file
      ifstream input_file;
      if (i < argc) {
        input_file.open(argv[i]);
        if (!input_file.is_open()) runtime_failure("Cannot open input file '" << argv[i] << "'!");

        input_file_root.assign(argv[i]);
        auto directory = input_file_root.find_last_of("/\\");
        if (directory != string :: npos) input_file_root.erase(0, directory + 1);
        auto extension = input_file_root.rfind('.');
        if (extension != string::npos) input_file_root.erase(extension);
      }
      istream& input = i < argc ? input_file : cin;

      // Open output file
      if (options.count("output") && (i == 2 || output_per_file)) {
        output_file_name.assign(options["output"]);
        for (auto index = string::npos; (index = output_file_name.find("{}")) != string::npos; )
          output_file_name.replace(index, 2, input_file_root);
        output_file.open(output_file_name.c_str());
        if (!output_file.is_open()) runtime_failure("Cannot open output file '" << output_file_name << "'!");
      }
      ostream& output = options.count("output") ? output_file : cout;

      // Process the data
      while (getpara(input, para))
        if (!pipeline.process(para, output, error))
          runtime_failure("An error occurred during UDPipe execution: " << error);

      // Close the file if needed
      if (output_per_file) {
        output_file.close();
        if (!output_file) runtime_failure("Cannot close output file '" << output_file_name << "'!");
      }
    }
  }

  return 0;
}
