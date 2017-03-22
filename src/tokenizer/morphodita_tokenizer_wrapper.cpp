// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "morphodita_tokenizer_wrapper.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "utils/getpara.h"

namespace ufal {
namespace udpipe {

morphodita_tokenizer_wrapper::morphodita_tokenizer_wrapper(morphodita::tokenizer* tokenizer, const multiword_splitter* splitter, bool normalized_spaces)
  : tokenizer(tokenizer), splitter(splitter), normalized_spaces(normalized_spaces) {}

bool morphodita_tokenizer_wrapper::read_block(istream& is, string& block) const {
  return bool(getpara(is, block));
}

void morphodita_tokenizer_wrapper::reset_document(string_piece id) {
  new_document = true;
  document_id.assign(id.str, id.len);
  preceeding_newlines = 2;
  sentence_id = 1;
  set_text("");
  saved_spaces.clear();
}

void morphodita_tokenizer_wrapper::set_text(string_piece text, bool make_copy) {
  // Start by skipping spaces and copying them to saved_spaces
  string_piece following;
  for (char32_t chr; text.len && (following = text, chr = unilib::utf8::decode(following.str, following.len),
                                  (unilib::unicode::category(chr) & unilib::unicode::Zs) || chr == '\r' || chr == '\n' || chr == '\t'); text = following)
    saved_spaces.append(text.str, following.str - text.str);

  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text = string_piece(text_copy.c_str(), text_copy.size());
  }
  this->text = text;
  tokenizer->set_text(this->text, false);
}

bool morphodita_tokenizer_wrapper::next_sentence(sentence& s, string& error) {
  unsigned following_newlines = 0;

  s.clear();
  error.clear();

  if (tokenizer->next_sentence(&forms, nullptr)) {
    // The forms returned by GRU tokenizer *should not* start/end with spaces,
    // but we trim them anyway (including all "remove empty forms/sentences" machinery).
    for (size_t i = 0; i < forms.size(); i++) {
      while (forms[i].len && (forms[i].str[0] == '\r' || forms[i].str[0] == '\n' ||
                              forms[i].str[0] == '\t' || forms[i].str[0] == ' '))
        forms[i].str++, forms[i].len--;
      while (forms[i].len && (forms[i].str[forms[i].len-1] == '\r' || forms[i].str[forms[i].len-1] == '\n' ||
                              forms[i].str[forms[i].len-1] == '\t' || forms[i].str[forms[i].len-1] == ' '))
        forms[i].len--;
      if (!forms[i].len)
        forms.erase(forms.begin() + i--);
    }
    if (!forms.size()) return next_sentence(s, error);

    for (size_t i = 0; i < forms.size(); i++) {
      // The form might contain spaces, even '\r', '\n' or '\t',
      // which we change to space. We also normalize multiple spaces to one.
      tok.form.clear();
      for (size_t j = 0; j < forms[i].len; j++) {
        char chr = forms[i].str[j];
        if (chr == '\r' || chr == '\n' || chr == '\t') chr = ' ';
        if (chr != ' ' || tok.form.empty() || tok.form.back() != ' ')
          tok.form.push_back(chr);
      }

      // Store SpaceAfter or SpacesAfter/SpacesBefore
      if (normalized_spaces)
        tok.set_space_after(!(i+1 < forms.size() && forms[i+1].str == forms[i].str + forms[i].len));
      else {
        // Fill SpacesBefore
        if (i == 0) {
          if (forms[0].str > text.str)
            saved_spaces.append(text.str, forms[0].str - text.str);
          preceeding_newlines += count(saved_spaces.begin(), saved_spaces.end(), '\n');
          tok.set_spaces_before(saved_spaces);
          saved_spaces.clear();
        } else {
          tok.set_spaces_before("");
        }
        // Fill SpacesAfter
        if (i+1 < forms.size()) {
          tok.set_spaces_after(string_piece(forms[i].str + forms[i].len, forms[i+1].str - forms[i].str - forms[i].len));
        } else {
          text.len -= forms[i].str + forms[i].len - text.str;
          text.str = forms[i].str + forms[i].len;

          string_piece following;
          for (char32_t chr; text.len && (following = text, chr = unilib::utf8::decode(following.str, following.len),
                                          (unilib::unicode::category(chr) & unilib::unicode::Zs) || chr == '\r' || chr == '\n' || chr == '\t'); text = following)
            saved_spaces.append(text.str, following.str - text.str);

          following_newlines += count(saved_spaces.begin(), saved_spaces.end(), '\n');
          tok.set_spaces_after(saved_spaces);
          saved_spaces.clear();
        }

        // SpacesInToken on every token
        if (tok.form.size() != forms[i].len)
          tok.set_spaces_in_token(forms[i]);

      }

      if (splitter)
        splitter->append_token(tok.form, tok.misc, s);
      else
        s.add_word(tok.form).misc.assign(tok.misc);
    }

    // Mark new document if needed
    if (new_document) {
      s.set_new_doc(true, document_id);
      new_document = false;
    }

    // Mark new paragraph if needed
    if (preceeding_newlines >= 2)
      s.set_new_par(true);
    preceeding_newlines = following_newlines;

    s.set_sent_id(to_string(sentence_id++));

    // Fill "# text" comment
    s.comments.emplace_back("# text = ");
    for (size_t i = 1, j = 0; i < s.words.size(); i++) {
      const token& tok = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? (const token&)s.multiword_tokens[j].form : (const token&)s.words[i].form;
      if (j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i))
        i = s.multiword_tokens[j++].id_last;

      s.comments.back().append(tok.form);
      if (i+1 < s.words.size() && tok.get_space_after()) s.comments.back().push_back(' ');
    }

    return true;
  }

  // Save unused text parts.
  if (text.len) {
    saved_spaces.append(text.str, text.len);
    text.str += text.len;
    text.len = 0;
  }

  return false;
}

} // namespace udpipe
} // namespace ufal
