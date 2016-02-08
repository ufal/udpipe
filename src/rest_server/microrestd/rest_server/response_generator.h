// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "string_piece.h"

namespace ufal {
namespace microrestd {

class response_generator {
 public:
  virtual ~response_generator() {}

  virtual bool generate() = 0;
  virtual string_piece current() const = 0;
  virtual void consume(size_t length) = 0;
};

} // namespace microrestd
} // namespace ufal
