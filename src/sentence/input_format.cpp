// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "input_format.h"
#include "utils/getpara.h"
#include "utils/parse_int.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {

// CoNLL-U input format
class input_format_conllu : public input_format {
 public:
  virtual bool read_block(istream& is, string& block) const override;
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(sentence& s, string& error) override;

 private:
  string_piece text;
  string text_copy;

  static const string columns[10];
};

const string input_format_conllu::columns[10] = {"ID", "FORM", "LEMMA",
  "UPOS", "XPOS", "FEATS", "HEAD", "DEPREL", "DEPS", "MISC"};

bool input_format_conllu::read_block(istream& is, string& block) const {
  return bool(getpara(is, block));
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

    // Check that no column is empty and contains no spaces (except FORM and LEMMA)
    for (int i = 0; i < 10; i++) {
      if (!tokens[i].len)
        return error.assign("The CoNLL-U line '").append(line.str, line.len).append("' contains empty column ").append(columns[i]).append("!"), false;
      if (i != 1 && i != 2 && memchr(tokens[i].str, ' ', tokens[i].len) != NULL)
        return error.assign("The CoNLL-U line '").append(line.str, line.len).append("' contains spaces in column ").append(columns[i]).append("!"), false;
    }

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
      for (int i = 2; i < 9; i++)
        if (tokens[i].len != 1 || tokens[i].str[0] != '_')
          return error.assign("Column ").append(columns[i]).append(" of an multi-word token '").append(line.str, line.len).append("' is not an empty!"), false;
      s.multiword_tokens.emplace_back(from, to, tokens[1], tokens[9].len == 1 && tokens[9].str[0] == '_' ? string_piece() : tokens[9]);
      continue;
    }

    // Handle empty nodes
    if (memchr(tokens[0].str, '.', tokens[0].len)) {
      split(tokens[0], '.', parts);
      if (parts.size() != 2)
        return error.assign("Cannot parse ID of empty node '").append(line.str, line.len).append("'!") , false;
      int id, index;
      if (!parse_int(parts[0], "CoNLL-U empty node id", id, error) || !parse_int(parts[1], "CoNLL-U empty node index", index, error))
        return false;
      if (id != int(s.words.size()) - 1)
        return error.assign("Incorrect ID '").append(parts[0].str, parts[0].len).append("' of empty node token '").append(line.str, line.len).append("'!"), false;
      if (!((s.empty_nodes.empty() && index == 1) || (!s.empty_nodes.empty() && s.empty_nodes.back().id < id && index == 1) ||
           (!s.empty_nodes.empty() && s.empty_nodes.back().id == id && index == s.empty_nodes.back().index + 1)))
        return error.assign("Incorrect ID index '").append(parts[1].str, parts[1].len).append("' of empty node token '").append(line.str, line.len).append("'!"), false;
      for (int i = 6; i < 8; i++)
        if (tokens[i].len != 1 || tokens[i].str[0] != '_')
          return error.assign("Column ").append(columns[i]).append(" of an empty node token '").append(line.str, line.len).append("' is not an empty!"), false;

      s.empty_nodes.emplace_back(id, index);
      s.empty_nodes.back().form.assign(tokens[1].str, tokens[1].len);
      s.empty_nodes.back().lemma.assign(tokens[2].str, tokens[2].len);
      if (!(tokens[3].len == 1 && tokens[3].str[0] == '_')) s.empty_nodes.back().upostag.assign(tokens[3].str, tokens[3].len);
      if (!(tokens[4].len == 1 && tokens[4].str[0] == '_')) s.empty_nodes.back().xpostag.assign(tokens[4].str, tokens[4].len);
      if (!(tokens[5].len == 1 && tokens[5].str[0] == '_')) s.empty_nodes.back().feats.assign(tokens[5].str, tokens[5].len);
      if (!(tokens[8].len == 1 && tokens[8].str[0] == '_')) s.empty_nodes.back().deps.assign(tokens[8].str, tokens[8].len);
      if (!(tokens[9].len == 1 && tokens[9].str[0] == '_')) s.empty_nodes.back().misc.assign(tokens[9].str, tokens[9].len);
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
    auto& word = s.add_word(tokens[1]);
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

// Horizontal input format
class input_format_horizontal : public input_format {
 public:
  virtual bool read_block(istream& is, string& block) const override;
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(sentence& s, string& error) override;

 private:
  string_piece text;
  string text_copy;
};

bool input_format_horizontal::read_block(istream& is, string& block) const {
  return bool(getline(is, block));
}

void input_format_horizontal::set_text(string_piece text, bool make_copy) {
  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text = string_piece(text_copy.c_str(), text_copy.size());
  }
  this->text = text;
}

bool input_format_horizontal::next_sentence(sentence& s, string& error) {
  error.clear();
  s.clear();

  // Skip spaces and newlines
  while (text.len && (*text.str == ' ' || *text.str == '\t' || *text.str == '\r' || *text.str == '\n'))
    text.str++, text.len--;

  // Read space (and tab) separated words
  while (text.len && *text.str != '\r' && *text.str != '\n') {
    string_piece word = text;

    // Slurp the word
    while (text.len && *text.str != ' ' && *text.str != '\t' && *text.str != '\r' && *text.str != '\n')
      text.str++, text.len--;
    word.len = text.str - word.str;
    s.add_word(word);

    // Skip spaces
    while (text.len && (*text.str == ' ' || *text.str == '\t'))
      text.str++, text.len--;
  }

  return !s.empty();
}

