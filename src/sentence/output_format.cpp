// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "output_format.h"
#include "utils/named_values.h"
#include "utils/parse_int.h"
#include "utils/split.h"
#include "utils/xml_encoded.h"

namespace ufal {
namespace udpipe {

const string output_format::CONLLU_V1 = "v1";
const string output_format::CONLLU_V2 = "v2";
const string output_format::HORIZONTAL_PARAGRAPHS = "paragraphs";
const string output_format::PLAINTEXT_NORMALIZED_SPACES = "normalized_spaces";
const string output_format::VERTICAL_PARAGRAPHS = "paragraphs";

// CoNLL-U output format
class output_format_conllu : public output_format {
 public:
  output_format_conllu(unsigned version) : version(version) {}

  virtual void write_sentence(const sentence& s, ostream& os) override;

 private:
  unsigned version;
  static const string underscore;
  const string& underscore_on_empty(const string& str) const { return str.empty() ? underscore : str; }
  ostream& write_with_spaces(ostream& os, const string& str);
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
           << s.multiword_tokens[multiword_token].id_last << '\t';
        write_with_spaces(os, s.multiword_tokens[multiword_token].form) << "\t_\t_\t_\t_\t_\t_\t_\t"
           << underscore_on_empty(s.multiword_tokens[multiword_token].misc) << '\n';
        multiword_token++;
      }

      // Write the word
      os << i << '\t';
      write_with_spaces(os, s.words[i].form) << '\t';
      write_with_spaces(os, underscore_on_empty(s.words[i].lemma)) << '\t'
         << underscore_on_empty(s.words[i].upostag) << '\t'
         << underscore_on_empty(s.words[i].xpostag) << '\t'
         << underscore_on_empty(s.words[i].feats) << '\t';
      if (s.words[i].head < 0) os << '_'; else os << s.words[i].head; os << '\t'
         << underscore_on_empty(s.words[i].deprel) << '\t'
         << underscore_on_empty(s.words[i].deps) << '\t'
         << underscore_on_empty(s.words[i].misc) << '\n';
    }

    // Empty nodes
    if (version >= 2)
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

ostream& output_format_conllu::write_with_spaces(ostream& os, const string& str) {
  if (version >= 2 || str.find(' ') == string::npos)
    os << str;
  else
    for (auto&& chr : str)
      os << (chr == ' ' ? '_' : chr);

  return os;
}

// EPE output format
class output_format_epe : public output_format {
 public:
  virtual void write_sentence(const sentence& s, ostream& os) override;
  virtual void finish_document(ostream& os) override;

 private:
  class json_builder {
   public:
    json_builder& object() { comma(); json.push_back('{'); stack.push_back('}'); return *this; }
    json_builder& array() { comma(); json.push_back('['); stack.push_back(']'); return *this; }
    json_builder& close() { if (!stack.empty()) { json.push_back(stack.back()); stack.pop_back(); } comma_needed = true; return *this; }
    json_builder& key(string_piece name) { comma(); string(name); json.push_back(':'); return *this; }
    json_builder& value(string_piece value) { comma(); string(value); comma_needed=true; return *this; }
    json_builder& value(size_t value) { comma(); number(value); comma_needed=true; return *this; }
    json_builder& value_true() { comma(); json.push_back('t'); json.push_back('r'); json.push_back('u'); json.push_back('e'); comma_needed=true; return *this; }

    string_piece current() const { return string_piece(json.data(), json.size()); }
    void clear() { json.clear(); stack.clear(); comma_needed=false; }

   private:
    void comma() {
      if (comma_needed) {
        json.push_back(',');
        json.push_back(' ');
      }
      comma_needed = false;
    }
    void string(string_piece str) {
      json.push_back('"');
      for (; str.len; str.str++, str.len--)
        switch (*str.str) {
          case '"': json.push_back('\\'); json.push_back('\"'); break;
          case '\\': json.push_back('\\'); json.push_back('\\'); break;
          case '\b': json.push_back('\\'); json.push_back('b'); break;
          case '\f': json.push_back('\\'); json.push_back('f'); break;
          case '\n': json.push_back('\\'); json.push_back('n'); break;
          case '\r': json.push_back('\\'); json.push_back('r'); break;
          case '\t': json.push_back('\\'); json.push_back('t'); break;
          default:
            if (((unsigned char)*str.str) < 32) {
              json.push_back('u'); json.push_back('0'); json.push_back('0'); json.push_back('0' + (*str.str >> 4)); json.push_back("0123456789ABCDEF"[*str.str & 0xF]);
            } else {
              json.push_back(*str.str);
            }
        }
      json.push_back('"');
    }
    void number(size_t value) {
      size_t start_size = json.size();
      for (; value || start_size == json.size(); value /= 10)
        json.push_back('0' + (value % 10));
      reverse(json.begin() + start_size, json.end());
    }

