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
#include "morphodita/morpho/morpho.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class tagger {
 public:
  virtual ~tagger() {}

  static tagger* load(const char* fname);
  static tagger* load(istream& is);

  // Return morpho associated with the tagger. Do not delete the pointer, it is
  // owned by the tagger instance and deleted in the tagger destructor.
  virtual const morpho* get_morpho() const = 0;

  // Perform morphologic analysis and subsequent disambiguation.
  virtual void tag(const vector<string_piece>& forms,
                   vector<tagged_lemma>& tags,
                   vector<string>& all_analyzes,
                   bool provide_all_analyzes,
                   morpho::guesser_mode guesser = morpho::GUESSER_UNSPECIFIED) const = 0;

  // Perform disambiguation only on given analyses.
  virtual void tag_analyzed(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<int>& tags) const = 0;

  // Construct a new tokenizer instance appropriate for this tagger.
  // Can return NULL if no such tokenizer exists.
  // Is equal to get_morpho()->new_tokenizer.
  tokenizer* new_tokenizer() const;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
