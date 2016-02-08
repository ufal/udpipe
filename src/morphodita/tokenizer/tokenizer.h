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
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Range of a token, measured in Unicode characters, not UTF8 bytes.
struct token_range {
  size_t start;
  size_t length;

  token_range() {}
  token_range(size_t start, size_t length) : start(start), length(length) {}
};

class tokenizer {
 public:
  virtual ~tokenizer() {}

  virtual void set_text(string_piece text, bool make_copy = false) = 0;
  virtual bool next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) = 0;

  // Static factory methods
  static tokenizer* new_vertical_tokenizer();

  static tokenizer* new_czech_tokenizer();
  static tokenizer* new_english_tokenizer();
  static tokenizer* new_generic_tokenizer();
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
