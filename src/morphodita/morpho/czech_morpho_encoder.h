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

class czech_morpho_encoder {
 public:
  static void encode(istream& in_dictionary, int max_suffix_len, istream& in_prefix_guesser, istream& in_statistical_guesser, int max_tag_length, ostream& out_morpho);
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
