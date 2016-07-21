// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef UFAL_UDPIPE_H
#define UFAL_UDPIPE_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace ufal {
namespace udpipe {

namespace utils {
struct string_piece {
  const char* str;
  size_t len;

  string_piece() : str(nullptr), len(0) {}
  string_piece(const char* str) : str(str), len(strlen(str)) {}
  string_piece(const char* str, size_t len) : str(str), len(len) {}
  string_piece(const std::string& str) : str(str.c_str()), len(str.size()) {}
};
}
typedef utils::string_piece string_piece;

class word {
 public:
  int id;         // 0 is root, >0 is sentence word, <0 is undefined
  std::string form;    // form
  std::string lemma;   // lemma
  std::string upostag; // universal part-of-speech tag
  std::string xpostag; // language-specific part-of-speech tag
  std::string feats;   // list of morphological features
  int head;       // head, 0 is root, <0 is undefined
  std::string deprel;  // dependency relation to the head
  std::string deps;    // secondary dependencies
  std::string misc;    // miscellaneous information

  std::vector<int> children;

  word(int id = -1, string_piece form = string_piece()) : id(id), head(-1) {
    if (form.len) this->form.assign(form.str, form.len);
  }
};

class multiword_token {
 public:
  int id_first, id_last;
  std::string form;
  std::string misc;

  multiword_token(int id_first = -1, int id_last = -1, string_piece form = string_piece(), string_piece misc = string_piece()) : id_first(id_first), id_last(id_last) {
    if (form.len) this->form.assign(form.str, form.len);
    if (misc.len) this->misc.assign(misc.str, misc.len);
  }
};

class sentence {
 public:
  sentence();

  std::vector<word> words;
  std::vector<multiword_token> multiword_tokens;
  std::vector<std::string> comments;
  static const std::string root_form;

  bool empty();
  void clear();
  word& add_word(string_piece form = string_piece());
  void set_head(int id, int head, const std::string& deprel);
  void unlink_all_words();
};

class input_format {
 public:
  virtual ~input_format() {}

  virtual bool read_block(std::istream& is, std::string& block) const = 0;
  virtual void set_text(string_piece text, bool make_copy = false) = 0;
  virtual bool next_sentence(sentence& s, std::string& error) = 0;

  // Static factory methods
  static input_format* new_input_format(const std::string& name);
  static input_format* new_conllu_input_format();
  static input_format* new_horizontal_input_format();
  static input_format* new_vertical_input_format();

  static input_format* new_presegmented_tokenizer(input_format* tokenizer);
};

class output_format {
 public:
  virtual ~output_format() {}

  virtual void write_sentence(const sentence& s, std::ostream& os) = 0;
  virtual void finish_document(std::ostream& /*os*/) {};

  // Static factory methods
  static output_format* new_output_format(const std::string& name);
  static output_format* new_conllu_output_format();
  static output_format* new_matxin_output_format();
  static output_format* new_horizontal_output_format();
  static output_format* new_vertical_output_format();
};

class model {
 public:
  virtual ~model() {}

  static model* load(const char* fname);
  static model* load(std::istream& is);

  virtual input_format* new_tokenizer(const std::string& options) const = 0;
  virtual bool tag(sentence& s, const std::string& options, std::string& error) const = 0;
  virtual bool parse(sentence& s, const std::string& options, std::string& error) const = 0;

  static const std::string DEFAULT;
};

class pipeline {
 public:
  pipeline(const model* m, const std::string& input, const std::string& tagger, const std::string& parser, const std::string& output);

  void set_model(const model* m);
  void set_input(const std::string& input);
  void set_tagger(const std::string& tagger);
  void set_parser(const std::string& parser);
  void set_output(const std::string& output);

  bool process(std::istream& is, std::ostream& os, std::string& error) const;

  static const std::string DEFAULT;
  static const std::string NONE;

 private:
  const model* m;
  std::string input, tokenizer, tagger, parser, output;
};

class trainer {
 public:
  static bool train(const std::string& method, const std::vector<sentence>& train, const std::vector<sentence>& heldout,
                    const std::string& tokenizer, const std::string& tagger, const std::string& parser,
                    std::ostream& os, std::string& error);

  static const std::string DEFAULT;
  static const std::string NONE;
};

class evaluator {
 public:
  evaluator(const model* m, const std::string& tokenizer, const std::string& tagger, const std::string& parser);

  void set_model(const model* m);
  void set_tokenizer(const std::string& tokenizer);
  void set_tagger(const std::string& tagger);
  void set_parser(const std::string& parser);

  bool evaluate(std::istream& is, std::ostream& os, std::string& error) const;

  static const std::string DEFAULT;
  static const std::string NONE;

 private:
  const model* m;
  std::string tokenizer, tagger, parser;
};

class version {
 public:
  unsigned major;
  unsigned minor;
  unsigned patch;
  std::string prerelease;

  // Returns current version.
  static version current();
};

} // namespace udpipe
} // namespace ufal

#endif
