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
#include "transition.h"
#include "transition_oracle.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class transition_system {
 public:
  virtual ~transition_system() {}

  virtual unsigned transition_count() const;
  virtual bool applicable(const configuration& conf, unsigned transition) const;
  virtual int perform(configuration& conf, unsigned transition) const;
  virtual transition_oracle* oracle(const string& name) const = 0;

  static transition_system* create(const string& name, const vector<string>& labels);

 protected:
  transition_system(const vector<string>& labels) : labels(labels) {}

  const vector<string>& labels;
  vector<unique_ptr<transition>> transitions;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
