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

  for (int tag_pos = 0, filter_pos = 0; filter[filter_pos]; tag_pos++, filter_pos++) {
    if (filter[filter_pos] == '?') continue;
    if (filter[filter_pos] == '[') {
      filter_pos++;

      bool negate = false;
      if (filter[filter_pos] == '^') negate = true, filter_pos++;

      int chars_start = filter_pos;
      for (bool first = true; filter[filter_pos] && (first || filter[filter_pos] != ']'); first = false)
        filter_pos++;

      filters.emplace_back(tag_pos, negate, chars_start, filter_pos - chars_start);
      if (!filter[filter_pos]) break;
    } else {
      filters.emplace_back(tag_pos, false, filter_pos, 1);
    }
  }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
