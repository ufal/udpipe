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
  clear();
}

bool sentence::empty() {
  return words.size() == 1;
}

void sentence::clear() {
  words.clear();
  multiword_tokens.clear();
  empty_nodes.clear();
  comments.clear();

  word& root = add_word(root_form);
  root.lemma = root.upostag = root.xpostag = root.feats = root_form;
}

word& sentence::add_word(string_piece form) {
  words.emplace_back((int)words.size(), form);
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

bool sentence::get_new_doc(string* id) const {
  if (get_comment("newdoc id", id))
    return true;
  return get_comment("newdoc", id);
}

void sentence::set_new_doc(bool new_doc, string_piece id) {
  remove_comment("newdoc");
  remove_comment("newdoc id");

  if (new_doc && id.len)
    set_comment("newdoc id", id);
  else if (new_doc)
    set_comment("newdoc");
}

bool sentence::get_new_par(string* id) const {
  if (get_comment("newpar id", id))
    return true;
  return get_comment("newpar", id);
}

void sentence::set_new_par(bool new_par, string_piece id) {
  remove_comment("newpar");
  remove_comment("newpar id");

  if (new_par && id.len)
    set_comment("newpar id", id);
  else if (new_par)
    set_comment("newpar");
}

bool sentence::get_sent_id(string& id) const {
  id.clear();

  return get_comment("sent_id", &id);
}

void sentence::set_sent_id(string_piece id) {
  remove_comment("sent_id");

  if (id.len)
    set_comment("sent_id", id);
}

bool sentence::get_text(string& text) const {
  text.clear();

  return get_comment("text", &text);
}

void sentence::set_text(string_piece text) {
  remove_comment("text");

  if (text.len)
    set_comment("text", text);
}

bool sentence::get_comment(string_piece name, string* value) const {
  for (auto&& comment : comments)
    if (comment[0] == '#') {
      // Skip spaces
      unsigned j = 1;
      while (j < comment.size() && (comment[j] == ' ' || comment[j] == '\t')) j++;

      // Try matching the name
      if (j + name.len <= comment.size() && comment.compare(j, name.len, name.str, name.len) == 0) {
        j += name.len;
        while (j < comment.size() && (comment[j] == ' ' || comment[j] == '\t')) j++;
        if (j < comment.size() && comment[j] == '=') {
          //We have a value
          j++;
          while (j < comment.size() && (comment[j] == ' ' || comment[j] == '\t')) j++;
          if (value) value->assign(comment, j, comment.size() - j);
        } else {
          // No value
          if (value) value->clear();
        }

        return true;
      }
    }

  return false;
}

void sentence::remove_comment(string_piece name) {
  for (unsigned i = comments.size(); i--; )
    if (comments[i][0] == '#') {
      // Skip spaces
      unsigned j = 1;
      while (j < comments[i].size() && (comments[i][j] == ' ' || comments[i][j] == '\t')) j++;

      // Remove matching comments
      if (j + name.len <= comments[i].size() && comments[i].compare(j, name.len, name.str, name.len) == 0)
        comments.erase(comments.begin() + i);
    }
}

void sentence::set_comment(string_piece name, string_piece value) {
  remove_comment(name);

  string comment;
  comment.append("# ").append(name.str, name.len);
  if (value.len) {
    comment.append(" = ");
    for (size_t i = 0; i < value.len; i++)
      comment.push_back(value.str[i] == '\r' || value.str[i] == '\n' ? ' ' : value.str[i]);
  }
  comments.push_back(move(comment));
}


} // namespace udpipe
} // namespace ufal
