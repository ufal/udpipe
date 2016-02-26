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

namespace ufal {
namespace udpipe {

// Output CoNLL-U format
class output_format_conllu : public output_format {
 public:
  virtual void write_sentence(const sentence& s, string& output) const override;

 private:
  static const string underscore;
  const string& underscore_on_empty(const string& str) const { return str.empty() ? underscore : str; }
};

const string output_format_conllu::underscore = "_";

void output_format_conllu::write_sentence(const sentence& s, string& output) const {
  output.clear();

  // Comments
  for (auto&& comment : s.comments)
    output.append(comment).push_back('\n');

  // Words and multiword tokens
  size_t multiword_token = 0;
  for (int i = 1 /*skip the root node*/; i < int(s.words.size()); i++) {
    // Multiword token if present
    if (multiword_token < s.multiword_tokens.size() &&
        i == s.multiword_tokens[multiword_token].id_first) {
      output.append(to_string(s.multiword_tokens[multiword_token].id_first)).push_back('-');
      output.append(to_string(s.multiword_tokens[multiword_token].id_last)).push_back('\t');
      output.append(s.multiword_tokens[multiword_token].form).append("\t_\t_\t_\t_\t_\t_\t_\t_\n");
      multiword_token++;
    }

    // Write the word
    output.append(to_string(i)).push_back('\t');
    output.append(s.words[i].form).push_back('\t');
    output.append(s.words[i].lemma).push_back('\t');
    output.append(underscore_on_empty(s.words[i].upostag)).push_back('\t');
    output.append(underscore_on_empty(s.words[i].xpostag)).push_back('\t');
    output.append(underscore_on_empty(s.words[i].feats)).push_back('\t');
    output.append(s.words[i].head < 0 ? underscore : to_string(s.words[i].head)).push_back('\t');
    output.append(underscore_on_empty(s.words[i].deprel)).push_back('\t');
    output.append(underscore_on_empty(s.words[i].deps)).push_back('\t');
    output.append(underscore_on_empty(s.words[i].misc)).push_back('\n');
  }
  output.push_back('\n');
}

// Static factory methods
output_format* output_format::new_conllu_output_format() {
  return new output_format_conllu();
}

output_format* output_format::new_output_format(const string& name) {
  if (name == "conllu") return new_conllu_output_format();
  return nullptr;
}

} // namespace udpipe
} // namespace ufal
