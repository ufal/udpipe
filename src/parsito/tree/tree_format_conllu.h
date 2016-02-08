// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <utility>

#include "common.h"
#include "tree_format.h"

namespace ufal {
namespace udpipe {
namespace parsito {

// Input CoNLL-U format
class tree_input_format_conllu : public tree_input_format {
 public:
  virtual bool read_block(istream& in, string& block) const override;
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_tree(tree& t) override;

 private:
  friend class tree_output_format_conllu;
  vector<string_piece> comments;
  vector<pair<int, string_piece>> multiword_tokens;

  string_piece text;
  string text_copy;
};

// Output CoNLL-U format
class tree_output_format_conllu : public tree_output_format {
 public:
  virtual void write_tree(const tree& t, string& output, const tree_input_format* additional_info = nullptr) const override;

 private:
  static const string underscore;
  const string& underscore_on_empty(const string& str) const { return str.empty() ? underscore : str; }
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
