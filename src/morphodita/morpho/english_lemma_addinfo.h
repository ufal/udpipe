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
#include "small_stringops.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
struct english_lemma_addinfo {
  inline static int raw_lemma_len(string_piece lemma);
  inline static int lemma_id_len(string_piece lemma);
  inline static string format(const unsigned char* addinfo, int addinfo_len);
  inline static bool generatable(const unsigned char* addinfo, int addinfo_len);

  inline int parse(string_piece lemma, bool die_on_failure = false);
  inline bool match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len);

  vector<unsigned char> data;
};


// Definitions
int english_lemma_addinfo::raw_lemma_len(string_piece lemma) {
  // Lemma ends either by
  // - '^' on non-first position followed by nothing or [A-Za-z][-A-Za-z]*
  // - '+' on non-first position followed by nothing
  for (unsigned len = 1; len < lemma.len; len++) {
    if (len + 1 == lemma.len && (lemma.str[len] == '^' || lemma.str[len] == '+'))
      return len;
    if (len + 1 < lemma.len && lemma.str[len] == '^') {
      bool ok = true;
      for (unsigned i = len + 1; ok && i < lemma.len; i++)
        ok &= (lemma.str[i] >= 'A' && lemma.str[i] <= 'Z') ||
            (lemma.str[i] >= 'a' && lemma.str[i] <= 'z') ||
            (i > len + 1 && lemma.str[i] == '-');
      if (ok) return len;
    }
  }
  return lemma.len;
}

int english_lemma_addinfo::lemma_id_len(string_piece lemma) {
  // No lemma comments.
  return lemma.len;
}

string english_lemma_addinfo::format(const unsigned char* addinfo, int addinfo_len) {
  return string((const char*) addinfo, addinfo_len);
}

bool english_lemma_addinfo::generatable(const unsigned char* /*addinfo*/, int /*addinfo_len*/) {
  return true;
}

int english_lemma_addinfo::parse(string_piece lemma, bool /*die_on_failure*/) {
  data.clear();

  size_t len = raw_lemma_len(lemma);
  for (size_t i = len; i < lemma.len; i++)
    data.push_back(lemma.str[i]);

  return len;
}

bool english_lemma_addinfo::match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len) {
  if (data.empty()) return true;
  if (data.size() == 1 && data[0] == '^') return other_addinfo_len > 0 && other_addinfo[0] == '^';
  if (data.size() == 1 && data[0] == '+') return other_addinfo_len == 0;
  return data.size() == size_t(other_addinfo_len) && small_memeq(data.data(), other_addinfo, other_addinfo_len);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
