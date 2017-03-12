// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "parser.h"
#include "parser_nn.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace parsito {

parser* parser::load(const char* file, unsigned cache) {
  ifstream in(file, ifstream::in | ifstream::binary);
  if (!in.is_open()) return nullptr;
  return load(in, cache);
}

parser* parser::load(istream& in, unsigned cache) {
  unique_ptr<parser> result;

  binary_decoder data;
  if (!compressor::load(in, data)) return nullptr;

  try {
    string name;
    data.next_str(name);

    result.reset(create(name));
    if (!result) return nullptr;

    result->load(data, cache);
  } catch (binary_decoder_error&) {
    return nullptr;
  }

  return result && data.is_end() ? result.release() : nullptr;
}

parser* parser::create(const string& name) {
  if (name == "nn") return new parser_nn(false);
  if (name == "nn_versioned") return new parser_nn(true);
  return nullptr;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
