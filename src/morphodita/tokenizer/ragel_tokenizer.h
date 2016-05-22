// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <atomic>

#include "common.h"
#include "unicode_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class ragel_tokenizer : public unicode_tokenizer {
 public:
  ragel_tokenizer(unsigned url_email_tokenizer);

 protected:
  static inline uint8_t ragel_char(const char_info& chr);

 private:
  static void initialize_ragel_map();
  static vector<uint8_t> ragel_map;
  static atomic_flag ragel_map_flag;
  static void ragel_map_add(char32_t chr, uint8_t mapping);

  friend class unicode_tokenizer;
  static bool ragel_url_email(unsigned version, const vector<char_info>& chars, size_t& current_char, vector<token_range>& tokens);
};

uint8_t ragel_tokenizer::ragel_char(const char_info& chr) {
  return chr.chr < ragel_map.size() && ragel_map[chr.chr] != 128 ? ragel_map[chr.chr] : 128 + (uint32_t(chr.cat) * uint32_t(0x077CB531U) >> 27);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
