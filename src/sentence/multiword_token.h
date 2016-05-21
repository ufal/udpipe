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
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {

class multiword_token {
 public:
  int id_first, id_last;
  string form;
  string misc;

  multiword_token(int id_first, int id_last, string_piece form = string_piece(), string_piece misc = string_piece()) : id_first(id_first), id_last(id_last) {
    if (form.len) this->form.assign(form.str, form.len);
    if (misc.len) this->misc.assign(misc.str, misc.len);
  }
};

} // namespace udpipe
} // namespace ufal
