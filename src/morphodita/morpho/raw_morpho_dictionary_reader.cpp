// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "raw_morpho_dictionary_reader.h"
#include "utils/split.h"
#include "trainer/training_failure.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool raw_morpho_dictionary_reader::next_lemma(string& lemma, vector<pair<string, string>>& tagged_forms) {
  if (line.empty()) {
    if (!getline(in, line))
      return false;
    split(line, '\t', tokens);
    if (tokens.size() != 3) training_failure("Line " << line << " does not have three columns!");
  }

  lemma = tokens[0];
  tagged_forms.clear();
  tagged_forms.emplace_back(tokens[2], tokens[1]);
  while (getline(in, line)) {
    split(line, '\t', tokens);
    if (tokens.size() != 3) training_failure("Line " << line << " does not have three columns!");

    if (lemma != tokens[0]) break;
    tagged_forms.emplace_back(tokens[2], tokens[1]);
  }

  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
