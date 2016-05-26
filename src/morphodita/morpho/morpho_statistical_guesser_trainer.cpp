// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "morpho_statistical_guesser_trainer.h"
#include "unilib/utf8.h"
#include "unilib/unicode.h"
#include "utils/split.h"
#include "trainer/training_failure.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void morpho_statistical_guesser_trainer::train(istream& is, unsigned suffix_len, unsigned rules_per_suffix, unsigned max_prefixes, unsigned min_prefix_count, ostream& os) {
  vector<instance> data;

  // Load training data
  string form;
  vector<string> tokens;
  for (string line; getline(is, line);) {
    if (line.empty()) continue;

    split(line, '\t', tokens);
    if (tokens.size() != 3) training_failure("The guesser training line '" << line << "' does not contain three columns!");
    if (tokens[0].empty() || tokens[1].empty() || tokens[2].empty()) training_failure("The guesser training line '" << line << "' contains an empty column!");

    // Normalize case
    casing form_case = get_casing(tokens[0], false);
    casing lemma_case = get_casing(tokens[1], true);
    if ((lemma_case == CASE_LC && (form_case == CASE_UCLC || form_case == CASE_UC)) ||
        (lemma_case == CASE_UCLC && form_case == CASE_UC)) {
      set_casing(tokens[0], lemma_case, form);
    } else {
      form.swap(tokens[0]);
    }

    data.emplace_back(form, tokens[1], tokens[2]);
  }

  // Generate at most max_prefixes prefixes with min_prefix_count
  unordered_map<string, unordered_set<string>> prefixes_with_forms;
  for (auto&& instance : data)
    if (!instance.form_prefix.empty())
      prefixes_with_forms[instance.form_prefix].insert(instance.form);

  vector<pair<unsigned, string>> prefixes_with_counts;
  for (auto&& prefix : prefixes_with_forms)
    if (prefix.second.size() >= min_prefix_count)
      prefixes_with_counts.emplace_back(prefix.second.size(), prefix.first);

  if (prefixes_with_counts.size() > max_prefixes) {
    sort(prefixes_with_counts.begin(), prefixes_with_counts.end(), greater<pair<unsigned, string>>());
    prefixes_with_counts.resize(max_prefixes);
  }

  unordered_set<string> prefixes;
  prefixes.emplace();
  for (auto&& prefix : prefixes_with_counts)
    prefixes.insert(prefix.second);

  // Generate the guesser rules
  unordered_map<string, unordered_set<string>> tags;
  unordered_map<string, unordered_map<string, unordered_set<string>>> rules;
  unordered_set<string> suffixes;
  string prefix_suffix, tag_lemma_rule;
  for (auto&& instance : data) {
    // Add tag
    tags[instance.tag].insert(instance.form);

    // Find longest matching prefix
    unsigned prefix_length = 0;
    for (auto&& prefix : prefixes)
      if (prefix.size() > prefix_length && instance.form.compare(0, prefix.size(), prefix) == 0)
        prefix_length = prefix.size();

    tag_lemma_rule.assign(instance.lemma_rule).append("\t").append(instance.tag);

    // Add prefix + all suffixes of length 1..suffix_len to rules
    for (unsigned length = 0, utf8_length = 0; length < suffix_len && suffix(instance.form, utf8_length); length++) {
      prefix_suffix.assign(instance.form, 0, prefix_length).append(" ").append(instance.form, instance.form.size() - utf8_length, utf8_length);
      rules[prefix_suffix][tag_lemma_rule].insert(instance.form);
      suffixes.emplace(instance.form, instance.form.size() - utf8_length, utf8_length);
    }
  }

  // Start generating the guesser description by writing the most "frequent" tag
  string most_frequent_tag; unsigned most_frequent_tag_count = 0;
  for (auto&& tag : tags)
    if (tag.second.size() > most_frequent_tag_count)
      most_frequent_tag.assign(tag.first), most_frequent_tag_count = tag.second.size();

  os << most_frequent_tag << endl;

  // For every prefix-suffix, write at most rules_per_suffix most "frequent" rules
  string rule_key, output;
  unordered_set<string> rules_set;
  vector<pair<unsigned, string>> rules_counts;
  for (auto&& suffix : suffixes) {
    for (auto&& prefix : prefixes) {
      rules_counts.clear();
      rules_set.clear();

      // Gather at most rules_per_suffix rules
      for (int prefix_len = int(prefix.size()); prefix_len >= 0; prefix_len -= prefix.empty() ? 1 : prefix.size()) {
        for (int suffix_len = int(suffix.size()); rules_counts.size() < rules_per_suffix && suffix_len > 0; suffix_len--) {
          rule_key.assign(prefix, 0, prefix_len).append(" ").append(suffix, suffix.size() - suffix_len, suffix_len);
          if (!rules.count(rule_key)) continue;

          unsigned rules_counts_original = rules_counts.size();
          for (auto&& entry : rules[rule_key])
            if (!rules_set.count(entry.first)) {
              rules_counts.emplace_back(entry.second.size(), entry.first);
              rules_set.insert(entry.first);
            }

          sort(rules_counts.begin() + rules_counts_original, rules_counts.end(), greater<pair<unsigned, string>>());

          if (rules_counts.size() >= rules_per_suffix) {
            rules_counts.resize(rules_per_suffix);
            break;
          }
        }
        // Stop if there are no rules for given prefix
        if (rules_set.empty()) break;
      }
      if (!rules_set.empty()) {
        // Write the chosen rules
        output.assign(prefix).append(" ").append(suffix);
        for (unsigned i = 0; i < rules_counts.size(); i++) {
          unsigned tab = rules_counts[i].second.find('\t');

          output.append("\t").append(rules_counts[i].second, 0, tab).append("\t").append(rules_counts[i].second, tab + 1, string::npos);

          // Join rules with same lemma_rule
          for (unsigned start = i; i+1 < rules_counts.size() && rules_counts[i+1].second.compare(0, tab + 1, rules_counts[start].second, 0, tab + 1) == 0; i++)
            output.append(" ").append(rules_counts[i+1].second, tab + 1, string::npos);
        }
        os << output << endl;
      }
    }
  }
}

