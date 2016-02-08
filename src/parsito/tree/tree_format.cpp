// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <utility>

#include "tree_format.h"
#include "tree_format_conllu.h"

namespace ufal {
namespace udpipe {
namespace parsito {

const string& tree_input_format::last_error() const {
  return error;
}

// Input Static factory methods
tree_input_format* tree_input_format::new_conllu_input_format() {
  return new tree_input_format_conllu();
}

tree_input_format* tree_input_format::new_input_format(const string& name) {
  if (name == "conllu") return new_conllu_input_format();
  return nullptr;
}

// Output static factory methods
tree_output_format* tree_output_format::new_conllu_output_format() {
  return new tree_output_format_conllu();
}

tree_output_format* tree_output_format::new_output_format(const string& name) {
  if (name == "conllu") return new_conllu_output_format();
  return nullptr;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
