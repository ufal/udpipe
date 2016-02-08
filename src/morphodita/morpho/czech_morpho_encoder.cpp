// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "czech_lemma_addinfo.h"
#include "czech_morpho_encoder.h"
#include "morpho_dictionary_encoder.h"
#include "morpho_prefix_guesser_encoder.h"
#include "morpho_statistical_guesser_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void czech_morpho_encoder::encode(istream& in_dictionary, int max_suffix_len, istream& in_prefix_guesser, istream& in_statistical_guesser, int max_tag_length, ostream& out_morpho) {
  binary_encoder enc;

  enc.add_1B(max_tag_length);

  cerr << "Encoding dictionary." << endl;
  morpho_dictionary_encoder<czech_lemma_addinfo>::encode(in_dictionary, max_suffix_len, enc);

  // Load and encode prefix guesser if requested
  enc.add_1B(bool(in_prefix_guesser));
  if (in_prefix_guesser) {
    cerr << "Encoding prefix guesser." << endl;
    morpho_prefix_guesser_encoder::encode(in_prefix_guesser, enc);
  }

  // Load and encode statistical guesser if requested
  enc.add_1B(bool(in_statistical_guesser));
  if (in_statistical_guesser) {
    cerr << "Encoding statistical guesser." << endl;
    morpho_statistical_guesser_encoder::encode(in_statistical_guesser, enc);
  }

  // done, save the dictionary
  cerr << "Compressing dictionary." << endl;
  if (!compressor::save(out_morpho, enc)) runtime_failure("Cannot compress and write dictionary to file!");
  cerr << "Dictionary saved." << endl;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
