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

namespace ufal {
namespace udpipe {
namespace utils {

class binary_decoder;
class binary_encoder;

class compressor {
 public:
  static bool load(istream& is, binary_decoder& data);
  static bool save(ostream& os, const binary_encoder& enc);
};

} // namespace utils
} // namespace udpipe
} // namespace ufal
