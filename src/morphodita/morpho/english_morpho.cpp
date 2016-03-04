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
#include "english_morpho.h"
#include "tag_filter.h"
#include "morphodita/tokenizer/english_tokenizer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool english_morpho::load(istream& is) {
  binary_decoder data;
  if (!compressor::load(is, data)) return false;

  try {
    dictionary.load(data);
    morpho_guesser.load(data);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

int english_morpho::analyze(string_piece form, guesser_mode guesser, vector<tagged_lemma>& lemmas) const {
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
    if (!lemmas.empty())
      return guesser == NO_GUESSER || !morpho_guesser.analyze_proper_names(form, form_lc.empty() ? form : form_lc, lemmas) ? NO_GUESSER : GUESSER;

    // Then call analyze_special to handle numbers, punctuation and symbols.
    analyze_special(form, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // Use English guesser on form_lc if allowed.
    if (guesser == GUESSER)
      morpho_guesser.analyze(form, form_lc.empty() ? form : form_lc, lemmas);
    if (!lemmas.empty()) return GUESSER;
  }

  lemmas.emplace_back(string(form.str, form.len), unknown_tag);
  return -1;
}

int english_morpho::generate(string_piece lemma, const char* tag_wildcard, morpho::guesser_mode /*guesser*/, vector<tagged_lemma_forms>& forms) const {
  forms.clear();

  tag_filter filter(tag_wildcard);

  if (lemma.len) {
    if (dictionary.generate(lemma, filter, forms))
      return NO_GUESSER;
  }

  return -1;
}

int english_morpho::raw_lemma_len(string_piece lemma) const {
  return english_lemma_addinfo::raw_lemma_len(lemma);
}

int english_morpho::lemma_id_len(string_piece lemma) const {
  return english_lemma_addinfo::lemma_id_len(lemma);
}

int english_morpho::raw_form_len(string_piece form) const {
  return form.len;
}

tokenizer* english_morpho::new_tokenizer() const {
  return new english_tokenizer(version <= 2 ? 1 : 2);
}

void english_morpho::analyze_special(string_piece form, vector<tagged_lemma>& lemmas) const {
  using namespace unilib;

  // Analyzer for numbers and punctuation.
  if (!form.len) return;

  // One-letter punctuation exceptions.
  if (form.len == 1)
    switch(*form.str) {
      case '.':
      case '!':
      case '?': lemmas.emplace_back(string(form.str, form.len), dot_tag); return;
      case ',': lemmas.emplace_back(string(form.str, form.len), comma_tag); return;
      case '#': lemmas.emplace_back(string(form.str, form.len), hash_tag); return;
      case '$': lemmas.emplace_back(string(form.str, form.len), dollar_tag); return;
      case '[': lemmas.emplace_back(string(form.str, form.len), sym_tag); return;
      case ']': lemmas.emplace_back(string(form.str, form.len), sym_tag); return;
      case '%': lemmas.emplace_back(string(form.str, form.len), jj_tag);
                lemmas.emplace_back(string(form.str, form.len), nn_tag); return;
      case '&': lemmas.emplace_back(string(form.str, form.len), cc_tag);
                lemmas.emplace_back(string(form.str, form.len), sym_tag); return;
      case '*': lemmas.emplace_back(string(form.str, form.len), sym_tag);
                lemmas.emplace_back(string(form.str, form.len), nn_tag); return;
      case '@': lemmas.emplace_back(string(form.str, form.len), sym_tag);
                lemmas.emplace_back(string(form.str, form.len), in_tag); return;
      case '\'': lemmas.emplace_back(string(form.str, form.len), close_quotation_tag);
                 lemmas.emplace_back(string(form.str, form.len), pos_tag); return;
    }

  // Try matching a number: [+-]? is_Pn* (, is_Pn{3})? (. is_Pn*)? (s | [Ee] [+-]? is_Pn+)? with at least one digit
  string_piece number = form;
  char32_t codepoint = utf8::decode(number.str, number.len);
  bool any_digit = false;
  if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(number.str, number.len);
  while (unicode::category(codepoint) & unicode::N) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  while (codepoint == ',') {
    string_piece group = number;
    if (unicode::category(utf8::decode(group.str, group.len) & ~unicode::N)) break;
    if (unicode::category(utf8::decode(group.str, group.len) & ~unicode::N)) break;
    if (unicode::category(utf8::decode(group.str, group.len) & ~unicode::N)) break;
    any_digit = true;
    number = group;
    codepoint = utf8::decode(number.str, number.len);
  }
  if (codepoint == '.' && number.len) {
    codepoint = utf8::decode(number.str, number.len);
    while (unicode::category(codepoint) & unicode::N) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  }
  if (version >= 2 && any_digit && codepoint == 's' && !number.len) {
    lemmas.emplace_back(string(form.str, form.len), number_tag);
    lemmas.emplace_back(string(form.str, form.len - 1), nns_tag);
    return;
  }
  if (any_digit && (codepoint == 'e' || codepoint == 'E')) {
    codepoint = utf8::decode(number.str, number.len);
    if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(number.str, number.len);
    any_digit = false;
    while (unicode::category(codepoint) & unicode::N) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  }
  if (any_digit && !number.len && (!codepoint || codepoint == '.')) {
    lemmas.emplace_back(string(form.str, form.len - (codepoint == '.')), number_tag);
    lemmas.emplace_back(string(form.str, form.len - (codepoint == '.')), nnp_tag);
    if (form.len == 1 + (codepoint == '.') && *form.str >= '1' && *form.str <= '9')
      lemmas.emplace_back(string(form.str, form.len - (codepoint == '.')), ls_tag);
    return;
  }

  // Open quotation, end quotation, open parentheses, end parentheses, symbol, or other
  string_piece punctuation = form;
  bool open_quotation = true, close_quotation = true, open_parenthesis = true, close_parenthesis = true, any_punctuation = true, symbol = true;
  while ((symbol || any_punctuation) && punctuation.len) {
    codepoint = utf8::decode(punctuation.str, punctuation.len);
    if (open_quotation) open_quotation = codepoint == '`' || unicode::category(codepoint) & unicode::Pi;
    if (close_quotation) close_quotation = codepoint == '\'' || codepoint == '"' || unicode::category(codepoint) & unicode::Pf;
    if (open_parenthesis) open_parenthesis = unicode::category(codepoint) & unicode::Ps;
    if (close_parenthesis) close_parenthesis = unicode::category(codepoint) & unicode::Pe;
    if (any_punctuation) any_punctuation = unicode::category(codepoint) & unicode::P;
    if (symbol) symbol = codepoint == '*' || unicode::category(codepoint) & unicode::S;
  }
  if (!punctuation.len && open_quotation) { lemmas.emplace_back(string(form.str, form.len), open_quotation_tag); return; }
  if (!punctuation.len && close_quotation) { lemmas.emplace_back(string(form.str, form.len), close_quotation_tag); return; }
  if (!punctuation.len && open_parenthesis) { lemmas.emplace_back(string(form.str, form.len), open_parenthesis_tag); return; }
  if (!punctuation.len && close_parenthesis) { lemmas.emplace_back(string(form.str, form.len), close_parenthesis_tag); return; }
  if (!punctuation.len && symbol) { lemmas.emplace_back(string(form.str, form.len), sym_tag); return; }
  if (!punctuation.len && any_punctuation) { lemmas.emplace_back(string(form.str, form.len), punctuation_tag); return; }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
