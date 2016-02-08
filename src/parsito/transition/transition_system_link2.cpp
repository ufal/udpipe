// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "transition_system_link2.h"

namespace ufal {
namespace udpipe {
namespace parsito {

transition_system_link2::transition_system_link2(const vector<string>& labels) : transition_system(labels) {
  transitions.emplace_back(new transition_shift());
  for (auto&& label : labels) {
    transitions.emplace_back(new transition_left_arc(label));
    transitions.emplace_back(new transition_right_arc(label));
    transitions.emplace_back(new transition_left_arc_2(label));
    transitions.emplace_back(new transition_right_arc_2(label));
  }
}

// Static oracle
class transition_system_link2_oracle_static : public transition_oracle {
 public:
  transition_system_link2_oracle_static(const vector<string>& labels) : labels(labels) {}

  class tree_oracle_static : public transition_oracle::tree_oracle {
   public:
    tree_oracle_static(const vector<string>& labels, const tree& gold) : labels(labels), gold(gold) {}
    virtual predicted_transition predict(const configuration& conf, unsigned network_outcome, unsigned iteration) const override;
    virtual void interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const override;
   private:
    const vector<string>& labels;
    const tree& gold;
  };

  virtual unique_ptr<tree_oracle> create_tree_oracle(const tree& gold) const override;
 private:
  const vector<string>& labels;
};

unique_ptr<transition_oracle::tree_oracle> transition_system_link2_oracle_static::create_tree_oracle(const tree& gold) const {
  return unique_ptr<transition_oracle::tree_oracle>(new tree_oracle_static(labels, gold));
}

void transition_system_link2_oracle_static::tree_oracle_static::interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const {
  transitions.clear();

  // Shift
  if (!conf.buffer.empty()) transitions.push_back(0);

  // Arcs
  unsigned parents[4] = {1, 2, 1, 3};
  unsigned children[4] = {2, 1, 3, 1};
  for (int direction = 0; direction < 4; direction++)
    if (conf.stack.size() >= parents[direction] && conf.stack.size() >= children[direction]) {
      int parent = conf.stack[conf.stack.size() - parents[direction]];
      int child = conf.stack[conf.stack.size() - children[direction]];

      // Allow arc_2 only when seeing golden edge.
      if (direction >= 2 && gold.nodes[child].head != parent) continue;

      for (size_t i = 0; i < labels.size(); i++)
        if (gold.nodes[child].deprel == labels[i])
          transitions.push_back(1 + 4*i + direction);
    }
}

transition_oracle::predicted_transition transition_system_link2_oracle_static::tree_oracle_static::predict(const configuration& conf, unsigned /*network_outcome*/, unsigned /*iteration*/) const {
  // Arcs
  unsigned parents[4] = {1, 2, 1, 3};
  unsigned children[4] = {2, 1, 3, 1};
  for (int direction = 0; direction < 4; direction++)
    if (conf.stack.size() >= parents[direction] && conf.stack.size() >= children[direction]) {
      int parent = conf.stack[conf.stack.size() - parents[direction]];
      int child = conf.stack[conf.stack.size() - children[direction]];

      if (gold.nodes[child].head == parent && gold.nodes[child].children.size() == conf.t->nodes[child].children.size()) {
        for (size_t i = 0; i < labels.size(); i++)
          if (gold.nodes[child].deprel == labels[i])
            return predicted_transition(1 + 4*i + direction, 1 + 4*i + direction);

        assert(!"label was not found");
      }
    }

  // Otherwise, just shift
  return predicted_transition(0, 0);
}

// Oracle factory method
transition_oracle* transition_system_link2::oracle(const string& name) const {
  if (name == "static") return new transition_system_link2_oracle_static(labels);
  return nullptr;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
