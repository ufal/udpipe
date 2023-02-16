// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "casing_variants.h"
#include "generic_morpho.h"
#include "generic_lemma_addinfo.h"
#include "morpho_dictionary.h"
#include "morpho_prefix_guesser.h"
#include "morpho_statistical_guesser.h"
#include "tag_filter.h"
#include "morphodita/tokenizer/generic_tokenizer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool generic_morpho::load(istream& is) {
  binary_decoder data;
  if (!compressor::load(is, data)) return false;

  try {
    // Load tags
    unsigned length = data.next_1B();
    unknown_tag.assign(data.next<char>(length), length);
    length = data.next_1B();
    number_tag.assign(data.next<char>(length), length);
    length = data.next_1B();
    punctuation_tag.assign(data.next<char>(length), length);
    length = data.next_1B();
    symbol_tag.assign(data.next<char>(length), length);

    // Load dictionary
    dictionary.load(data);

    // Optionally statistical guesser if present
    statistical_guesser.reset();
    if (data.next_1B()) {
      statistical_guesser.reset(new morpho_statistical_guesser());
      statistical_guesser->load(data);
    }
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

int generic_morpho::analyze(string_piece form, guesser_mode guesser, vector<tagged_lemma>& lemmas) const {
  lemmas.clear();

  if (form.len) {
    // Generate all casing variants if needed (they are different than given form).
    string form_uclc; // first uppercase, rest lowercase
    string form_lc;   // all lowercase
    generate_casing_variants(form, form_uclc, form_lc);

    // Start by analysing using the dictionary and all casing variants.
    dictionary.analyze(form, lemmas);
    if (!form_uclc.empty()) dictionary.analyze(form_uclc, lemmas);
    if (!form_lc.empty()) dictionary.analyze(form_lc, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // Then call analyze_special to handle numbers, punctuation and symbols.
    analyze_special(form, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // For the statistical guesser, use all casing variants.
    if (guesser == GUESSER && statistical_guesser) {
      if (form_uclc.empty() && form_lc.empty())
        statistical_guesser->analyze(form, lemmas, nullptr);
      else {
        morpho_statistical_guesser::used_rules used_rules; used_rules.reserve(3);
        statistical_guesser->analyze(form, lemmas, &used_rules);
        if (!form_uclc.empty()) statistical_guesser->analyze(form_uclc, lemmas, &used_rules);
        if (!form_lc.empty()) statistical_guesser->analyze(form_lc, lemmas, &used_rules);
      }
    }
    if (!lemmas.empty()) return GUESSER;
  }

  lemmas.emplace_back(string(form.str, form.len), unknown_tag);
  return -1;
}

int generic_morpho::generate(string_piece lemma, const char* tag_wildcard, morpho::guesser_mode /*guesser*/, vector<tagged_lemma_forms>& forms) const {
  forms.clear();

  tag_filter filter(tag_wildcard);

  if (lemma.len) {
    if (dictionary.generate(lemma, filter, forms))
      return NO_GUESSER;
  }

  return -1;
}

int generic_morpho::raw_lemma_len(string_piece lemma) const {
  return generic_lemma_addinfo::raw_lemma_len(lemma);
}

int generic_morpho::lemma_id_len(string_piece lemma) const {
  return generic_lemma_addinfo::lemma_id_len(lemma);
}

int generic_morpho::raw_form_len(string_piece form) const {
  return form.len;
}

tokenizer* generic_morpho::new_tokenizer() const {
  return new generic_tokenizer(version);
}

void generic_morpho::analyze_special(string_piece form, vector<tagged_lemma>& lemmas) const {
  using namespace unilib;

  // Analyzer for numbers, punctuation and symbols.
  // Number is anything matching [+-]? is_Pn* ([.,] is_Pn*)? ([Ee] [+-]? is_Pn+)? for at least one is_Pn* nonempty.
  // Punctuation is any form beginning with either unicode punctuation or punctuation_exceptions character.
  // Beware that numbers takes precedence, so - is punctuation, -3 is number, -. is punctuation, -.3 is number.
  if (!form.len) return;

  string_piece number = form;
  char32_t first = utf8::decode(number.str, number.len);

  // Try matching a number.
  char32_t codepoint = first;
  bool any_digit = false;
  if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(number.str, number.len);
  while (unicode::category(codepoint) & unicode::N) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  if ((codepoint == '.' && number.len) || codepoint == ',') codepoint = utf8::decode(number.str, number.len);
  while (unicode::category(codepoint) & unicode::N) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  if (any_digit && (codepoint == 'e' || codepoint == 'E')) {
    codepoint = utf8::decode(number.str, number.len);
    if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(number.str, number.len);
    any_digit = false;
    while (unicode::category(codepoint) & unicode::N) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  }

  if (any_digit && !number.len && (!codepoint || codepoint == '.')) {
    lemmas.emplace_back(string(form.str, form.len), number_tag);
    return;
  }

  // Try matching punctuation or symbol.
  bool punctuation = true, symbol = true;
  string_piece form_ori = form;
  while (form.len) {
    codepoint = utf8::decode(form.str, form.len);
    punctuation = punctuation && unicode::category(codepoint) & unicode::P;
    symbol = symbol && unicode::category(codepoint) & unicode::S;
  }
  if (punctuation)
    lemmas.emplace_back(string(form_ori.str, form_ori.len), punctuation_tag);
  else if (symbol)
    lemmas.emplace_back(string(form_ori.str, form_ori.len), symbol_tag);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
