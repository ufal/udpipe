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
#include "token.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {

class multiword_token : public token {
 public:
  // form and misc are inherited from token
  int id_first, id_last;
  string feats; // can contain only Typo=Yes

  multiword_token(int id_first = -1, int id_last = -1, string_piece form = string_piece(), string_piece misc = string_piece())
      : token(form, misc), id_first(id_first), id_last(id_last), feats() {}
};

} // namespace udpipe
} // namespace ufal
