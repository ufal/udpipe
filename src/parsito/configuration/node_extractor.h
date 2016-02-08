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

class node_extractor {
 public:
  unsigned node_count() const;
  void extract(const configuration& conf, vector<int>& nodes) const;

  bool create(string_piece description, string& error);

 private:
  enum start_t { STACK = 0, BUFFER = 1 };
  enum direction_t { PARENT = 0, CHILD = 1 };
  struct node_selector {
    pair<start_t, int> start;
    vector<pair<direction_t, int>> directions;

    node_selector(start_t start, int start_index) : start(start, start_index) {}
  };

  vector<node_selector> selectors;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
