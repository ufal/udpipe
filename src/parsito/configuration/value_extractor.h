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
#include "configuration.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class value_extractor {
 public:
  void extract(const node& n, string& value) const;

  bool create(string_piece description, string& error);

 private:
  enum value_t { FORM = 0, LEMMA = 1, LEMMA_ID = 2, TAG = 3, UNIVERSAL_TAG = 4,
    FEATS = 5, UNIVERSAL_TAG_FEATS = 6, DEPREL = 7 };
  value_t selector;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
