// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
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
namespace morphodita {

struct derivated_lemma {
  string lemma;
};

class derivator {
 public:
  virtual ~derivator() {}

  // For given lemma, return the parent in the derivation graph.
  // The lemma is assumed to be lemma id and any lemma comments are ignored.
  virtual bool parent(string_piece lemma, derivated_lemma& parent) const = 0;

  // For given lemma, return the children in the derivation graph.
  // The lemma is assumed to be lemma id and any lemma comments are ignored.
  virtual bool children(string_piece lemma, vector<derivated_lemma>& children) const = 0;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
