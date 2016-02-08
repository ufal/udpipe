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
#include "utils/string_piece.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

inline void generate_casing_variants(string_piece form, string& form_uclc, string& form_lc) {
  using namespace unilib;

  // Detect uppercase+titlecase characters.
  bool first_Lut = false; // first character is uppercase or titlecase
  bool rest_has_Lut = false; // any character but first is uppercase or titlecase
  {
    string_piece form_tmp = form;
    first_Lut = unicode::category(utf8::decode(form_tmp.str, form_tmp.len)) & unicode::Lut;
    while (form_tmp.len && !rest_has_Lut)
      rest_has_Lut = unicode::category(utf8::decode(form_tmp.str, form_tmp.len)) & unicode::Lut;
  }

  // Generate all casing variants if needed (they are different than given form).
  // We only replace letters with their lowercase variants.
  // - form_uclc: first uppercase, rest lowercase
  // - form_lc: all lowercase

  if (first_Lut && !rest_has_Lut) { // common case allowing fast execution
    form_lc.reserve(form.len);
    string_piece form_tmp = form;
    utf8::append(form_lc, unicode::lowercase(utf8::decode(form_tmp.str, form_tmp.len)));
    form_lc.append(form_tmp.str, form_tmp.len);
  } else if (!first_Lut && rest_has_Lut) {
    form_lc.reserve(form.len);
    utf8::map(unicode::lowercase, form.str, form.len, form_lc);
  } else if (first_Lut && rest_has_Lut) {
    form_lc.reserve(form.len);
    form_uclc.reserve(form.len);
    string_piece form_tmp = form;
    char32_t first = utf8::decode(form_tmp.str, form_tmp.len);
    utf8::append(form_lc, unicode::lowercase(first));
    utf8::append(form_uclc, first);
    while (form_tmp.len) {
      char32_t lowercase = unicode::lowercase(utf8::decode(form_tmp.str, form_tmp.len));
      utf8::append(form_lc, lowercase);
      utf8::append(form_uclc, lowercase);
    }
  }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
