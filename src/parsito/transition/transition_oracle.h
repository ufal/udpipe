// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "parsito/configuration/configuration.h"
#include "parsito/tree/tree.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class transition_oracle {
 public:
  virtual ~transition_oracle() {}

  struct predicted_transition {
    unsigned best;
    unsigned to_follow;

    predicted_transition(unsigned best, unsigned to_follow) : best(best), to_follow(to_follow) {}
  };

  class tree_oracle {
   public:
    virtual ~tree_oracle() {}

    virtual predicted_transition predict(const configuration& conf, unsigned network_outcome, unsigned iteration) const = 0;
    virtual void interesting_transitions(const configuration& conf, vector<unsigned>& transitions) const = 0;
  };

  virtual unique_ptr<tree_oracle> create_tree_oracle(const tree& gold) const = 0;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
