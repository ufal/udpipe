// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "multiword_splitter.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "utils/compressor.h"
#include "utils/binary_decoder.h"

namespace ufal {
namespace udpipe {

void multiword_splitter::append_token(string_piece token, string_piece misc, sentence& s) const {
  using namespace unilib;

  // Buffer
  s.add_word();
  string& buffer = s.words.back().form;

  // Lowercase the token
  utf8::map(unicode::lowercase, token.str, token.len, buffer);
  reverse(buffer.begin(), buffer.end());

  // Try finding lowercased version in the full_rules
  size_t prefix_len = 0;
  auto it = full_rules.find(buffer);

  if (it == full_rules.end()) {
    if (version >= 2) {
      string& suffix = s.words.back().misc;
      // Try searching suffix_rules if needed
      while (suffix.size() + 1 < buffer.size()) {
        suffix.push_back(buffer[suffix.size()]);

        auto suffix_it = suffix_rules.find(suffix);
        if (suffix_it == suffix_rules.end())
          break;

        if (!suffix_it->second.words.empty()) {
          it = suffix_it;
          prefix_len = buffer.size() - suffix.size();
        }
      }
      suffix.clear();
    }

    if (!prefix_len) {
      // No match
      s.words.back().form.assign(token.str, token.len);
      if (misc.len) s.words.back().misc.assign(misc.str, misc.len);
      return;
    }
  }

  // Determine casing
  enum { UPPER_FIRST, UPPER_ALL, UPPER_OTHER }; int casing = UPPER_OTHER;

  if (unicode::category(utf8::first(token.str, token.len)) & unicode::Lut) {
    casing = UPPER_ALL;
    for (auto&& chr : utf8::decoder(token.str, token.len))
      if (unicode::category(chr) & (unicode::L & ~unicode::Lut)) { casing = UPPER_FIRST; break; }
  }

  // Fill the multiword token
  s.multiword_tokens.emplace_back(s.words.back().id, s.words.back().id + (int)it->second.words.size() - 1, token, misc);

  s.words.back().form.clear();
  if (prefix_len) {
    // Note that prefix_len is measured in byte length of lowercased characters
    string_piece suffix(token);
    while (s.words.back().form.size() < prefix_len && suffix.len)
      utf8::append(s.words.back().form, unicode::lowercase(utf8::decode(suffix.str, suffix.len)));
    s.words.back().form.assign(token.str, token.len - suffix.len);
  }
  for (auto&& chr : utf8::decoder(it->second.words[0]))
    utf8::append(s.words.back().form, casing == UPPER_ALL || (casing == UPPER_FIRST && s.words.back().form.empty()) ? unicode::uppercase(chr) : chr);

  for (size_t i = 1; i < it->second.words.size(); i++)
    if (casing != UPPER_ALL) {
      s.add_word(it->second.words[i]);
    } else {
      s.add_word();
      utf8::map(unicode::uppercase, it->second.words[i], s.words.back().form);
    }
}

multiword_splitter* multiword_splitter::load(istream& is) {
  char version;
  if (!is.get(version)) return nullptr;
  if (!(version >= 1 && version <= VERSION_LATEST)) return nullptr;

  binary_decoder data;
  if (!compressor::load(is, data)) return nullptr;

  unique_ptr<multiword_splitter> splitter(new multiword_splitter(version));
  try {
    for (unsigned full_rules = data.next_4B(); full_rules; full_rules--) {
      string full_rule;
      data.next_str(full_rule);
      reverse(full_rule.begin(), full_rule.end());

      // Add the full_rule and its words
      auto& info = splitter->full_rules[full_rule];
      for (unsigned words = data.next_1B(); words; words--) {
        info.words.emplace_back();
        data.next_str(info.words.back());
      }
      if (info.words.empty()) return nullptr;
    }

    if (version >= 2)
      for (unsigned suffix_rules = data.next_4B(); suffix_rules; suffix_rules--) {
        string suffix_rule;
        data.next_str(suffix_rule);
        reverse(suffix_rule.begin(), suffix_rule.end());

        // Add the suffix_rule and its words
        auto& info = splitter->suffix_rules[suffix_rule];
        for (unsigned words = data.next_1B(); words; words--) {
          info.words.emplace_back();
          data.next_str(info.words.back());
        }
        if (info.words.empty()) return nullptr;

        // Add prefixes of the suffix with empty data
        if (!suffix_rule.empty())
          for (suffix_rule.pop_back(); !suffix_rule.empty(); suffix_rule.pop_back())
            splitter->suffix_rules[suffix_rule];
      }
  } catch (binary_decoder_error&) {
    return nullptr;
  }

  return data.is_end() ? splitter.release() : nullptr;
}

} // namespace udpipe
} // namespace ufal
