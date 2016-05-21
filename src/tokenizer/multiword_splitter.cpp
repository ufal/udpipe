// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "multiword_splitter.h"

namespace ufal {
namespace udpipe {

void multiword_splitter::append_token(string_piece token, string_piece misc, sentence& s) const {
  // TODO

  s.add_word(string(token.str, token.len));
  if (misc.len) s.words.back().misc.assign(misc.str, misc.len);
}

multiword_splitter* multiword_splitter::load(istream& is) {
  char version;
  if (!is.get(version)) return nullptr;

  // TODO

  return new multiword_splitter();
}

} // namespace udpipe
} // namespace ufal
