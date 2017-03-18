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
  virtual void write_sentence(const sentence& s, ostream& os) override;

 private:
  static const string underscore;
  const string& underscore_on_empty(const string& str) const { return str.empty() ? underscore : str; }
};

const string output_format_conllu::underscore = "_";

void output_format_conllu::write_sentence(const sentence& s, ostream& os) {
  // Comments
  for (auto&& comment : s.comments)
    os << comment << '\n';

  // Words and multiword tokens
  size_t multiword_token = 0, empty_node = 0;
  for (int i = 0; i < int(s.words.size()); i++) {
    // Write non-root nodes
    if (i > 0) {
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

    // Empty nodes
    for (; empty_node < s.empty_nodes.size() && i == s.empty_nodes[empty_node].id; empty_node++) {
      os << i << '.' << s.empty_nodes[empty_node].index << '\t'
         << s.empty_nodes[empty_node].form << '\t'
         << underscore_on_empty(s.empty_nodes[empty_node].lemma) << '\t'
         << underscore_on_empty(s.empty_nodes[empty_node].upostag) << '\t'
         << underscore_on_empty(s.empty_nodes[empty_node].xpostag) << '\t'
         << underscore_on_empty(s.empty_nodes[empty_node].feats) << '\t'
         << "_\t"
         << "_\t"
         << underscore_on_empty(s.empty_nodes[empty_node].deps) << '\t'
         << underscore_on_empty(s.empty_nodes[empty_node].misc) << '\n';
    }
  }
  os << endl;
}

// Matxin output format
class output_format_matxin : public output_format {
 public:
  virtual void write_sentence(const sentence& s, ostream& os) override;
  virtual void finish_document(ostream& os) override;

 private:
  void write_node(const sentence& s, int node, string& pad, ostream& os);

  int sentences = 0;
};

void output_format_matxin::write_sentence(const sentence& s, ostream& os) {
  if (!sentences) {
    os << "<corpus>";
  }
  os << "\n<SENTENCE ord=\"" << ++sentences << "\" alloc=\"" << 0 << "\">\n";

  string pad;
  for (auto&& node : s.words[0].children)
    write_node(s, node, pad, os);

  os << "</SENTENCE>" << endl;
}

void output_format_matxin::finish_document(ostream& os) {
  os << "</corpus>\n";

  sentences = 0;
}

void output_format_matxin::write_node(const sentence& s, int node, string& pad, ostream& os) {
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
  output_format_horizontal(bool paragraphs) : paragraphs(paragraphs), empty(true) {}

  virtual void write_sentence(const sentence& s, ostream& os) override;
  virtual void finish_document(ostream& /*os*/) override { empty = true; }

 private:
  bool paragraphs;
  bool empty;
};

void output_format_horizontal::write_sentence(const sentence& s, ostream& os) {
  if (paragraphs && !empty && (s.get_new_doc() || s.get_new_par()))
    os << '\n';
  empty = false;

  string line;
  for (size_t i = 1; i < s.words.size(); i++) {
    // Append word, but replace spaces by &nbsp;s
    for (auto&& chr : s.words[i].form)
      if (chr == ' ')
        line.append("\302\240");
      else
        line.push_back(chr);

    if (i+1 < s.words.size())
      line.push_back(' ');
  }
  os << line << endl;
}

// Plaintext output format
class output_format_plaintext : public output_format {
 public:
  output_format_plaintext(bool normalized): normalized(normalized), empty(true) {}

  virtual void write_sentence(const sentence& s, ostream& os) override;
  virtual void finish_document(ostream& /*os*/) override { empty = true; }
 private:
  bool normalized;
  bool empty;
};

void output_format_plaintext::write_sentence(const sentence& s, ostream& os) {
  if (normalized) {
    if (!empty && (s.get_new_doc() || s.get_new_par()))
      os << '\n';
    for (size_t i = 1, j = 0; i < s.words.size(); i++) {
      const token& tok = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? (const token&)s.multiword_tokens[j] : (const token&)s.words[i];
      if (i > 1 && tok.get_space_after()) os << ' ';
      os << tok.form;
      if (j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i))
        i = s.multiword_tokens[j++].id_last;
    }
    os << endl;
  } else {
    string spaces;
    for (size_t i = 1, j = 0; i < s.words.size(); i++) {
      const token& tok = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? (const token&)s.multiword_tokens[j] : (const token&)s.words[i];
      tok.get_spaces_before(spaces); os << spaces;
      tok.get_spaces_in_token(spaces); os << (!spaces.empty() ? spaces : tok.form);
      tok.get_spaces_after(spaces); os << spaces;
      if (j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i))
        i = s.multiword_tokens[j++].id_last;
    }
    os << flush;
  }
  empty = false;
}

// Vertical output format
class output_format_vertical : public output_format {
 public:
  output_format_vertical(bool paragraphs) : paragraphs(paragraphs), empty(true) {}

  virtual void write_sentence(const sentence& s, ostream& os) override;
  virtual void finish_document(ostream& /*os*/) override { empty = true; }

 private:
  bool paragraphs;
  bool empty;
};

void output_format_vertical::write_sentence(const sentence& s, ostream& os) {
  if (paragraphs && !empty && (s.get_new_doc() || s.get_new_par()))
    os << '\n';
  empty = false;

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
  return new output_format_horizontal(false);
}

output_format* output_format::new_horizontal_paragraphs_output_format() {
  return new output_format_horizontal(true);
}

output_format* output_format::new_plaintext_exact_output_format() {
  return new output_format_plaintext(false);
}

output_format* output_format::new_plaintext_normalized_output_format() {
  return new output_format_plaintext(true);
}

output_format* output_format::new_vertical_output_format() {
  return new output_format_vertical(false);
}

output_format* output_format::new_vertical_paragraphs_output_format() {
  return new output_format_vertical(true);
}

output_format* output_format::new_output_format(const string& name) {
  if (name == "conllu") return new_conllu_output_format();
  if (name == "matxin") return new_matxin_output_format();
  if (name == "horizontal") return new_horizontal_output_format();
  if (name == "horizontal_paragraphs") return new_horizontal_paragraphs_output_format();
  if (name == "plaintext_exact") return new_plaintext_exact_output_format();
  if (name == "plaintext_normalized") return new_plaintext_normalized_output_format();
  if (name == "vertical") return new_vertical_output_format();
  if (name == "vertical_paragraphs") return new_vertical_paragraphs_output_format();
  return nullptr;
}

} // namespace udpipe
} // namespace ufal
