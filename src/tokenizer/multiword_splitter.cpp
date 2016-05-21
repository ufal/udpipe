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

  // Try finding lowercased version in the suffixes
  utf8::map(unicode::lowercase, token.str, token.len, buffer);
  reverse(buffer.begin(), buffer.end());
  auto it = suffixes.find(buffer);
  if (it == suffixes.end() || it->second.words.empty()) {
    s.words.back().form.assign(token.str, token.len);
    if (misc.len) s.words.back().misc.assign(misc.str, misc.len);
    return;
  }

  // Determine casing
  enum { UC_FIRST, UC_ALL, OTHER } casing = OTHER;

  if (unicode::category(utf8::first(token.str, token.len)) & unicode::Lut) {
    casing = UC_ALL;
    for (auto&& chr : utf8::decoder(token.str, token.len))
      if (unicode::category(chr) & ~unicode::Lut) { casing = UC_FIRST; break; }
  }

  // Fill the multiword token
  s.multiword_tokens.emplace_back(s.words.back().id, s.words.back().id + it->second.words.size() - 1, token, misc);

  if (casing == OTHER) {
    s.words.back().form.assign(it->second.words[0]);
  } else if (casing == UC_ALL) {
    utf8::map(unicode::uppercase, it->second.words[0], s.words.back().form);
  } else /*if (casing == UC_FIRST)*/ {
    s.words.back().form.clear();
    for (auto&& chr : utf8::decoder(it->second.words[0]))
      utf8::append(s.words.back().form, s.words.back().form.empty() ? unicode::uppercase(chr) : chr);
  }
  for (size_t i = 1; i < it->second.words.size(); i++)
    if (casing != UC_ALL) {
      s.add_word(it->second.words[i]);
    } else {
      s.add_word();
      utf8::map(unicode::uppercase, it->second.words[i], s.words.back().form);
    }
}

multiword_splitter* multiword_splitter::load(istream& is) {
  char version;
  if (!is.get(version)) return nullptr;

  binary_decoder data;
  if (!compressor::load(is, data)) return nullptr;

  try {
    unique_ptr<multiword_splitter> splitter(new multiword_splitter());

    for (unsigned suffixes = data.next_4B(); suffixes; suffixes--) {
      string suffix;
      data.next_str(suffix);
      reverse(suffix.begin(), suffix.end());

      // Add the suffix and its words
      auto& info = splitter->suffixes[suffix];
      for (unsigned words = data.next_1B(); words; words--) {
        info.words.emplace_back();
        data.next_str(info.words.back());
      }
      if (info.words.empty()) return nullptr;

      // Add prefixes of the suffix with empty data
      if (!suffix.empty())
        for (suffix.pop_back(); !suffix.empty(); suffix.pop_back())
          splitter->suffixes[suffix];
    }

    return splitter.release();
  } catch (binary_decoder_error&) {
    return nullptr;
  }
}

} // namespace udpipe
} // namespace ufal
