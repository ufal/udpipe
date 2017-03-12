// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "transition_system_swap.h"

namespace ufal {
namespace udpipe {
namespace parsito {

transition_system_swap::transition_system_swap(const vector<string>& labels) : transition_system(labels) {
  transitions.emplace_back(new transition_shift());
  transitions.emplace_back(new transition_swap());
  for (auto&& label : labels) {
    transitions.emplace_back(new transition_left_arc(label));
    transitions.emplace_back(new transition_right_arc(label));
  }
}

// Static oracle
class transition_system_swap_oracle_static : public transition_oracle {
 public:
  transition_system_swap_oracle_static(const vector<string>& labels, bool lazy) : labels(labels), lazy(lazy) {}

  class tree_oracle_static : public transition_oracle::tree_oracle {
   public:
    tree_oracle_static(const vector<string>& labels, const tree& gold, vector<int>&& projective_order, vector<int>&& projective_components)
        : labels(labels), gold(gold), projective_order(projective_order), projective_components(projective_components) {}
    virtual predicted_transition predict(const configuration& conf, unsigned network_outcome, unsigned iteration) const override;
    virtual void interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const override;
   private:
    const vector<string>& labels;
    const tree& gold;
    const vector<int> projective_order;
    const vector<int> projective_components;
  };

  virtual unique_ptr<tree_oracle> create_tree_oracle(const tree& gold) const override;
 private:
  void create_projective_order(const tree& gold, int node, vector<int>& projective_order, int& projective_index) const;
  void create_projective_component(const tree& gold, int node, vector<int>& projective_components, int component_index) const;

  const vector<string>& labels;
  bool lazy;
};

unique_ptr<transition_oracle::tree_oracle> transition_system_swap_oracle_static::create_tree_oracle(const tree& gold) const {
  vector<int> projective_order(gold.nodes.size());
  int projective_index;
  create_projective_order(gold, 0, projective_order, projective_index);

  vector<int> projective_components;
  if (lazy) {
    tree_oracle_static projective_oracle(labels, gold, vector<int>(), vector<int>());
    configuration conf(false);
    tree t = gold;
    transition_system_swap system(labels);

    conf.init(&t);
    while (!conf.final()) {
      auto prediction = projective_oracle.predict(conf, 0, 0);
      if (!system.applicable(conf, prediction.to_follow)) break;
      system.perform(conf, prediction.to_follow);
    }

    projective_components.assign(gold.nodes.size(), 0);
    for (auto&& node : conf.stack)
      if (node)
        create_projective_component(t, node, projective_components, node);
  }

  return unique_ptr<transition_oracle::tree_oracle>(new tree_oracle_static(labels, gold, move(projective_order), move(projective_components)));
}

void transition_system_swap_oracle_static::create_projective_order(const tree& gold, int node, vector<int>& projective_order, int& projective_index) const {
  unsigned child_index = 0;
  while (child_index < gold.nodes[node].children.size() && gold.nodes[node].children[child_index] < node)
    create_projective_order(gold, gold.nodes[node].children[child_index++], projective_order, projective_index);
  projective_order[node] = projective_index++;
  while (child_index < gold.nodes[node].children.size())
    create_projective_order(gold, gold.nodes[node].children[child_index++], projective_order, projective_index);
}

void transition_system_swap_oracle_static::create_projective_component(const tree& gold, int node, vector<int>& projective_components, int component_index) const {
  projective_components[node] = component_index;
  for (auto&& child : gold.nodes[node].children)
    create_projective_component(gold, child, projective_components, component_index);
}

void transition_system_swap_oracle_static::tree_oracle_static::interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const {
  transitions.clear();
  if (!conf.buffer.empty()) transitions.push_back(0);
  if (conf.stack.size() >= 2) {
    // Swap
    if (!projective_order.empty()) {
      int last = conf.stack[conf.stack.size() - 1];
      int prev = conf.stack[conf.stack.size() - 2];
      if (projective_order[last] < projective_order[prev] &&
          (projective_components.empty() ||
           (conf.buffer.empty() || projective_components[last] != projective_components[conf.buffer.back()])))
        transitions.push_back(1);
    }

    // Arcs
    for (int direction = 0; direction < 2; direction++) {
      int child = conf.stack[conf.stack.size() - 2 + direction];
      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          transitions.push_back(2 + 2*i + direction);
    }
  }
}

transition_oracle::predicted_transition transition_system_swap_oracle_static::tree_oracle_static::predict(const configuration& conf, unsigned /*network_outcome*/, unsigned /*iteration*/) const {
  // Use left if appropriate
  if (conf.stack.size() >= 2) {
    int parent = conf.stack[conf.stack.size() - 1];
    int child = conf.stack[conf.stack.size() - 2];
    if (gold.nodes[child].head == parent && gold.nodes[child].children.size() == conf.t->nodes[child].children.size()) {
      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          return predicted_transition(2 + 2*i, 2 + 2*i);

      assert(!"label was not found");
    }
  }

  // Use right if appropriate
  if (conf.stack.size() >= 2) {
    int child = conf.stack[conf.stack.size() - 1];
    int parent = conf.stack[conf.stack.size() - 2];
    if (gold.nodes[child].head == parent && gold.nodes[child].children.size() == conf.t->nodes[child].children.size()) {
      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          return predicted_transition(2 + 2*i + 1, 2 + 2*i + 1);

      assert(!"label was not found");
    }
  }

  // Use swap if appropriate
  if (conf.stack.size() >= 2 && !projective_order.empty()) {
    int last = conf.stack[conf.stack.size() - 1];
    int prev = conf.stack[conf.stack.size() - 2];
    if (projective_order[last] < projective_order[prev] &&
        (projective_components.empty() ||
         (conf.buffer.empty() || projective_components[last] != projective_components[conf.buffer.back()])))
      return predicted_transition(1, 1);
  }

  // Otherwise, just shift
  return predicted_transition(0, 0);
}

// Oracle factory method
transition_oracle* transition_system_swap::oracle(const string& name) const {
  if (name == "static_eager") return new transition_system_swap_oracle_static(labels, false);
  if (name == "static_lazy") return new transition_system_swap_oracle_static(labels, true);
  return nullptr;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
