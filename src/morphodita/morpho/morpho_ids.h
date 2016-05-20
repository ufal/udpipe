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

class morpho_ids {
 public:
  enum morpho_id {
    CZECH = 0,
    ENGLISH_V1 = 1,
    GENERIC = 2,
    EXTERNAL = 3,
    ENGLISH_V2 = 4,
    ENGLISH_V3 = 5, ENGLISH = ENGLISH_V3,
    SLOVAK_PDT = 6,
    DERIVATOR_DICTIONARY = 7,
  };

  static bool parse(const string& str, morpho_id& id) {
    if (str == "czech") return id = CZECH, true;
    if (str == "english") return id = ENGLISH, true;
    if (str == "external") return id = EXTERNAL, true;
    if (str == "generic") return id = GENERIC, true;
    if (str == "slovak_pdt") return id = SLOVAK_PDT, true;
    return false;
  }
};

typedef morpho_ids::morpho_id morpho_id;

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