// Vertical input format
class input_format_vertical : public input_format {
 public:
  virtual bool read_block(istream& is, string& block) const override;
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(sentence& s, string& error) override;

 private:
  string_piece text;
  string text_copy;
};

bool input_format_vertical::read_block(istream& is, string& block) const {
  return bool(getpara(is, block));
}

void input_format_vertical::set_text(string_piece text, bool make_copy) {
  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text = string_piece(text_copy.c_str(), text_copy.size());
  }
  this->text = text;
}

bool input_format_vertical::next_sentence(sentence& s, string& error) {
  error.clear();
  s.clear();

  // Skip spaces and newlines
  while (text.len && (*text.str == ' ' || *text.str == '\t' || *text.str == '\r' || *text.str == '\n'))
    text.str++, text.len--;

  // Read first word without spaces on every line
  while (text.len && *text.str != '\r' && *text.str != '\n') {
    string_piece word = text;

    // Slurp the word
    while (text.len && *text.str != ' ' && *text.str != '\t' && *text.str != '\r' && *text.str != '\n')
      text.str++, text.len--;
    word.len = text.str - word.str;
    s.add_word(word);

    // Skip spaces till end of line
    while (text.len && *text.str != '\r' && *text.str != '\n')
      text.str++, text.len--;

    // Skip one new line
    if (text.len >= 2 && text.str[0] == '\r' && text.str[1] == '\r')
      text.str += 2, text.len -= 2;
    else if (text.len && *text.str == '\n')
      text.str++, text.len--;

    // Skip spaces on the beginning of the line
    while (text.len && (*text.str == ' ' || *text.str == '\t'))
      text.str++, text.len--;
  }

  return !s.empty();
}

// Presegmented tokenizer
class input_format_presegmented_tokenizer : public input_format {
 public:
  input_format_presegmented_tokenizer(input_format* tokenizer) : tokenizer(tokenizer) {}

  virtual bool read_block(istream& is, string& block) const override;
  virtual void reset_document() override;
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(sentence& s, string& error) override;

 private:
  unique_ptr<input_format> tokenizer;
  string_piece text;
  string text_copy;
};

bool input_format_presegmented_tokenizer::read_block(istream& is, string& block) const {
  return bool(getline(is, block));
}

void input_format_presegmented_tokenizer::reset_document() {
  tokenizer->reset_document();
}

void input_format_presegmented_tokenizer::set_text(string_piece text, bool make_copy) {
  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text = string_piece(text_copy.c_str(), text_copy.size());
  }
  this->text = text;
}

bool input_format_presegmented_tokenizer::next_sentence(sentence& s, string& error) {
  error.clear();
  s.clear();

  sentence partial;
  while (text.len && s.empty()) {
    // Move next line from `text' to `line', including leading and following newlines
    string_piece line(text.str, 0);
    while (line.len < text.len && (line.str[line.len] == '\n' || line.str[line.len] == '\r'))
      line.len++;
    while (line.len < text.len && (line.str[line.len] != '\n' && line.str[line.len] != '\r'))
      line.len++;
    while (line.len < text.len && (line.str[line.len] == '\n' || line.str[line.len] == '\r'))
      line.len++;
    text.str += line.len, text.len -= line.len;

    // Add all tokens from the line to `s'
    tokenizer->set_text(line, false);
    while (tokenizer->next_sentence(partial, error)) {
      // Append words
      size_t words = s.words.size() - 1;
      for (size_t i = 1; i < partial.words.size(); i++) {
        s.words.push_back(move(partial.words[i]));
        s.words.back().id += words;
        if (s.words.back().head > 0) s.words.back().head += words;
      }

      // Append multiword_tokens
      for (auto&& multiword_token : partial.multiword_tokens) {
        s.multiword_tokens.push_back(move(multiword_token));
        s.multiword_tokens.back().id_first += words;
        s.multiword_tokens.back().id_last += words;
      }

      // Append empty nodes
      for (auto&& empty_node : partial.empty_nodes) {
        s.empty_nodes.push_back(move(empty_node));
        s.empty_nodes.back().id += words;
      }

      // Append comments
      for (auto&& comment : partial.comments) {
        s.comments.push_back(move(comment));
      }
    }
    if (!error.empty()) return false;
  }

  return !s.empty();
}

// Default implementation of virtual methods
void input_format::reset_document() {
  set_text("");
}

// Static factory methods
input_format* input_format::new_conllu_input_format() {
  return new input_format_conllu();
}

input_format* input_format::new_horizontal_input_format() {
  return new input_format_horizontal();
}

input_format* input_format::new_vertical_input_format() {
  return new input_format_vertical();
}

input_format* input_format::new_input_format(const string& name) {
  if (name == "conllu") return new_conllu_input_format();
  if (name == "horizontal") return new_horizontal_input_format();
  if (name == "vertical") return new_vertical_input_format();
  return nullptr;
}

input_format* input_format::new_presegmented_tokenizer(input_format* tokenizer) {
  return new input_format_presegmented_tokenizer(tokenizer);
}

} // namespace udpipe
} // namespace ufal