    std::vector<char> json;
    std::vector<char> stack;
    bool comma_needed = false;
  } json;

  vector<string_piece> feats;
  size_t sentences = 0;
};

void output_format_epe::write_sentence(const sentence& s, ostream& os) {
  json.object().key("id").value(++sentences).key("nodes").array();

  for (size_t i = 1; i < s.words.size(); i++) {
    json.object().key("id").value(i).key("form").value(s.words[i].form);

    size_t start, end;
    if (s.words[i].get_token_range(start, end))
      json.key("start").value(start).key("end").value(end);
    if (s.words[i].head == 0)
      json.key("top").value_true();

    json.key("properties").object()
        .key("lemma").value(s.words[i].lemma)
        .key("upos").value(s.words[i].upostag)
        .key("xpos").value(s.words[i].xpostag);
    split(s.words[i].feats, '|', feats);
    for (auto&& feat : feats) {
      string_piece key(feat.str, 0);
      while (key.len < feat.len && key.str[key.len] != '=')
        key.len++;
      if (key.len + 1 < feat.len)
        json.key(key).value(string_piece(key.str + key.len + 1, feat.len - key.len - 1));
    }
    json.close();

    if (!s.words[i].children.empty()) {
      json.key("edges").array();
      for (auto&& child : s.words[i].children)
        json.object().key("label").value(s.words[child].deprel).key("target").value(child).close();
      json.close();
    }

    json.close();
  }
  json.close().close();

  string_piece current = json.current();
  os.write(current.str, current.len).put('\n');
  json.clear();
}

void output_format_epe::finish_document(ostream& /*os*/) {
  sentences = 0;
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
      os << tok.form;
      if (i+1 < s.words.size() && tok.get_space_after())
        os << ' ';
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
output_format* output_format::new_conllu_output_format(const string& options) {
  named_values::map parsed_options;
  string parse_error;
  if (!named_values::parse(options, parsed_options, parse_error))
    return nullptr;

  unsigned version = 2;
  if (parsed_options.count(CONLLU_V1))
    version = 1;
  if (parsed_options.count(CONLLU_V2))
    version = 2;

  return new output_format_conllu(version);
}

output_format* output_format::new_epe_output_format(const string& /*options*/) {
  return new output_format_epe();
}

output_format* output_format::new_matxin_output_format(const string& /*options*/) {
  return new output_format_matxin();
}

output_format* output_format::new_horizontal_output_format(const string& options) {
  named_values::map parsed_options;
  string parse_error;
  if (!named_values::parse(options, parsed_options, parse_error))
    return nullptr;

  return new output_format_horizontal(parsed_options.count(HORIZONTAL_PARAGRAPHS));
}

output_format* output_format::new_plaintext_output_format(const string& options) {
  named_values::map parsed_options;
  string parse_error;
  if (!named_values::parse(options, parsed_options, parse_error))
    return nullptr;

  return new output_format_plaintext(parsed_options.count(PLAINTEXT_NORMALIZED_SPACES));
}

output_format* output_format::new_vertical_output_format(const string& options) {
  named_values::map parsed_options;
  string parse_error;
  if (!named_values::parse(options, parsed_options, parse_error))
    return nullptr;

  return new output_format_vertical(parsed_options.count(VERTICAL_PARAGRAPHS));
}

output_format* output_format::new_output_format(const string& name) {
  size_t equal = name.find('=');
  size_t name_len = equal != string::npos ? equal : name.size();
  size_t option_offset = equal != string::npos ? equal + 1 : name.size();

  if (name.compare(0, name_len, "conllu") == 0) return new_conllu_output_format(name.substr(option_offset));
  if (name.compare(0, name_len, "epe") == 0) return new_epe_output_format(name.substr(option_offset));
  if (name.compare(0, name_len, "matxin") == 0) return new_matxin_output_format(name.substr(option_offset));
  if (name.compare(0, name_len, "horizontal") == 0) return new_horizontal_output_format(name.substr(option_offset));
  if (name.compare(0, name_len, "plaintext") == 0) return new_plaintext_output_format(name.substr(option_offset));
  if (name.compare(0, name_len, "vertical") == 0) return new_vertical_output_format(name.substr(option_offset));
  return nullptr;
}

} // namespace udpipe
} // namespace ufal
