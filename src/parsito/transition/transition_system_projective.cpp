// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <unordered_set>

#include "transition_system_projective.h"

namespace ufal {
namespace udpipe {
namespace parsito {

transition_system_projective::transition_system_projective(const vector<string>& labels) : transition_system(labels) {
  transitions.emplace_back(new transition_shift());
  for (auto&& label : labels) {
    transitions.emplace_back(new transition_left_arc(label));
    transitions.emplace_back(new transition_right_arc(label));
  }
}

// Static oracle
class transition_system_projective_oracle_static : public transition_oracle {
 public:
  transition_system_projective_oracle_static(const vector<string>& labels) : labels(labels) {
    for (root_label = 0; root_label < labels.size(); root_label++) if (labels[root_label] == "root") break;
  }

  class tree_oracle_static : public transition_oracle::tree_oracle {
   public:
    tree_oracle_static(const vector<string>& labels, unsigned root_label, const tree& gold) : labels(labels), root_label(root_label), gold(gold) {}
    virtual predicted_transition predict(const configuration& conf, unsigned network_outcome, unsigned iteration) const override;
    virtual void interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const override;
   private:
    const vector<string>& labels;
    unsigned root_label;
    const tree& gold;
  };

  virtual unique_ptr<tree_oracle> create_tree_oracle(const tree& gold) const override;
 private:
  const vector<string>& labels;
  unsigned root_label;
};

unique_ptr<transition_oracle::tree_oracle> transition_system_projective_oracle_static::create_tree_oracle(const tree& gold) const {
  return unique_ptr<transition_oracle::tree_oracle>(new tree_oracle_static(labels, root_label, gold));
}

void transition_system_projective_oracle_static::tree_oracle_static::interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const {
  transitions.clear();
  if (!conf.buffer.empty()) transitions.push_back(0);
  if (conf.stack.size() >= 2)
    for (int direction = 0; direction < 2; direction++) {
      int child = conf.stack[conf.stack.size() - 2 + direction];
      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          if (!conf.single_root ||
              (i == root_label && conf.stack.size() == 2 && conf.buffer.empty() && direction == 1) ||
              (i != root_label && conf.stack.size() > 2))
            transitions.push_back(1 + 2*i + direction);
    }
}

transition_oracle::predicted_transition transition_system_projective_oracle_static::tree_oracle_static::predict(const configuration& conf, unsigned /*network_outcome*/, unsigned /*iteration*/) const {
  // Use left if appropriate
  if (conf.stack.size() >= 2) {
    int parent = conf.stack[conf.stack.size() - 1];
    int child = conf.stack[conf.stack.size() - 2];
    if (gold.nodes[child].head == parent) {
      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          return predicted_transition(1 + 2*i, 1 + 2*i);

      assert(!"label was not found");
    }
  }

  // Use right if appropriate
  if (conf.stack.size() >= 2) {
    int child = conf.stack[conf.stack.size() - 1];
    int parent = conf.stack[conf.stack.size() - 2];
    if (gold.nodes[child].head == parent &&
        (conf.buffer.empty() || gold.nodes[child].children.empty() || gold.nodes[child].children.back() < conf.buffer.back())) {
      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          return predicted_transition(1 + 2*i + 1, 1 + 2*i + 1);

      assert(!"label was not found");
    }
  }

  // Otherwise, just shift
  return predicted_transition(0, 0);
}

// Dynamic oracle
class transition_system_projective_oracle_dynamic : public transition_oracle {
 public:
  transition_system_projective_oracle_dynamic(const vector<string>& labels) : labels(labels) {
    for (root_label = 0; root_label < labels.size(); root_label++) if (labels[root_label] == "root") break;
  }

  class tree_oracle_dynamic : public transition_oracle::tree_oracle {
   public:
    tree_oracle_dynamic(const vector<string>& labels, unsigned root_label, const tree& gold) : labels(labels), gold(gold), oracle_static(labels, root_label, gold) {}
    virtual predicted_transition predict(const configuration& conf, unsigned network_outcome, unsigned iteration) const override;
    virtual void interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const override;
   private:
    const vector<string>& labels;
    const tree& gold;
    transition_system_projective_oracle_static::tree_oracle_static oracle_static;
  };

