// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "configuration.h"

namespace ufal {
namespace udpipe {
namespace parsito {

void configuration::init(tree* t) {
  assert(t);

  t->unlink_all_nodes();
  this->t = t;

  stack.clear();
  if (!t->nodes.empty()) stack.push_back(0);

  buffer.clear();
  buffer.reserve(t->nodes.size());
  for (size_t i = t->nodes.size(); i > 1; i--)
    buffer.push_back(i - 1);
}

bool configuration::final() {
  return buffer.empty() && stack.size() <= 1;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
