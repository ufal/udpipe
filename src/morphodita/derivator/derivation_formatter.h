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
#include "derivator.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class derivation_formatter {
 public:
  virtual ~derivation_formatter() {}

  // Perform the required derivation and store it directly in the lemma.
  virtual void format_derivation(string& lemma) const = 0;

  // Static factory methods.
  static derivation_formatter* new_none_derivation_formatter();
  static derivation_formatter* new_root_derivation_formatter(const derivator* derinet);
  static derivation_formatter* new_path_derivation_formatter(const derivator* derinet);
  static derivation_formatter* new_tree_derivation_formatter(const derivator* derinet);
  // String version of static factory method.
  static derivation_formatter* new_derivation_formatter(string_piece name, const derivator* derinet);
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
