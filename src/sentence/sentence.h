// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "empty_node.h"
#include "multiword_token.h"
#include "word.h"

namespace ufal {
namespace udpipe {

class sentence {
 public:
  sentence();

  vector<word> words;
  vector<multiword_token> multiword_tokens;
  vector<empty_node> empty_nodes;
  vector<string> comments;
  static const string root_form;

  // Basic sentence modifications
  bool empty();
  void clear();
  word& add_word(string_piece form = string_piece());
  void set_head(int id, int head, const string& deprel);
  void unlink_all_words();

  // CoNLL-U defined comments
  bool get_new_doc(string* id = nullptr) const;
  void set_new_doc(bool new_doc, string_piece id = string_piece());
  bool get_new_par(string* id = nullptr) const;
  void set_new_par(bool new_par, string_piece id = string_piece());
  bool get_sent_id(string& id) const;
  void set_sent_id(string_piece id);
  bool get_text(string& text) const;
  void set_text(string_piece text);

 private:
  bool get_comment(string_piece name, string* value) const;
  void remove_comment(string_piece name);
  void set_comment(string_piece name, string_piece value = string_piece());
};

} // namespace udpipe
} // namespace ufal
