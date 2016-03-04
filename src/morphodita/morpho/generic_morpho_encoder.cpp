// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "generic_lemma_addinfo.h"
#include "generic_morpho_encoder.h"
#include "morpho_dictionary_encoder.h"
#include "morpho_prefix_guesser_encoder.h"
#include "morpho_statistical_guesser_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void generic_morpho_encoder::encode(istream& in_dictionary, int max_suffix_len, const tags& tags, istream& in_statistical_guesser, ostream& out_morpho) {
  binary_encoder enc;

  enc.add_1B(tags.unknown_tag.size());
  enc.add_data(tags.unknown_tag);
  enc.add_1B(tags.number_tag.size());
  enc.add_data(tags.number_tag);
  enc.add_1B(tags.punctuation_tag.size());
  enc.add_data(tags.punctuation_tag);
  enc.add_1B(tags.symbol_tag.size());
  enc.add_data(tags.symbol_tag);

//  cerr << "Encoding dictionary." << endl;
  morpho_dictionary_encoder<generic_lemma_addinfo>::encode(in_dictionary, max_suffix_len, enc);

  // Load and encode statistical guesser if requested
  enc.add_1B(bool(in_statistical_guesser));
  if (in_statistical_guesser) {
//    cerr << "Encoding statistical guesser." << endl;
    morpho_statistical_guesser_encoder::encode(in_statistical_guesser, enc);
  }

  // done, save the dictionary
//  cerr << "Compressing dictionary." << endl;
  if (!compressor::save(out_morpho, enc)) runtime_failure("Cannot compress and write dictionary to file!");
//  cerr << "Dictionary saved." << endl;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
