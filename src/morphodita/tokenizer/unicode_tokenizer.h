// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_set>

#include "common.h"
#include "tokenizer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

class unicode_tokenizer : public tokenizer {
 public:
  unicode_tokenizer(unsigned url_email_tokenizer);

  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) override;

  virtual bool next_sentence(vector<token_range>& tokens) = 0;

 protected:
  struct char_info {
    char32_t chr;
    unilib::unicode::category_t cat;
    const char* str;

    char_info(char32_t chr, const char* str) : chr(chr), cat(unilib::unicode::category(chr)), str(str) {}
  };
  vector<char_info> chars;
  size_t current;

  bool tokenize_url_email(vector<token_range>& tokens);
  bool emergency_sentence_split(const vector<token_range>& tokens);
  bool is_eos(const vector<token_range>& tokens, char32_t eos_chr, const unordered_set<string>* abbreviations);

 private:
  unsigned url_email_tokenizer;
  string text_buffer;
  vector<token_range> tokens_buffer;
  string eos_buffer;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
