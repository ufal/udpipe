// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "elementary_features.h"
#include "utils/binary_encoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

template <class Map>
inline bool elementary_features<Map>::save(ostream& os) {
  binary_encoder enc;

  enc.add_1B(maps.size());
  for (auto&& map : maps)
    map.save(enc);

  return compressor::save(os, enc);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
