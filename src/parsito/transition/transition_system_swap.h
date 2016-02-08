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
#include "transition_system.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class transition_system_swap : public transition_system {
 public:
  transition_system_swap(const vector<string>& labels);

  virtual transition_oracle* oracle(const string& name) const override;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
