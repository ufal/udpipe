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
#include "morphodita/morpho/persistent_unordered_map.h"
#include "tagger.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"
#include "utils/unaligned_access.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
enum elementary_feature_type { PER_FORM, PER_TAG, DYNAMIC };
enum elementary_feature_range { ONLY_CURRENT, ANY_OFFSET };

typedef uint32_t elementary_feature_value;
enum :elementary_feature_value { elementary_feature_unknown = 0, elementary_feature_empty = 1 };

struct elementary_feature_description {
  string name;
  elementary_feature_type type;
  elementary_feature_range range;
  int index;
  int map_index;
};

template<class Map>
class elementary_features {
 public:
  bool load(istream& is);
  bool save(ostream& out);

  vector<Map> maps;
};

class persistent_elementary_feature_map : public persistent_unordered_map {
 public:
  persistent_elementary_feature_map() : persistent_unordered_map() {}
  persistent_elementary_feature_map(const persistent_unordered_map&& map) : persistent_unordered_map(map) {}

  elementary_feature_value value(const char* feature, int len) const {
    auto* it = at_typed<elementary_feature_value>(feature, len);
    return it ? unaligned_load<elementary_feature_value>(it) : elementary_feature_unknown;
  }
};


// Definitions
template <class Map>
inline bool elementary_features<Map>::load(istream& is) {
  binary_decoder data;
  if (!compressor::load(is, data)) return false;

  try {
    maps.resize(data.next_1B());
    for (auto&& map : maps)
      map.load(data);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
