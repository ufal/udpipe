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

namespace ufal {
namespace udpipe {
namespace morphodita {

class generic_morpho_encoder {
 public:
  struct tags {
    string unknown_tag, number_tag, punctuation_tag, symbol_tag;
  };
  static void encode(istream& in_dictionary, int max_suffix_len, const tags& tags, istream& in_statistical_guesser, ostream& out_morpho);
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
