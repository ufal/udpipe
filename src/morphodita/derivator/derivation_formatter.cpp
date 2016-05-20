// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "derivation_formatter.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class none_derivation_formatter : public derivation_formatter {
  virtual void format_derivation(string& /*lemma*/) const override {}
};

derivation_formatter* derivation_formatter::new_none_derivation_formatter() {
  return new none_derivation_formatter();
}

class root_derivation_formatter : public derivation_formatter {
 public:
  root_derivation_formatter(const derivator* derinet) : derinet(derinet) {}

  virtual void format_derivation(string& lemma) const override {
    for (derivated_lemma parent; derinet->parent(lemma, parent); )
      lemma.assign(parent.lemma);
  }

 private:
  const derivator* derinet;
};

derivation_formatter* derivation_formatter::new_root_derivation_formatter(const derivator* derinet) {
  return derinet ? new root_derivation_formatter(derinet) : nullptr;
}

class path_derivation_formatter : public derivation_formatter {
 public:
  path_derivation_formatter(const derivator* derinet) : derinet(derinet) {}

  virtual void format_derivation(string& lemma) const override {
    string current(lemma);
    for (derivated_lemma parent; derinet->parent(current, parent); current.swap(parent.lemma))
      lemma.append(" ").append(parent.lemma);
  }

 private:
  const derivator* derinet;
};

derivation_formatter* derivation_formatter::new_path_derivation_formatter(const derivator* derinet) {
  return derinet ? new path_derivation_formatter(derinet) : nullptr;
}

class tree_derivation_formatter : public derivation_formatter {
 public:
  tree_derivation_formatter(const derivator* derinet) : derinet(derinet) {}

  virtual void format_derivation(string& lemma) const override {
    string root(lemma);
    for (derivated_lemma parent; derinet->parent(root, parent); root.swap(parent.lemma)) {}
    format_tree(root, lemma);
  }

  void format_tree(const string& root, string& tree) const {
    vector<derivated_lemma> children;

    tree.append(" ").append(root);
    if (derinet->children(root, children))
      for (auto&& child : children)
        format_tree(child.lemma, tree);
    tree.push_back(' ');
  }

 private:
  const derivator* derinet;
};

derivation_formatter* derivation_formatter::new_tree_derivation_formatter(const derivator* derinet) {
  return derinet ? new tree_derivation_formatter(derinet) : nullptr;
}

derivation_formatter* derivation_formatter::new_derivation_formatter(string_piece name, const derivator* derinet) {
  if (name == "none") return new_none_derivation_formatter();
  if (name == "root") return new_root_derivation_formatter(derinet);
  if (name == "path") return new_path_derivation_formatter(derinet);
  if (name == "tree") return new_tree_derivation_formatter(derinet);
  return nullptr;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
