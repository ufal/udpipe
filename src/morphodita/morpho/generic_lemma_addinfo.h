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
struct generic_lemma_addinfo {
  inline static int raw_lemma_len(string_piece lemma);
  inline static int lemma_id_len(string_piece lemma);
  inline static string format(const unsigned char* addinfo, int addinfo_len);
  inline static bool generatable(const unsigned char* addinfo, int addinfo_len);

  inline int parse(string_piece lemma, bool die_on_failure = false);
  inline bool match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len);

  vector<unsigned char> data;
};


// Definitions
int generic_lemma_addinfo::raw_lemma_len(string_piece lemma) {
  return lemma.len;
}

int generic_lemma_addinfo::lemma_id_len(string_piece lemma) {
  return lemma.len;
}

string generic_lemma_addinfo::format(const unsigned char* /*addinfo*/, int /*addinfo_len*/) {
  return string();
}

bool generic_lemma_addinfo::generatable(const unsigned char* /*addinfo*/, int /*addinfo_len*/) {
  return true;
}

int generic_lemma_addinfo::parse(string_piece lemma, bool /*die_on_failure*/) {
  return lemma.len;
}

bool generic_lemma_addinfo::match_lemma_id(const unsigned char* /*other_addinfo*/, int /*other_addinfo_len*/) {
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
