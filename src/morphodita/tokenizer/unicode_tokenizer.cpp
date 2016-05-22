// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "ragel_tokenizer.h"
#include "unicode_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

unicode_tokenizer::unicode_tokenizer(unsigned url_email_tokenizer) : url_email_tokenizer(url_email_tokenizer) {
  ragel_tokenizer::initialize_ragel_map();

  set_text(string_piece(nullptr, 0));
}

void unicode_tokenizer::set_text(string_piece text, bool make_copy /*= false*/) {
  using namespace unilib;

  if (make_copy && text.str) {
    text_buffer.assign(text.str, text.len);
    text.str = text_buffer.c_str();
  }
  current = 0;

  chars.clear();
  for (const char* curr_str = text.str; text.len; curr_str = text.str)
    chars.emplace_back(utf8::decode(text.str, text.len), curr_str);
  chars.emplace_back(0, text.str);
}

bool unicode_tokenizer::next_sentence(vector<string_piece>* forms, vector<token_range>* tokens_ptr) {
  vector<token_range>& tokens = tokens_ptr ? *tokens_ptr : tokens_buffer;
  tokens.clear();
  if (forms) forms->clear();
  if (current >= chars.size() - 1) return false;

  bool result = next_sentence(tokens);
  if (forms)
    for (auto&& token : tokens)
      forms->emplace_back(chars[token.start].str, chars[token.start + token.length].str - chars[token.start].str);

  return result;
}

bool unicode_tokenizer::tokenize_url_email(vector<token_range>& tokens) {
  if (current >= chars.size() - 1) return false;
  return url_email_tokenizer ? ragel_tokenizer::ragel_url_email(url_email_tokenizer, chars, current, tokens) : false;
}

bool unicode_tokenizer::emergency_sentence_split(const vector<token_range>& tokens) {
  using namespace unilib;

  // Implement emergency splitting for large sentences
  return tokens.size() >= 500 ||
         (tokens.size() >= 450 && chars[tokens.back().start].cat & unicode::P) ||
         (tokens.size() >= 400 && chars[tokens.back().start].cat & unicode::Po);
}

bool unicode_tokenizer::is_eos(const vector<token_range>& tokens, char32_t eos_chr, const unordered_set<string>* abbreviations) {
  using namespace unilib;

  if (eos_chr == '.' && !tokens.empty()) {
    // Ignore one-letter capitals before dot
    if (tokens.back().length == 1 && chars[tokens.back().start].cat & unicode::Lut)
      return false;

    // Ignore specified abbreviations
    if (abbreviations) {
      eos_buffer.clear();
      for (size_t i = 0; i < tokens.back().length; i++)
        utf8::append(eos_buffer, unicode::lowercase(chars[tokens.back().start + i].chr));
      if (abbreviations->count(eos_buffer))
        return false;
    }
  }
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
