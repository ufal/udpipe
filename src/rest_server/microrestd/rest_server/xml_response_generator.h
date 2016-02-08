// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "xml_builder.h"
#include "response_generator.h"

namespace ufal {
namespace microrestd {

class xml_response_generator : public response_generator {
 public:
  virtual string_piece current() const override;
  virtual void consume(size_t length) override;

  xml_builder xml;
  static const char* mime;
};

} // namespace microrestd
} // namespace ufal
