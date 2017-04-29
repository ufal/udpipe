// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "morpho_statistical_guesser.h"
#include "small_stringops.h"
#include "utils/binary_decoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void morpho_statistical_guesser::load(binary_decoder& data) {
  // Load tags and default tag
  tags.resize(data.next_2B());
  for (auto&& tag : tags) {
    tag.resize(data.next_1B());
    for (unsigned i = 0; i < tag.size(); i++)
      tag[i] = data.next_1B();
  }
  default_tag = data.next_2B();

  // Load rules
  rules.load(data);
}

// Helper method for analyze.
static bool contains(morpho_statistical_guesser::used_rules* used, const string& rule) {
  if (!used) return false;

  for (auto&& used_rule : *used)
    if (used_rule == rule)
      return true;

  return false;
}

// Produces unique lemma-tag pairs.
void morpho_statistical_guesser::analyze(string_piece form, vector<tagged_lemma>& lemmas, morpho_statistical_guesser::used_rules* used) {
  unsigned lemmas_initial_size = lemmas.size();

  // We have rules in format "suffix prefix" in rules.
  // Find the matching rule with longest suffix and of those with longest prefix.
  string rule_label; rule_label.reserve(12);
  unsigned suffix_len = 0;
  for (; suffix_len < form.len; suffix_len++) {
    rule_label.push_back(form.str[form.len - (suffix_len + 1)]);
    if (!rules.at(rule_label.c_str(), rule_label.size(), [](pointer_decoder& data){ data.next<char>(data.next_2B()); }))
      break;
  }

  for (suffix_len++; suffix_len--; ) {
    rule_label.resize(suffix_len);
    rule_label.push_back(' ');

    const unsigned char* rule = nullptr;
    unsigned rule_prefix_len = 0;
    for (unsigned prefix_len = 0; prefix_len + suffix_len <= form.len; prefix_len++) {
      if (prefix_len) rule_label.push_back(form.str[prefix_len - 1]);
      const unsigned char* found = rules.at(rule_label.c_str(), rule_label.size(), [](pointer_decoder& data){ data.next<char>(data.next_2B()); });
      if (!found) break;
      if (*(found += sizeof(uint16_t))) {
        rule = found;
        rule_prefix_len = prefix_len;
      }
    }

    if (rule) {
      rule_label.resize(suffix_len + 1 + rule_prefix_len);
      if (rule_label.size() > 1 && !contains(used, rule_label)) { // ignore rule ' '
        if (used) used->push_back(rule_label);
        for (int rules_len = *rule++; rules_len; rules_len--) {
          unsigned pref_del_len = *rule++; const char* pref_del = (const char*)rule; rule += pref_del_len;
          unsigned pref_add_len = *rule++; const char* pref_add = (const char*)rule; rule += pref_add_len;
          unsigned suff_del_len = *rule++; const char* suff_del = (const char*)rule; rule += suff_del_len;
          unsigned suff_add_len = *rule++; const char* suff_add = (const char*)rule; rule += suff_add_len;
          unsigned tags_len = *rule++; const uint16_t* tags = (const uint16_t*)rule; rule += tags_len * sizeof(uint16_t);

          if (pref_del_len + suff_del_len > form.len ||
              (pref_del_len && !small_memeq(pref_del, form.str, pref_del_len)) ||
              (suff_del_len && !small_memeq(suff_del, form.str + form.len - suff_del_len, suff_del_len)) ||
              (form.len + pref_add_len - pref_del_len + suff_add_len - suff_del_len == 0))
            continue;

          string lemma;
          lemma.reserve(form.len + pref_add_len - pref_del_len + suff_add_len - suff_del_len);
          if (pref_add_len) lemma.append(pref_add, pref_add_len);
          if (pref_del_len + suff_del_len < form.len) lemma.append(form.str + pref_del_len, form.len - pref_del_len - suff_del_len);
          if (suff_add_len) lemma.append(suff_add, suff_add_len);
          while (tags_len--)
            lemmas.emplace_back(lemma, this->tags[*tags++]);
        }
      }
      break;
    }
  }

  // If nothing was found, use default tag.
  if (lemmas.size() == lemmas_initial_size)
    if (!contains(used, string())) {
      if (used) used->push_back(string());
      lemmas.emplace_back(string(form.str, form.len), tags[default_tag]);
    }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
