// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "sentence.h"

namespace ufal {
namespace udpipe {

const string sentence::root_form = "<root>";

sentence::sentence() {
  add_word(root_form);
}

bool sentence::empty() {
  return words.size() == 1;
}

void sentence::clear() {
  words.clear();
  multiword_tokens.clear();
  comments.clear();

  word& root = add_word(root_form);
  root.lemma = root.upostag = root.xpostag = root.feats = root_form;
}

word& sentence::add_word(const string& form) {
  words.emplace_back(words.size(), form);
  return words.back();
}

void sentence::set_head(int id, int head, const string& deprel) {
  assert(id >= 0 && id < int(words.size()));
  assert(head < int(words.size()));

  // Remove existing head
  if (words[id].head >= 0) {
    auto& children = words[words[id].head].children;
    for (size_t i = children.size(); i && children[i-1] >= id; i--)
      if (children[i-1] == id) {
        children.erase(children.begin() + i - 1);
        break;
      }
  }

  // Set new head
  words[id].head = head;
  words[id].deprel = deprel;
  if (head >= 0) {
    auto& children = words[head].children;
    size_t i = children.size();
    while (i && children[i-1] > id) i--;
    if (!i || children[i-1] < id) children.insert(children.begin() + i, id);
  }
}

void sentence::unlink_all_words() {
  for (auto&& word : words) {
    word.head = -1;
    word.deprel.clear();
    word.children.clear();
  }
}

} // namespace udpipe
} // namespace ufal
