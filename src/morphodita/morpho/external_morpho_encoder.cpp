// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "external_morpho_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"
#include "trainer/training_failure.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void external_morpho_encoder::encode(const string& unknown_tag, ostream& out_morpho) {
  binary_encoder enc;

  // Save unknown_tag
  enc.add_1B(unknown_tag.size());
  enc.add_data(unknown_tag);

  if (!compressor::save(out_morpho, enc)) training_failure("Cannot compress and write dictionary to file!");
//  cerr << "Dictionary saved." << endl;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
