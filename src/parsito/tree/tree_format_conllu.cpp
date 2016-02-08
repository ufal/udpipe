// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "tree_format_conllu.h"
#include "utils/parse_int.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {
namespace parsito {

// Input CoNLL-U format

bool tree_input_format_conllu::read_block(istream& in, string& block) const {
  block.clear();

  string line;
  while (getline(in, line)) {
    block.append(line).push_back('\n');
    if (line.empty()) break;
  }

  return !block.empty();
}

void tree_input_format_conllu::set_text(string_piece text, bool make_copy) {
  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text = string_piece(text_copy.c_str(), text_copy.size());
  }
  this->text = text;
}

bool tree_input_format_conllu::next_tree(tree& t) {
  error.clear();
  t.clear();
  comments.clear();
  multiword_tokens.clear();
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
      if (t.empty()) continue;
      break;
    }

    if (*line.str == '#') {
      // Store comments at the beginning and ignore the rest
      if (t.empty()) comments.push_back(line);
      continue;
    }

    // Parse another tree node
    split(line, '\t', tokens);
    if (tokens.size() != 10)
      return error.assign("The CoNLL-U line '").append(line.str, line.len).append("' does not contain 10 columns!") , false;

    // Store and skip multiword tokens
    if (memchr(tokens[0].str, '-', tokens[0].len)) {
      split(tokens[0], '-', parts);
      if (parts.size() != 2)
        return error.assign("Cannot parse ID of multiword token '").append(line.str, line.len).append("'!") , false;
      int from, to;
      if (!parse_int(parts[0], "CoNLL-U id", from, error) || !parse_int(parts[1], "CoNLL-U id", to, error))
        return false;
      if (from != int(t.nodes.size()))
        return error.assign("Incorrect ID '").append(parts[0].str, parts[0].len).append("' of multiword token '").append(line.str, line.len).append("'!"), false;
      if (to < from)
        return error.assign("Incorrect range '").append(tokens[0].str, tokens[0].len).append("' of multiword token '").append(line.str, line.len).append("'!"), false;
      if (from <= last_multiword_token)
        return error.assign("Multiword token '").append(line.str, line.len).append("' overlaps with the previous one!"), false;
      last_multiword_token = to;
      multiword_tokens.emplace_back(from, line);
      continue;
    }

    // Parse node ID and head
    int id;
    if (!parse_int(tokens[0], "CoNLL-U id", id, error))
      return false;
    if (id != int(t.nodes.size()))
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


    // Add new node
    auto& node = t.add_node(string(tokens[1].str, tokens[1].len));
    if (!(tokens[2].len == 1 && tokens[2].str[0] == '_')) node.lemma.assign(tokens[2].str, tokens[2].len);
    if (!(tokens[3].len == 1 && tokens[3].str[0] == '_')) node.upostag.assign(tokens[3].str, tokens[3].len);
    if (!(tokens[4].len == 1 && tokens[4].str[0] == '_')) node.xpostag.assign(tokens[4].str, tokens[4].len);
    if (!(tokens[5].len == 1 && tokens[5].str[0] == '_')) node.feats.assign(tokens[5].str, tokens[5].len);
    node.head = head;
    if (!(tokens[7].len == 1 && tokens[7].str[0] == '_')) node.deprel.assign(tokens[7].str, tokens[7].len);
    if (!(tokens[8].len == 1 && tokens[8].str[0] == '_')) node.deps.assign(tokens[8].str, tokens[8].len);
    if (!(tokens[9].len == 1 && tokens[9].str[0] == '_')) node.misc.assign(tokens[9].str, tokens[9].len);
  }

  // Check that we got word for the last multiword token
  if (last_multiword_token >= int(t.nodes.size()))
    return error.assign("There are words missing for multiword token '").append(multiword_tokens.back().second.str, multiword_tokens.back().second.len).append("'!"), false;

  // Set heads correctly
  for (auto&& node : t.nodes)
    if (node.id && node.head >= 0) {
      if (node.head >= int(t.nodes.size()))
        return error.assign("Node ID '").append(to_string(node.id)).append("' form '").append(node.form).append("' has too large head: '").append(to_string(node.head)).append("'!"), false;
      t.set_head(node.id, node.head, node.deprel);
    }

  return !t.empty();
}

// Output CoNLL-U format

const string tree_output_format_conllu::underscore = "_";

void tree_output_format_conllu::write_tree(const tree& t, string& output, const tree_input_format* additional_info) const {
  output.clear();

  // Try casting input format to CoNLL-U
  auto input_conllu = dynamic_cast<const tree_input_format_conllu*>(additional_info);
  size_t input_conllu_multiword_tokens = 0;

  // Comments if present
  if (input_conllu)
    for (auto&& comment : input_conllu->comments)
      output.append(comment.str, comment.len).push_back('\n');

  // Print out the tokens
  for (int i = 1 /*skip the root node*/; i < int(t.nodes.size()); i++) {
    // Write multiword token if present
    if (input_conllu && input_conllu_multiword_tokens < input_conllu->multiword_tokens.size() &&
        i == input_conllu->multiword_tokens[input_conllu_multiword_tokens].first) {
      output.append(input_conllu->multiword_tokens[input_conllu_multiword_tokens].second.str,
                    input_conllu->multiword_tokens[input_conllu_multiword_tokens].second.len).push_back('\n');
      input_conllu_multiword_tokens++;
    }

    // Write the token
    output.append(to_string(i)).push_back('\t');
    output.append(t.nodes[i].form).push_back('\t');
    output.append(underscore_on_empty(t.nodes[i].lemma)).push_back('\t');
    output.append(underscore_on_empty(t.nodes[i].upostag)).push_back('\t');
    output.append(underscore_on_empty(t.nodes[i].xpostag)).push_back('\t');
    output.append(underscore_on_empty(t.nodes[i].feats)).push_back('\t');
    output.append(t.nodes[i].head < 0 ? "_" : to_string(t.nodes[i].head)).push_back('\t');
    output.append(underscore_on_empty(t.nodes[i].deprel)).push_back('\t');
    output.append(underscore_on_empty(t.nodes[i].deps)).push_back('\t');
    output.append(underscore_on_empty(t.nodes[i].misc)).push_back('\n');
  }
  output.push_back('\n');
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
