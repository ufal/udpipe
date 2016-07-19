// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "output_format.h"
#include "utils/parse_int.h"
#include "utils/split.h"
#include "utils/xml_encoded.h"

namespace ufal {
namespace udpipe {

// CoNLL-U output format
class output_format_conllu : public output_format {
 public:
  virtual void write_sentence(const sentence& s, ostream& os) const override;

 private:
  static const string underscore;
  const string& underscore_on_empty(const string& str) const { return str.empty() ? underscore : str; }
};

const string output_format_conllu::underscore = "_";

void output_format_conllu::write_sentence(const sentence& s, ostream& os) const {
  // Comments
  for (auto&& comment : s.comments)
    os << comment << '\n';

  // Words and multiword tokens
  size_t multiword_token = 0;
  for (int i = 1 /*skip the root node*/; i < int(s.words.size()); i++) {
    // Multiword token if present
    if (multiword_token < s.multiword_tokens.size() &&
        i == s.multiword_tokens[multiword_token].id_first) {
      os << s.multiword_tokens[multiword_token].id_first << '-'
         << s.multiword_tokens[multiword_token].id_last << '\t'
         << s.multiword_tokens[multiword_token].form << "\t_\t_\t_\t_\t_\t_\t_\t"
         << underscore_on_empty(s.multiword_tokens[multiword_token].misc) << '\n';
      multiword_token++;
    }

    // Write the word
    os << i << '\t'
       << s.words[i].form << '\t'
       << underscore_on_empty(s.words[i].lemma) << '\t'
       << underscore_on_empty(s.words[i].upostag) << '\t'
       << underscore_on_empty(s.words[i].xpostag) << '\t'
       << underscore_on_empty(s.words[i].feats) << '\t';
    if (s.words[i].head < 0) os << '_'; else os << s.words[i].head; os << '\t'
       << underscore_on_empty(s.words[i].deprel) << '\t'
       << underscore_on_empty(s.words[i].deps) << '\t'
       << underscore_on_empty(s.words[i].misc) << '\n';
  }
  os << endl;
}

// Matxin output format
class output_format_matxin : public output_format {
 public:
  virtual void write_sentence(const sentence& s, ostream& os) const override;

 private:
  void write_node(const sentence& s, int node, string& pad, ostream& os) const ;
};

void output_format_matxin::write_sentence(const sentence& s, ostream& os) const {
  os << "<SENTENCE ord=\"\" alloc=\"0\">\n";

  string pad;
  for (auto&& node : s.words[0].children)
    write_node(s, node, pad, os);

  os << "</SENTENCE>\n" << endl;
}

void output_format_matxin::write_node(const sentence& s, int node, string& pad, ostream& os) const {
  // <NODE ord="%d" alloc="%d" form="%s" lem="%s" mi="%s" si="%s">
  pad.push_back(' ');

  os << pad << "<NODE ord=\"" << node << "\" alloc=\"" << 0
     << "\" form=\"" << xml_encoded(s.words[node].form, true)
     << "\" lem=\"" << xml_encoded(s.words[node].lemma, true)
     << "\" mi=\"" << xml_encoded(s.words[node].feats, true)
     << "\" si=\"" << xml_encoded(s.words[node].deprel, true) << '"';

  if (s.words[node].children.empty()) {
    os << "/>\n";
  } else {
    os << ">\n";
    for (auto&& child : s.words[node].children)
      write_node(s, child, pad, os);
    os << pad << "</NODE>\n";
  }

  pad.pop_back();
}

// Horizontal output format
class output_format_horizontal : public output_format {
 public:
  virtual void write_sentence(const sentence& s, ostream& os) const override;
};

void output_format_horizontal::write_sentence(const sentence& s, ostream& os) const {
  for (size_t i = 1; i < s.words.size(); i++) {
    os << s.words[i].form;
    if (i+1 < s.words.size()) os << ' ';
  }
  os << endl;
}

// Vertical output format
class output_format_vertical : public output_format {
 public:
  virtual void write_sentence(const sentence& s, ostream& os) const override;
};

void output_format_vertical::write_sentence(const sentence& s, ostream& os) const {
  for (size_t i = 1; i < s.words.size(); i++)
    os << s.words[i].form << '\n';
  os << endl;
}

// Static factory methods
output_format* output_format::new_conllu_output_format() {
  return new output_format_conllu();
}

output_format* output_format::new_matxin_output_format() {
  return new output_format_matxin();
}

output_format* output_format::new_horizontal_output_format() {
  return new output_format_horizontal();
}

output_format* output_format::new_vertical_output_format() {
  return new output_format_vertical();
}

output_format* output_format::new_output_format(const string& name) {
  if (name == "conllu") return new_conllu_output_format();
  if (name == "matxin") return new_matxin_output_format();
  if (name == "horizontal") return new_horizontal_output_format();
  if (name == "vertical") return new_vertical_output_format();
  return nullptr;
}

} // namespace udpipe
} // namespace ufal