  virtual unique_ptr<tree_oracle> create_tree_oracle(const tree& gold) const override;
 private:
  const vector<string>& labels;
  unsigned root_label;
};

unique_ptr<transition_oracle::tree_oracle> transition_system_projective_oracle_dynamic::create_tree_oracle(const tree& gold) const {
  return unique_ptr<transition_oracle::tree_oracle>(new tree_oracle_dynamic(labels, root_label, gold));
}

void transition_system_projective_oracle_dynamic::tree_oracle_dynamic::interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const {
  oracle_static.interesting_transitions(conf, transitions);
}

transition_oracle::predicted_transition transition_system_projective_oracle_dynamic::tree_oracle_dynamic::predict(const configuration& conf, unsigned network_outcome, unsigned iteration) const {
  // Use static oracle in the first iteration
  if (iteration <= 1)
    return oracle_static.predict(conf, network_outcome, iteration);

  // Use dynamic programming to compute transition leading to best parse tree

  // Start by computing the right stack
  vector<int> right_stack;

  unordered_set<int> right_stack_inserted;
  if (!conf.buffer.empty()) {
    int buffer_start = conf.buffer.back();
    for (size_t i = conf.buffer.size(); i--; ) {
      const auto& node = conf.buffer[i];
      bool to_right_stack = gold.nodes[node].head < buffer_start;
      for (auto&& child : gold.nodes[node].children)
        to_right_stack |= child < buffer_start || right_stack_inserted.count(child);
      if (to_right_stack) {
        right_stack.push_back(node);
        right_stack_inserted.insert(node);
      }
    }
  }

  // Fill the array T from the 2014 Goldberg paper
  class t_representation {
   public:
    t_representation(const vector<int>& stack, const vector<int>& right_stack, const tree& gold, const vector<string>& labels)
        : stack(stack), right_stack(right_stack), gold(gold), labels(labels) {
      for (int i = 0; i < 2; i++) {
        costs[i].reserve((stack.size() + right_stack.size()) * (stack.size() + right_stack.size()));
        transitions[i].reserve((stack.size() + right_stack.size()) * (stack.size() + right_stack.size()));
      }
    }

    void prepare(unsigned diagonal) {
      costs[diagonal & 1].assign((diagonal + 1) * (diagonal + 1), gold.nodes.size() + 1);
      transitions[diagonal & 1].assign((diagonal + 1) * (diagonal + 1), -1);
    }

    int& cost(unsigned i, unsigned j, unsigned h) { return costs[(i+j) & 1][i * (i+j+1) + h]; }
    int& transition(unsigned i, unsigned j, unsigned h) { return transitions[(i+j) & 1][i * (i+j+1) + h]; }

    int node(unsigned i, unsigned /*j*/, unsigned h) const { return h <= i ? stack[stack.size() - 1 - i + h] : right_stack[h - i - 1]; }
    int edge_cost(int parent, int child) const { return gold.nodes[child].head != parent; }
    int which_arc_transition(int parent, int child) const {
      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          return 1 + 2*i + (child > parent);
      assert(!"label was not found");
      return 0; // To keep VS 2015 happy and warning-free
    }

   private:
    const vector<int>& stack;
    const vector<int>& right_stack;
    const tree& gold;
    const vector<string>& labels;
    vector<int> costs[2], transitions[2];
  } t(conf.stack, right_stack, gold, labels);

  t.prepare(0);
  t.cost(0, 0, 0) = 0;
  for (unsigned diagonal = 0; diagonal < conf.stack.size() + right_stack.size(); diagonal++) {
    t.prepare(diagonal + 1);
    for (unsigned i = diagonal > right_stack.size() ? diagonal - right_stack.size() : 0; i <= diagonal && i < conf.stack.size(); i++) {
      unsigned j = diagonal - i;

      // Try extending stack
      if (i+1 < conf.stack.size())
        for (unsigned h = 0; h <= diagonal; h++) {
          int h_node = t.node(i, j, h), new_node = t.node(i+1, j, 0);
          if (new_node && t.cost(i, j, h) + t.edge_cost(h_node, new_node) < t.cost(i+1, j, h+1) + (t.transition(i, j, h) != 0)) {
            t.cost(i+1, j, h+1) = t.cost(i, j, h) + t.edge_cost(h_node, new_node);
            t.transition(i+1, j, h+1) = t.transition(i, j, h) >= 0 ? t.transition(i, j, h) : t.which_arc_transition(h_node, new_node);
          }
          if (t.cost(i, j, h) + t.edge_cost(new_node, h_node) < t.cost(i+1, j, 0) + (t.transition(i, j, h) != 0)) {
            t.cost(i+1, j, 0) = t.cost(i, j, h) + t.edge_cost(new_node, h_node);
            t.transition(i+1, j, 0) = t.transition(i, j, h) >= 0 ? t.transition(i, j, h) : t.which_arc_transition(new_node, h_node);
          }
        }

      // Try extending right_stack
      if (j+1 < right_stack.size() + 1)
        for (unsigned h = 0; h <= diagonal; h++) {
          int h_node = t.node(i, j, h), new_node = t.node(i, j+1, diagonal+1);
          if (t.cost(i, j, h) + t.edge_cost(h_node, new_node) < t.cost(i, j+1, h) + (t.transition(i, j, h) > 0)) {
            t.cost(i, j+1, h) = t.cost(i, j, h) + t.edge_cost(h_node, new_node);
            t.transition(i, j+1, h) = t.transition(i, j, h) >= 0 ? t.transition(i, j, h) : 0;
          }
          if (h_node && t.cost(i, j, h) + t.edge_cost(new_node, h_node) < t.cost(i, j+1, diagonal+1) + (t.transition(i, j, h) > 0)) {
            t.cost(i, j+1, diagonal+1) = t.cost(i, j, h) + t.edge_cost(new_node, h_node);
            t.transition(i, j+1, diagonal+1) = t.transition(i, j, h) >= 0 ? t.transition(i, j, h) : 0;
          }
        }
    }
  }

  return predicted_transition(t.transition(conf.stack.size() - 1, right_stack.size(), 0), network_outcome);
}

// Oracle factory method
transition_oracle* transition_system_projective::oracle(const string& name) const {
  if (name == "static") return new transition_system_projective_oracle_static(labels);
  if (name == "dynamic") return new transition_system_projective_oracle_dynamic(labels);
  return nullptr;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
