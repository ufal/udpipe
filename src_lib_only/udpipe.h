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

  string_piece() : str(NULL), len(0) {}
  string_piece(const char* str) : str(str), len(strlen(str)) {}
  string_piece(const char* str, size_t len) : str(str), len(len) {}
  string_piece(const std::string& str) : str(str.c_str()), len(str.size()) {}
};
}
typedef utils::string_piece string_piece;

class token {
 public:
  std::string form;
  std::string misc;

  token(string_piece form = string_piece(), string_piece misc = string_piece());

  // CoNLL-U defined SpaceAfter=No feature
  bool get_space_after() const;
  void set_space_after(bool space_after);

  // UDPipe-specific all-spaces-preserving SpacesBefore and SpacesAfter features
  void get_spaces_before(std::string& spaces_before) const;
  void set_spaces_before(string_piece spaces_before);
  void get_spaces_after(std::string& spaces_after) const;
  void set_spaces_after(string_piece spaces_after);
  void get_spaces_in_token(std::string& spaces_in_token) const;
  void set_spaces_in_token(string_piece spaces_in_token);

  // UDPipe-specific TokenRange feature
  bool get_token_range(size_t& start, size_t& end) const;
  void set_token_range(size_t start, size_t end);
};

class word : public token {
 public:
  // form and misc are inherited from token
  int id;         // 0 is root, >0 is sentence word, <0 is undefined
  std::string lemma;   // lemma
  std::string upostag; // universal part-of-speech tag
  std::string xpostag; // language-specific part-of-speech tag
  std::string feats;   // list of morphological features
  int head;       // head, 0 is root, <0 is undefined
  std::string deprel;  // dependency relation to the head
  std::string deps;    // secondary dependencies

  std::vector<int> children;

  word(int id = -1, string_piece form = string_piece()) : token(form), id(id), head(-1) {}
};

class multiword_token : public token {
 public:
  // form and misc are inherited from token
  int id_first, id_last;

  multiword_token(int id_first = -1, int id_last = -1, string_piece form = string_piece(), string_piece misc = string_piece())
      : token(form, misc), id_first(id_first), id_last(id_last) {}
};

class empty_node {
 public:
  int id;              // 0 is root, >0 is sentence word, <0 is undefined
  int index;           // index for the current id, should be numbered from 1, 0=undefined
  std::string form;    // form
  std::string lemma;   // lemma
  std::string upostag; // universal part-of-speech tag
  std::string xpostag; // language-specific part-of-speech tag
  std::string feats;   // list of morphological features
  std::string deps;    // secondary dependencies
  std::string misc;    // miscellaneous information

  empty_node(int id = -1, int index = 0) : id(id), index(index) {}
};

class sentence {
 public:
  sentence();

  std::vector<word> words;
  std::vector<multiword_token> multiword_tokens;
  std::vector<empty_node> empty_nodes;
  std::vector<std::string> comments;
  static const std::string root_form;

  // Basic sentence modifications
  bool empty();
  void clear();
  word& add_word(string_piece form = string_piece());
  void set_head(int id, int head, const std::string& deprel);
  void unlink_all_words();

  // CoNLL-U defined comments
  bool get_new_doc(std::string* id = nullptr) const;
  void set_new_doc(bool new_doc, string_piece id = string_piece());
  bool get_new_par(std::string* id = nullptr) const;
  void set_new_par(bool new_par, string_piece id = string_piece());
  bool get_sent_id(std::string& id) const;
  void set_sent_id(string_piece id);
  bool get_text(std::string& text) const;
  void set_text(string_piece text);
};

class input_format {
 public:
  virtual ~input_format() {}

  virtual bool read_block(std::istream& is, std::string& block) const = 0;
  virtual void reset_document(string_piece id = string_piece()) = 0;
  virtual void set_text(string_piece text, bool make_copy = false) = 0;
  virtual bool next_sentence(sentence& s, std::string& error) = 0;

  // Static factory methods
  static input_format* new_input_format(const std::string& name);
  static input_format* new_conllu_input_format(const std::string& options = std::string());
  static input_format* new_generic_tokenizer_input_format(const std::string& options = std::string());
  static input_format* new_horizontal_input_format(const std::string& options = std::string());
  static input_format* new_vertical_input_format(const std::string& options = std::string());

  static input_format* new_presegmented_tokenizer(input_format* tokenizer);

  static const std::string CONLLU_V1;
  static const std::string CONLLU_V2;
  static const std::string GENERIC_TOKENIZER_NORMALIZED_SPACES;
  static const std::string GENERIC_TOKENIZER_PRESEGMENTED;
  static const std::string GENERIC_TOKENIZER_RANGES;
};

class output_format {
 public:
  virtual ~output_format() {}

  virtual void write_sentence(const sentence& s, std::ostream& os) = 0;
  virtual void finish_document(std::ostream& /*os*/) {};

  // Static factory methods
  static output_format* new_output_format(const std::string& name);
  static output_format* new_conllu_output_format(const std::string& options = std::string());
  static output_format* new_epe_output_format(const std::string& options = std::string());
  static output_format* new_matxin_output_format(const std::string& options = std::string());
  static output_format* new_horizontal_output_format(const std::string& options = std::string());
  static output_format* new_plaintext_output_format(const std::string& options = std::string());
  static output_format* new_vertical_output_format(const std::string& options = std::string());

  static const std::string CONLLU_V1;
  static const std::string CONLLU_V2;
  static const std::string HORIZONTAL_PARAGRAPHS;
  static const std::string PLAINTEXT_NORMALIZED_SPACES;
  static const std::string VERTICAL_PARAGRAPHS;
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
  static const std::string TOKENIZER_NORMALIZED_SPACES;
  static const std::string TOKENIZER_PRESEGMENTED;
  static const std::string TOKENIZER_RANGES;
};

class pipeline {
 public:
  pipeline(const model* m, const std::string& input, const std::string& tagger, const std::string& parser, const std::string& output);

  void set_model(const model* m);
  void set_input(const std::string& input);
  void set_tagger(const std::string& tagger);
  void set_parser(const std::string& parser);
  void set_output(const std::string& output);

  void set_immediate(bool immediate);
  void set_document_id(const std::string& document_id);

  bool process(std::istream& is, std::ostream& os, std::string& error) const;

  static const std::string DEFAULT;
  static const std::string NONE;

 private:
  const model* m;
  std::string input, tokenizer, tagger, parser, output;
  std::string document_id;
  bool immediate;
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
