// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "external_morpho.h"
#include "tag_filter.h"
#include "morphodita/tokenizer/generic_tokenizer.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool external_morpho::load(istream& is) {
  binary_decoder data;
  if (!compressor::load(is, data)) return false;

  try {
    // Load unknown_tag
    unsigned length = data.next_1B();
    unknown_tag.assign(data.next<char>(length), length);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

int external_morpho::analyze(string_piece form, guesser_mode /*guesser*/, vector<tagged_lemma>& lemmas) const {
  lemmas.clear();

  if (form.len) {
    // Start by skipping the first form
    string_piece lemmatags = form;
    while (lemmatags.len && *lemmatags.str != ' ') lemmatags.len--, lemmatags.str++;
    if (lemmatags.len) lemmatags.len--, lemmatags.str++;

    // Split lemmatags using ' ' into lemma-tag pairs.
    while (lemmatags.len) {
      auto lemma_start = lemmatags.str;
      while (lemmatags.len && *lemmatags.str != ' ') lemmatags.len--, lemmatags.str++;
      if (!lemmatags.len) break;
      auto lemma_len = lemmatags.str - lemma_start;
      lemmatags.len--, lemmatags.str++;

      auto tag_start = lemmatags.str;
      while (lemmatags.len && *lemmatags.str != ' ') lemmatags.len--, lemmatags.str++;
      auto tag_len = lemmatags.str - tag_start;
      if (lemmatags.len) lemmatags.len--, lemmatags.str++;

      lemmas.emplace_back(string(lemma_start, lemma_len), string(tag_start, tag_len));
    }

    if (!lemmas.empty()) return NO_GUESSER;
  }

  lemmas.emplace_back(string(form.str, form.len), unknown_tag);
  return -1;
}

int external_morpho::generate(string_piece lemma, const char* tag_wildcard, morpho::guesser_mode /*guesser*/, vector<tagged_lemma_forms>& forms) const {
  forms.clear();

  tag_filter filter(tag_wildcard);

  if (lemma.len) {
    // Start by locating the lemma
    string_piece formtags = lemma;
    while (formtags.len && *formtags.str != ' ') formtags.len--, formtags.str++;
    string_piece real_lemma(lemma.str, lemma.len - formtags.len);
    if (formtags.len) formtags.len--, formtags.str++;

    // Split formtags using ' ' into form-tag pairs.
    bool any_result = false;
    while (formtags.len) {
      auto form_start = formtags.str;
      while (formtags.len && *formtags.str != ' ') formtags.len--, formtags.str++;
      if (!formtags.len) break;
      auto form_len = formtags.str - form_start;
      formtags.len--, formtags.str++;

      auto tag_start = formtags.str;
      while (formtags.len && *formtags.str != ' ') formtags.len--, formtags.str++;
      auto tag_len = formtags.str - tag_start;
      if (formtags.len) formtags.len--, formtags.str++;

      any_result = true;
      string tag(tag_start, tag_len);
      if (filter.matches(tag.c_str())) {
        if (forms.empty()) forms.emplace_back(string(real_lemma.str, real_lemma.len));
        forms.back().forms.emplace_back(string(form_start, form_len), tag);
      }
    }

    if (any_result) return NO_GUESSER;
  }

  return -1;
}

int external_morpho::raw_lemma_len(string_piece lemma) const {
  unsigned lemma_len = 0;
  while (lemma_len < lemma.len && lemma.str[lemma_len] != ' ') lemma_len++;
  return lemma_len;
}

int external_morpho::lemma_id_len(string_piece lemma) const {
  unsigned lemma_len = 0;
  while (lemma_len < lemma.len && lemma.str[lemma_len] != ' ') lemma_len++;
  return lemma_len;
}

int external_morpho::raw_form_len(string_piece form) const {
  unsigned form_len = 0;
  while (form_len < form.len && form.str[form_len] != ' ') form_len++;
  return form_len;
}

tokenizer* external_morpho::new_tokenizer() const {
  return new generic_tokenizer(version);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
