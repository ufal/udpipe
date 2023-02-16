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

void derivation_formatter::format_derivation(string& lemma) const {
  tagged_lemma result;
  result.lemma.swap(lemma);
  format_tagged_lemma(result);
  lemma.swap(result.lemma);
}

void derivation_formatter::format_tagged_lemmas(vector<tagged_lemma>& lemmas, const tagset_converter* converter) const {
  for (auto&& lemma : lemmas)
    format_tagged_lemma(lemma, converter);

  if (lemmas.size() > 1)
    tagset_converter_unique_analyzed(lemmas);
}


class none_derivation_formatter : public derivation_formatter {
  virtual void format_derivation(string& /*lemma*/) const override {}

  virtual void format_tagged_lemma(tagged_lemma& lemma, const tagset_converter* converter) const override {
    if (converter) converter->convert(lemma);
  }

  virtual void format_tagged_lemmas(vector<tagged_lemma>& lemmas, const tagset_converter* converter) const override {
    if (converter) converter->convert_analyzed(lemmas);
  }
};

derivation_formatter* derivation_formatter::new_none_derivation_formatter() {
  return new none_derivation_formatter();
}

class root_derivation_formatter : public derivation_formatter {
 public:
  root_derivation_formatter(const derivator* derinet) : derinet(derinet) {}

  virtual void format_tagged_lemma(tagged_lemma& lemma, const tagset_converter* converter) const override {
    for (derivated_lemma parent; derinet->parent(lemma.lemma, parent); )
      lemma.lemma.assign(parent.lemma);
    if (converter) converter->convert(lemma);
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

  virtual void format_tagged_lemma(tagged_lemma& lemma, const tagset_converter* converter) const override {
    tagged_lemma current(lemma);
    if (converter) converter->convert(lemma);
    for (derivated_lemma parent; derinet->parent(current.lemma, parent); current.lemma.swap(parent.lemma)) {
      tagged_lemma parrent_lemma(parent.lemma, current.tag);
      if (converter) converter->convert(parrent_lemma);
      lemma.lemma.append(" ").append(parrent_lemma.lemma);
    }
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

  virtual void format_tagged_lemma(tagged_lemma& lemma, const tagset_converter* converter) const override {
    string root(lemma.lemma), tag(lemma.tag);
    if (converter) converter->convert(lemma);
    for (derivated_lemma parent; derinet->parent(root, parent); root.swap(parent.lemma)) {}
    format_tree(root, tag, lemma, converter);
  }

  void format_tree(const string& root, const string& tag, tagged_lemma& tree, const tagset_converter* converter) const {
    vector<derivated_lemma> children;

    if (converter) {
      tagged_lemma current(root, tag);
      converter->convert(current);
      tree.lemma.append(" ").append(current.lemma);
    } else {
      tree.lemma.append(" ").append(root);
    }

    if (derinet->children(root, children))
      for (auto&& child : children)
        format_tree(child.lemma, tag, tree, converter);
    tree.lemma.push_back(' ');
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
