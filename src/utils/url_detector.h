// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "string_piece.h"

namespace ufal {
namespace udpipe {
namespace utils {

class url_detector {
 public:
  enum url_type { NO_URL = 0, URL = 1, EMAIL = 2 };

  // Detect whether given string is an url or an email.
  // If length==nullptr, the whole str must match.
  // If length!=nullptr, length of longest matching prefix is returned.
  static url_type detect(string_piece str, size_t* length = nullptr);
};

} // namespace utils
} // namespace udpipe
} // namespace ufal
