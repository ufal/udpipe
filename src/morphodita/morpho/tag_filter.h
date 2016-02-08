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

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
class tag_filter {
 public:
  tag_filter(const char* filter = nullptr);

  inline bool matches(const char* tag) const;

 private:
  struct char_filter {
    char_filter(int pos, bool negate, const char* chars, int len) : pos(pos), negate(negate), chars(chars), len(len) {}

    int pos;
    bool negate;
    const char* chars;
    int len;
  };

  string wildcard;
  std::vector<char_filter> filters;
};

// Definitions
inline bool tag_filter::matches(const char* tag) const {
  if (filters.empty()) return true;

  int tag_pos = 0;
  for (auto&& filter : filters) {
    while (tag_pos < filter.pos)
      if (!tag[tag_pos++])
        return true;

    // We assume filter.len >= 1.
    bool matched = (*filter.chars == tag[tag_pos]) ^ filter.negate;
    for (int i = 1; i < filter.len && !matched; i++)
      matched = (filter.chars[i] == tag[tag_pos]) ^ filter.negate;
    if (!matched) return false;
  }
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
