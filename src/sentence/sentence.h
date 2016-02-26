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
#include "multiword_token.h"
#include "word.h"

namespace ufal {
namespace udpipe {

class sentence {
 public:
  sentence();

  vector<word> words;
  vector<multiword_token> multiword_tokens;
  vector<string> comments;
  static const string root_form;

  bool empty();
  void clear();
  word& add_word(const string& form);
  void set_head(int id, int head, const string& deprel);
  void unlink_all_words();
};

} // namespace udpipe
} // namespace ufal
