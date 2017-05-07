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
#include "utils/binary_decoder.h"

namespace ufal {
namespace udpipe {
namespace parsito {

// Parser
class parser {
 public:
  virtual ~parser() {};

  virtual void parse(tree& t, unsigned beam_size = 0, double* cost = nullptr) const = 0;

  enum { NO_CACHE = 0, FULL_CACHE = 2147483647};
  static parser* load(const char* file, unsigned cache = 1000);
  static parser* load(istream& in, unsigned cache = 1000);

 protected:
  virtual void load(binary_decoder& data, unsigned cache) = 0;
  static parser* create(const string& name);
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
