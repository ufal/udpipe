// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "tag_filter.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tag_filter::tag_filter(const char* filter) {
  if (!filter) return;

  wildcard.assign(filter);
  filter = wildcard.c_str();

  for (int tag_pos = 0; *filter; tag_pos++, filter++) {
    if (*filter == '?') continue;
    if (*filter == '[') {
      filter++;

      bool negate = false;
      if (*filter == '^') negate = true, filter++;

      const char* chars = filter;
      for (bool first = true; *filter && (first || *filter != ']'); first = false) filter++;

      filters.emplace_back(tag_pos, negate, chars, filter - chars);
      if (!*filter) break;
    } else {
      filters.emplace_back(tag_pos, false, filter, 1);
    }
  }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
