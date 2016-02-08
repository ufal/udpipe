// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "tree.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {
namespace parsito {

// Input format
class tree_input_format {
 public:
  virtual ~tree_input_format() {}

  virtual bool read_block(istream& in, string& block) const = 0;
  virtual void set_text(string_piece text, bool make_copy = false) = 0;
  virtual bool next_tree(tree& t) = 0;
  const string& last_error() const;

  // Static factory methods
  static tree_input_format* new_input_format(const string& name);
  static tree_input_format* new_conllu_input_format();

 protected:
  string error;
};

// Output format
class tree_output_format {
 public:
  virtual ~tree_output_format() {}

  virtual void write_tree(const tree& t, string& output, const tree_input_format* additional_info = nullptr) const = 0;

  // Static factory methods
  static tree_output_format* new_output_format(const string& name);
  static tree_output_format* new_conllu_output_format();
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
