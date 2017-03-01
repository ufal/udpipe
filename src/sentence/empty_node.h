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

namespace ufal {
namespace udpipe {

class empty_node {
 public:
  int id;         // 0 is root, >0 is sentence word, <0 is undefined
  int index;      // index for the current id, should be numbered from 1, 0=undefined
  string form;    // form
  string lemma;   // lemma
  string upostag; // universal part-of-speech tag
  string xpostag; // language-specific part-of-speech tag
  string feats;   // list of morphological features
  string deps;    // secondary dependencies
  string misc;    // miscellaneous information

  empty_node(int id = -1, int index = 0) : id(id), index(index) {}
};

} // namespace udpipe
} // namespace ufal
