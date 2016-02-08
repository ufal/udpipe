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

class tagset_converter {
 public:
  virtual ~tagset_converter() {}

  // Convert a tag-lemma pair to a different tag set.
  virtual void convert(tagged_lemma& tagged_lemma) const = 0;
  // Convert a result of analysis to a different tag set. Apart from calling
  // convert, any repeated entry is removed.
  virtual void convert_analyzed(vector<tagged_lemma>& tagged_lemmas) const = 0;
  // Convert a result of generation to a different tag set. Apart from calling
  // convert, any repeated entry is removed.
  virtual void convert_generated(vector<tagged_lemma_forms>& forms) const = 0;

  // Static factory methods
  static tagset_converter* new_identity_converter();

  static tagset_converter* new_pdt_to_conll2009_converter();
  static tagset_converter* new_strip_lemma_comment_converter(const morpho& dictionary);
  static tagset_converter* new_strip_lemma_id_converter(const morpho& dictionary);
};

// Helper method for creating tagset_converter from instance name.
tagset_converter* new_tagset_converter(const string& name, const morpho& dictionary);

// Helper methods making sure remapped results are unique.
void tagset_converter_unique_analyzed(vector<tagged_lemma>& tagged_lemmas);
void tagset_converter_unique_generated(vector<tagged_lemma_forms>& forms);

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
