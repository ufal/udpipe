// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "transition.h"

namespace ufal {
namespace udpipe {
namespace parsito {

// Left arc
bool transition_left_arc::applicable(const configuration& conf) const {
  if (conf.single_root && label_is_root)
    return false;
  else
    return conf.stack.size() >= 2 && conf.stack[conf.stack.size() - 2];
}

int transition_left_arc::perform(configuration& conf) const {
  assert(applicable(conf));

  int parent = conf.stack.back(); conf.stack.pop_back();
  int child = conf.stack.back(); conf.stack.pop_back();
  conf.stack.push_back(parent);
  conf.t->set_head(child, parent, label);
  return child;
}

// Right arc
bool transition_right_arc::applicable(const configuration& conf) const {
  if (conf.single_root && label_is_root)
    return conf.stack.size() == 2 && conf.buffer.empty();
  else if (conf.single_root) // && !label_is_root
    return conf.stack.size() > 2;
  else
    return conf.stack.size() >= 2;
}

int transition_right_arc::perform(configuration& conf) const {
  assert(applicable(conf));

  int child = conf.stack.back(); conf.stack.pop_back();
  int parent = conf.stack.back();
  conf.t->set_head(child, parent, label);
  return child;
}

// Shift
bool transition_shift::applicable(const configuration& conf) const {
  return !conf.buffer.empty();
}

int transition_shift::perform(configuration& conf) const {
  assert(applicable(conf));

  conf.stack.push_back(conf.buffer.back());
  conf.buffer.pop_back();
  return -1;
}

// Swap
bool transition_swap::applicable(const configuration& conf) const {
  return conf.stack.size() >= 2 && conf.stack[conf.stack.size() - 2] && conf.stack[conf.stack.size() - 2] < conf.stack[conf.stack.size() - 1];
}

int transition_swap::perform(configuration& conf) const {
  assert(applicable(conf));

  int top = conf.stack.back(); conf.stack.pop_back();
  int to_buffer = conf.stack.back(); conf.stack.pop_back();
  conf.stack.push_back(top);
  conf.buffer.push_back(to_buffer);
  return -1;
}

// Left arc 2
bool transition_left_arc_2::applicable(const configuration& conf) const {
  if (conf.single_root && label_is_root)
    return false;
  else
    return conf.stack.size() >= 3 && conf.stack[conf.stack.size() - 3];
}

int transition_left_arc_2::perform(configuration& conf) const {
  assert(applicable(conf));

  int parent = conf.stack.back(); conf.stack.pop_back();
  int ignore = conf.stack.back(); conf.stack.pop_back();
  int child = conf.stack.back(); conf.stack.pop_back();
  conf.stack.push_back(ignore);
  conf.stack.push_back(parent);
  conf.t->set_head(child, parent, label);
  return child;
}

// Right arc 2
bool transition_right_arc_2::applicable(const configuration& conf) const {
  if (conf.single_root && label_is_root)
    return false;
  else if (conf.single_root) // && !label_is_root
    return conf.stack.size() >= 4;
  else
    return conf.stack.size() >= 3;
}

int transition_right_arc_2::perform(configuration& conf) const {
  assert(applicable(conf));

  int child = conf.stack.back(); conf.stack.pop_back();
  int to_buffer = conf.stack.back(); conf.stack.pop_back();
  int parent = conf.stack.back();
  conf.buffer.push_back(to_buffer);
  conf.t->set_head(child, parent, label);
  return child;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
