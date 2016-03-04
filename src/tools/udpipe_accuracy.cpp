// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>

#include "common.h"
#include "model/model.h"
#include "model/pipeline.h"
#include "sentence/input_format.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "version/version.h"

using namespace ufal::udpipe;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"parse", options::value::none},
                       {"parser", options::value::any},
                       {"tag", options::value::none},
                       {"tagger", options::value::any},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] model_file <input_files\n"
                    "Options: --tag (perform tagging)\n"
                    "         --tagger=tagger options, implies --tag\n"
                    "         --parse (perform parsing)\n"
                    "         --parser=parser options, implies --parse\n"
                    "         --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  // Load the model
  cerr << "Loading UDPipe model: " << flush;
  unique_ptr<model> model(model::load(argv[1]));
  if (!model) runtime_failure("Cannot load UDPipe model '" << argv[1] << "'!");
  cerr << "done." << endl;

  bool tag = options.count("tag") || options.count("tagger");
  const string& tagger_options = options["tagger"];
  bool parse = options.count("parse") || options.count("parser");
  const string& parser_options = options["parser"];

  clock_t now = clock();

  // Process the data
  string block, error;
  sentence gold, system;
  unique_ptr<input_format> conllu(input_format::new_conllu_input_format());
  int words = 0, upostag = 0, xpostag = 0, feats = 0, all_tags = 0, lemma = 0;
  int punct = 0, punct_uas = 0, punct_las = 0, nopunct = 0, nopunct_uas = 0, nopunct_las = 0;
  while (conllu->read_block(cin, block)) {
    conllu->set_text(block.c_str());
    while (conllu->next_sentence(gold, error)) {
      // Create empty copy
      system.clear();
      for (size_t i = 1; i < gold.words.size(); i++)
        system.add_word(gold.words[i].form);

      // Tag
      if (tag) {
        if (!model->tag(system, tagger_options, error))
          runtime_failure("An error occurred during tagging: " << error);

        for (size_t i = 1; i < gold.words.size(); i++) {
          words++;
          upostag += gold.words[i].upostag == system.words[i].upostag;
          xpostag += gold.words[i].xpostag == system.words[i].xpostag;
          feats += gold.words[i].feats == system.words[i].feats;
          all_tags += gold.words[i].upostag == system.words[i].upostag && gold.words[i].xpostag == system.words[i].xpostag && gold.words[i].feats == system.words[i].feats;
          lemma += gold.words[i].lemma == system.words[i].lemma;
        }
      } else {
        for (size_t i = 1; i < gold.words.size(); i++) {
          system.words[i].upostag = gold.words[i].upostag;
          system.words[i].xpostag = gold.words[i].xpostag;
          system.words[i].feats = gold.words[i].feats;
          system.words[i].lemma = gold.words[i].lemma;
        }
      }

      // Parse
      if (parse) {
        if (!model->parse(system, parser_options, error))
          runtime_failure("An error occurred during parsing: " << error);

        for (size_t i = 1; i < gold.words.size(); i++) {
          punct++;
          punct_uas += gold.words[i].head == system.words[i].head;
          punct_las += gold.words[i].head == system.words[i].head && gold.words[i].deprel == system.words[i].deprel;
          if (gold.words[i].deprel != "PUNCT") {
            nopunct++;
            nopunct_uas += gold.words[i].head == system.words[i].head;
            nopunct_las += gold.words[i].head == system.words[i].head && gold.words[i].deprel == system.words[i].deprel;
          }
          if (gold.words[i].head >= 0 && gold.words[gold.words[i].head].upostag == "PUNCT")
            cerr << "Word " << gold.words[i].form << " depending on word " << gold.words[i].head << " which is PUNCT" << endl;
        }
      }
    }
    if (!error.empty()) runtime_failure("Cannot parse the CoNLL-U input: " << error);
  }

  cout << "UDPipe finished, in " << fixed << setprecision(3) << (clock() - now) / double(CLOCKS_PER_SEC) << " seconds." << endl;
  if (tag)
    cout << "Tagging - forms: " << words << ", upostag: " << setprecision(2) << 100. * upostag / words << "%, xpostag: " << 100. * xpostag / words
         << "%, feats: " << 100. * feats / words << "%, all tags: " << 100. * all_tags / words << "%, lemma: " << 100. * lemma / words << '%' << endl;
  if (parse)
    cout << "Parsing - UAS: " << 100. * punct_uas / punct << "%, LAS: " << 100. * punct_las / punct << "%, "
         << "without punctuation - UAS: " << 100. * nopunct_uas / nopunct << "%, LAS: " << 100. * nopunct_las / nopunct << '%' << endl;

  return 0;
}
