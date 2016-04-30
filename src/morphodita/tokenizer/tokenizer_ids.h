// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class tokenizer_ids {
 public:
  enum tokenizer_id {
    CZECH = 0,
    ENGLISH = 1,
    GENERIC = 2,
  };

  static bool parse(const string& str, tokenizer_id& id) {
    if (str == "czech") return id = CZECH, true;
    if (str == "english") return id = ENGLISH, true;
    if (str == "generic") return id = GENERIC, true;
    return false;
  }
};

typedef tokenizer_ids::tokenizer_id tokenizer_id;

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
