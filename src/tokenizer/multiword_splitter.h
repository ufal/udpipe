// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "sentence/sentence.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {

class multiword_splitter {
 public:
  void append_token(string_piece token, string_piece misc, sentence& s) const;

  static multiword_splitter* load(istream& is);

 private:
  multiword_splitter(unsigned version) : version(version) {}
  unsigned version;
  enum { VERSION_LATEST = 2 };
  friend class multiword_splitter_trainer;

  struct suffix_info {
    vector<string> words;
  };
  unordered_map<string, suffix_info> full_rules, suffix_rules;
};

} // namespace udpipe
} // namespace ufal
