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
struct czech_lemma_addinfo {
  inline static int raw_lemma_len(string_piece lemma);
  inline static int lemma_id_len(string_piece lemma);
  inline static string format(const unsigned char* addinfo, int addinfo_len);
  inline static bool generatable(const unsigned char* addinfo, int addinfo_len);

  inline int parse(string_piece lemma, bool die_on_failure = false);
  inline bool match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len);

  vector<unsigned char> data;
};


// Definitions
int czech_lemma_addinfo::raw_lemma_len(string_piece lemma) {
  // Lemma ends by a '-[0-9]', '`' or '_' on non-first position.
  for (unsigned len = 1; len < lemma.len; len++)
    if (lemma.str[len] == '`' || lemma.str[len] == '_' ||
        (lemma.str[len] == '-' && len+1 < lemma.len && lemma.str[len+1] >= '0' && lemma.str[len+1] <= '9'))
      return len;
  return lemma.len;
}

int czech_lemma_addinfo::lemma_id_len(string_piece lemma) {
  // Lemma ends by a '-[0-9]', '`' or '_' on non-first position.
  for (unsigned len = 1; len < lemma.len; len++) {
    if (lemma.str[len] == '`' || lemma.str[len] == '_')
      return len;
    if (lemma.str[len] == '-' && len+1 < lemma.len && lemma.str[len+1] >= '0' && lemma.str[len+1] <= '9') {
      len += 2;
      while (len < lemma.len && lemma.str[len] >= '0' && lemma.str[len] <= '9') len++;
      return len;
    }
  }
  return lemma.len;
}

string czech_lemma_addinfo::format(const unsigned char* addinfo, int addinfo_len) {
  string res;

  if (addinfo_len) {
    res.reserve(addinfo_len + 4);
    if (addinfo[0] != 255) {
      char num[5];
      sprintf(num, "-%u", addinfo[0]);
      res += num;
    }
    for (int i = 1; i < addinfo_len; i++)
      res += addinfo[i];
  }

  return res;
}

bool czech_lemma_addinfo::generatable(const unsigned char* addinfo, int addinfo_len) {
  for (int i = 1; i + 2 < addinfo_len; i++)
    if (addinfo[i] == '_' && addinfo[i+1] == ',' && addinfo[i+2] == 'x')
      return false;

  return true;
}

int czech_lemma_addinfo::parse(string_piece lemma, bool die_on_failure) {
  data.clear();

  const char* lemma_info = lemma.str + raw_lemma_len(lemma);
  if (lemma_info < lemma.str + lemma.len) {
    int lemma_num = 255;
    const char* lemma_additional_info = lemma_info;

    if (*lemma_info == '-') {
      lemma_num = strtol(lemma_info + 1, (char**) &lemma_additional_info, 10);

      if (lemma_additional_info == lemma_info + 1 || (*lemma_additional_info != '\0' && *lemma_additional_info != '`' && *lemma_additional_info != '_') || lemma_num < 0 || lemma_num >= 255) {
        if (die_on_failure)
          runtime_failure("Lemma number " << lemma_num << " in lemma " << lemma << " out of range!");
        else
          lemma_num = 255;
      }
    }
    data.emplace_back(lemma_num);
    while (lemma_additional_info < lemma.str + lemma.len)
      data.push_back(*(unsigned char*)lemma_additional_info++);

    if (data.size() > 255) {
      if (die_on_failure)
        runtime_failure("Too long lemma info " << lemma_info << " in lemma " << lemma << '!');
      else
        data.resize(255);
    }
  }

  return lemma_info - lemma.str;
}

bool czech_lemma_addinfo::match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len) {
  if (data.empty()) return true;
  if (data[0] != 255 && (!other_addinfo_len || other_addinfo[0] != data[0])) return false;
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
