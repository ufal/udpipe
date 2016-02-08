// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <utility>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class raw_morpho_dictionary_reader {
 public:
  raw_morpho_dictionary_reader(istream& in) : in(in) {}
  bool next_lemma(string& lemma, vector<pair<string, string>>& tagged_forms);
 private:
  istream& in;
  string line;
  vector<string> tokens;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
