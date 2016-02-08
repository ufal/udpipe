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
#include "node.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class tree {
 public:
  tree();

  vector<node> nodes;

  bool empty();
  void clear();
  node& add_node(const string& form);
  void set_head(int id, int head, const string& deprel);
  void unlink_all_nodes();

  static const string root_form;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