morpho_statistical_guesser_trainer::instance::instance(const string& form, const string& lemma, const string& tag)
  : form(form), lemma(lemma), tag(tag)
{
  using namespace unilib;

  unsigned length_best = 0;
  int form_best = 0, lemma_best = 0;
  for (int offset = -int(lemma.size() - 1); offset < int(form.size()) - 1; offset++) {
    unsigned form_offset = max(0, offset);
    unsigned lemma_offset = max(0, -offset);
    for (unsigned length = 0; form_offset < form.size() && lemma_offset < lemma.size(); form_offset++, lemma_offset++)
      if (form[form_offset] == lemma[lemma_offset]) {
        if (++length > length_best && utf8::valid(form.c_str() + form_offset + 1 - length, length))
          length_best = length, form_best = form_offset + 1 - length, lemma_best = lemma_offset + 1 - length;
      } else {
        length = 0;
      }
  }

  form_prefix.assign(form, 0, lemma_best == 0 ? form_best : 0);
  lemma_rule.assign(form, 0, form_best).append(" ").append(lemma, 0, lemma_best).append(" ")
      .append(form, form_best + length_best, string::npos).append(" ").append(lemma, lemma_best + length_best, string::npos);
}

morpho_statistical_guesser_trainer::casing morpho_statistical_guesser_trainer::get_casing(const string& word, bool allow_nonletters) {
  using namespace unilib;

  casing c = CASE_OTHER;
  int index = 0;
  for (auto&& chr : utf8::decoder(word)) {
    auto cat = unicode::category(chr);

    // Return OTHER for non-letters
    if (allow_nonletters && index >= 2 && cat & ~unicode::L) continue;
    if (cat & ~unicode::L) return CASE_OTHER;

    if (index == 0) {
      c = cat & unicode::Ll ? CASE_LC : CASE_UC;
    } else if (c == CASE_UC && index == 1) {
      c = cat & unicode::Ll ? CASE_UCLC : CASE_UC;
    } else if (c == CASE_UC) {
      if (cat & ~unicode::Lut) return CASE_OTHER;
    } else /*CASE_LC or CASE_UCLC*/ {
      if (cat & ~unicode::Ll) return CASE_OTHER;
    }
    index++;
  }
  return c;
}

void morpho_statistical_guesser_trainer::set_casing(const string& original, casing c, string& word) {
  using namespace unilib;

  word.clear();
  bool first = true;
  for (auto&& chr : utf8::decoder(original)) {
    utf8::append(word, (c == CASE_UC || (c == CASE_UCLC && first)) ? unicode::uppercase(chr) : unicode::lowercase(chr));
    first = false;
  }
}

bool morpho_statistical_guesser_trainer::suffix(const string& word, unsigned& length) {
  using namespace unilib;

  unsigned additional = 1;
  while (additional + length <= word.size() && !utf8::valid(word.c_str() + word.size() - length - additional, additional))
    additional++;

  if (additional + length > word.size()) return false;

  length += additional;
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
