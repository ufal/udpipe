// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "morpho.h"
#include "persistent_unordered_map.h"
#include "utils/binary_decoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class morpho_statistical_guesser {
 public:
  void load(binary_decoder& data);
  typedef vector<string> used_rules;
  void analyze(string_piece form, vector<tagged_lemma>& lemmas, used_rules* used);

 private:
  vector<string> tags;
  unsigned default_tag;
  persistent_unordered_map rules;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
