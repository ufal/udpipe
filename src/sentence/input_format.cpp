// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "input_format.h"
#include "utils/iostreams.h"
#include "utils/parse_int.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {

// Input CoNLL-U format
class input_format_conllu : public input_format {
 public:
  virtual bool read_block(istream& is, string& block) const override;
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(sentence& s, string& error) override;

 private:
  string_piece text;
  string text_copy;
};

bool input_format_conllu::read_block(istream& is, string& block) const {
  return getpara(is, block);
}

void input_format_conllu::set_text(string_piece text, bool make_copy) {
  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text = string_piece(text_copy.c_str(), text_copy.size());
  }
  this->text = text;
}

bool input_format_conllu::next_sentence(sentence& s, string& error) {
  error.clear();
  s.clear();
  int last_multiword_token = 0;

  vector<string_piece> tokens, parts;
  while (text.len) {
    // Read line
    string_piece line(text.str, 0);
    while (line.len < text.len && line.str[line.len] != '\n') line.len++;
    text.str += line.len + (line.len < text.len);
    text.len -= line.len + (line.len < text.len);

    // Empty lines denote end of tree, unless at the beginning
    if (!line.len) {
      if (s.empty()) continue;
      break;
    }

    if (*line.str == '#') {
      // Store comments at the beginning and ignore the rest
      if (s.empty()) s.comments.emplace_back(line.str, line.len);
      continue;
    }

    // Parse the line
    split(line, '\t', tokens);
    if (tokens.size() != 10)
      return error.assign("The CoNLL-U line '").append(line.str, line.len).append("' does not contain 10 columns!") , false;

    // Handle multiword tokens
    if (memchr(tokens[0].str, '-', tokens[0].len)) {
      split(tokens[0], '-', parts);
      if (parts.size() != 2)
        return error.assign("Cannot parse ID of multiword token '").append(line.str, line.len).append("'!") , false;
      int from, to;
      if (!parse_int(parts[0], "CoNLL-U id", from, error) || !parse_int(parts[1], "CoNLL-U id", to, error))
        return false;
      if (from != int(s.words.size()))
        return error.assign("Incorrect ID '").append(parts[0].str, parts[0].len).append("' of multiword token '").append(line.str, line.len).append("'!"), false;
      if (to < from)
        return error.assign("Incorrect range '").append(tokens[0].str, tokens[0].len).append("' of multiword token '").append(line.str, line.len).append("'!"), false;
      if (from <= last_multiword_token)
        return error.assign("Multiword token '").append(line.str, line.len).append("' overlaps with the previous one!"), false;
      last_multiword_token = to;
      s.multiword_tokens.emplace_back(from, to, string(tokens[1].str, tokens[1].len));
      continue;
    }

    // Parse word ID and head
    int id;
    if (!parse_int(tokens[0], "CoNLL-U id", id, error))
      return false;
    if (id != int(s.words.size()))
      return error.assign("Incorrect ID '").append(tokens[0].str, tokens[0].len).append("' of CoNLL-U line '").append(line.str, line.len).append("'!"), false;

    int head;
    if (tokens[6].len == 1 && tokens[6].str[0] == '_') {
      head = -1;
    } else {
      if (!parse_int(tokens[6], "CoNLL-U head", head, error))
        return false;
      if (head < 0)
        return error.assign("Numeric head value '").append(tokens[0].str, tokens[0].len).append("' cannot be negative!"), false;
    }

    // Add new word
    auto& word = s.add_word(string(tokens[1].str, tokens[1].len));
    word.lemma.assign(tokens[2].str, tokens[2].len);
    if (!(tokens[3].len == 1 && tokens[3].str[0] == '_')) word.upostag.assign(tokens[3].str, tokens[3].len);
    if (!(tokens[4].len == 1 && tokens[4].str[0] == '_')) word.xpostag.assign(tokens[4].str, tokens[4].len);
    if (!(tokens[5].len == 1 && tokens[5].str[0] == '_')) word.feats.assign(tokens[5].str, tokens[5].len);
    word.head = head;
    if (!(tokens[7].len == 1 && tokens[7].str[0] == '_')) word.deprel.assign(tokens[7].str, tokens[7].len);
    if (!(tokens[8].len == 1 && tokens[8].str[0] == '_')) word.deps.assign(tokens[8].str, tokens[8].len);
    if (!(tokens[9].len == 1 && tokens[9].str[0] == '_')) word.misc.assign(tokens[9].str, tokens[9].len);
  }

  // Check that we got word for the last multiword token
  if (last_multiword_token >= int(s.words.size()))
    return error.assign("There are words missing for multiword token '").append(s.multiword_tokens.back().form).append("'!"), false;

  // Set heads correctly
  for (auto&& word : s.words)
    if (word.id && word.head >= 0) {
      if (word.head >= int(s.words.size()))
        return error.assign("Node ID '").append(to_string(word.id)).append("' form '").append(word.form).append("' has too large head: '").append(to_string(word.head)).append("'!"), false;
      s.set_head(word.id, word.head, word.deprel);
    }

  return !s.empty();
}

// Static factory methods
input_format* input_format::new_conllu_input_format() {
  return new input_format_conllu();
}

input_format* input_format::new_input_format(const string& name) {
  if (name == "conllu") return new_conllu_input_format();
  return nullptr;
}

} // namespace udpipe
} // namespace ufal
