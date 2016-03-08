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
         << s.multiword_tokens[multiword_token].form << "\t_\t_\t_\t_\t_\t_\t_\t_\n";
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
