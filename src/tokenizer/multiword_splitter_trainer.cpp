// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <map>

#include "multiword_splitter.h"
#include "multiword_splitter_trainer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace udpipe {

bool multiword_splitter_trainer::train(const vector<sentence>& data, ostream& os, string& error) {
  using namespace unilib;
  error.clear();

  // Train
  struct rule_info {
    vector<string> words;
    unsigned count = 0;
  };
  map<string, rule_info> full_rules, suffix_rules;

  // Full rules
  string lc_form;
  vector<string> lc_words;
  for (auto&& sentence : data)
    for (auto&& multiword : sentence.multiword_tokens) {
      utf8::map(unicode::lowercase, multiword.form, lc_form);
      lc_words.clear();
      for (int i = multiword.id_first; i <= multiword.id_last; i++)
        utf8::map(unicode::lowercase, sentence.words[i].form, (lc_words.emplace_back(), lc_words.back()));

      auto& info = full_rules[lc_form];
      if (info.words.empty())
        info.words.assign(lc_words.begin(), lc_words.end());
      info.count += lc_words == info.words;
      if (!info.count) full_rules.erase(lc_form);
    }

  // Remove the full rules which trigger too negatively
  for (auto&& sentence : data)
    for (size_t i = 1, j = 0; i < sentence.words.size(); i++) {
      if (j < sentence.multiword_tokens.size() && sentence.multiword_tokens[j].id_first == int(i)) {
        i = sentence.multiword_tokens[j++].id_last;
        continue;
      }

      utf8::map(unicode::lowercase, sentence.words[i].form, lc_form);
      auto it = full_rules.find(lc_form);
      if (it != full_rules.end())
        if (!--it->second.count)
          full_rules.erase(it);
    }

  // Suffix rules
  for (auto&& full_rule : full_rules) {
    size_t prefix_match = 0;
    while (prefix_match < full_rule.first.size() && prefix_match < full_rule.second.words[0].size()) prefix_match++;
    for (; prefix_match; prefix_match--)
      if (((unsigned char)full_rule.first[prefix_match]) < 0x80 || ((unsigned char)full_rule.first[prefix_match]) >= 0xC0) {
        lc_form.assign(full_rule.first, prefix_match, string::npos);
        lc_words.assign(full_rule.second.words.begin(), full_rule.second.words.end());
        lc_words[0].erase(0, prefix_match);

        auto& info = suffix_rules[lc_form];
        if (info.words.empty())
          info.words.assign(lc_words.begin(), lc_words.end());
        info.count += lc_words == info.words;
        if (!info.count) suffix_rules.erase(lc_form);
      }
  }

  // Remove the suffix rules which trigger too negatively
  for (auto&& sentence : data)
    for (size_t i = 1, j = 0; i < sentence.words.size(); i++) {
      if (j < sentence.multiword_tokens.size() && sentence.multiword_tokens[j].id_first == int(i)) {
        i = sentence.multiword_tokens[j++].id_last;
        continue;
      }

      utf8::map(unicode::lowercase, sentence.words[i].form, lc_form);
      while (lc_form.size() > 1) {
        lc_form.erase(0, 1);
        auto it = suffix_rules.find(lc_form);
        if (it != suffix_rules.end()) {
          if (it->second.count <= 10)
            suffix_rules.erase(it);
          else
            it->second.count -= 10;
        }
      }
    }

  // Encode
  binary_encoder enc;
  enc.add_4B(full_rules.size());
  for (auto&& full_rule : full_rules) {
    enc.add_str(full_rule.first);
    enc.add_1B(full_rule.second.words.size());
    for (auto& word : full_rule.second.words)
      enc.add_str(word);
  }
  enc.add_4B(suffix_rules.size());
  for (auto&& suffix_rule : suffix_rules) {
    enc.add_str(suffix_rule.first);
    enc.add_1B(suffix_rule.second.words.size());
    for (auto& word : suffix_rule.second.words)
      enc.add_str(word);
  }

  // Save
  os.put(multiword_splitter::VERSION_LATEST);
  if (!compressor::save(os, enc)) return error.assign("Cannot encode multiword_splitter!"), false;

  return true;
}

} // namespace udpipe
} // namespace ufal
