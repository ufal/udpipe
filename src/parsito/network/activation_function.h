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
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {
namespace parsito {

struct activation_function {
  enum type { TANH = 0, CUBIC = 1 };

  static bool create(string_piece name, type& activation) {
    if (name == "tanh") return activation = TANH, true;
    if (name == "cubic") return activation = CUBIC, true;
    return false;
  }
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
