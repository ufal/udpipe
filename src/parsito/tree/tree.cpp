// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "tree.h"

namespace ufal {
namespace udpipe {
namespace parsito {

const string tree::root_form = "<root>";

tree::tree() {
  clear();
}

bool tree::empty() {
  return nodes.size() == 1;
}

void tree::clear() {
  nodes.clear();
  node& root = add_node(root_form);
  root.lemma = root.upostag = root.xpostag = root.feats = root_form;
}

node& tree::add_node(const string& form) {
  nodes.emplace_back((int)nodes.size(), form);
  return nodes.back();
}

void tree::set_head(int id, int head, const string& deprel) {
  assert(id >= 0 && id < int(nodes.size()));
  assert(head < int(nodes.size()));

  // Remove existing head
  if (nodes[id].head >= 0) {
    auto& children = nodes[nodes[id].head].children;
    for (size_t i = children.size(); i && children[i-1] >= id; i--)
      if (children[i-1] == id) {
        children.erase(children.begin() + i - 1);
        break;
      }
  }

  // Set new head
  nodes[id].head = head;
  nodes[id].deprel = deprel;
  if (head >= 0) {
    auto& children = nodes[head].children;
    size_t i = children.size();
    while (i && children[i-1] > id) i--;
    if (!i || children[i-1] < id) children.insert(children.begin() + i, id);
  }
}

void tree::unlink_all_nodes() {
  for (auto&& node : nodes) {
    node.head = -1;
    node.deprel.clear();
    node.children.clear();
  }
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
