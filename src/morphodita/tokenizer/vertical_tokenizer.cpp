// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "vertical_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool vertical_tokenizer::next_sentence(vector<token_range>& tokens) {
  if (current >= chars.size() - 1) return false;

  while (true) {
    size_t line_start = current;
    while (current < chars.size() - 1 && chars[current].chr != '\r' && chars[current].chr != '\n') current++;

    size_t line_end = current;
    if (current < chars.size() - 1) {
      current++;
      if (current < chars.size() - 1 &&
          ((chars[current-1].chr == '\r' && chars[current].chr == '\n') ||
           (chars[current-1].chr == '\n' && chars[current].chr == '\r')))
        current++;
    }

    if (line_start < line_end)
      tokens.emplace_back(line_start, line_end - line_start);
    else
      break;
  }

  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
