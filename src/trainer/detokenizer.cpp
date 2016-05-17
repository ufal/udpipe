// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "detokenizer.h"
#include "unilib/utf8.h"
#include "unilib/unicode.h"

namespace ufal {
namespace udpipe {

const string detokenizer::space_after_no = "SpaceAfter=No";

detokenizer::detokenizer(const string& plain_text)
    : data_lowercased(perform_lowercase(plain_text)), data_categorized(perform_categorize(plain_text)),
    sa_lowercased(data_lowercased), sa_categorized(data_categorized) {}

void detokenizer::detokenize(sentence& s) const {
  string* previous_form = nullptr, *previous_misc = nullptr;
  for (size_t i = 1, j = 0; i < s.words.size(); i++) {
    string* form = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? &s.multiword_tokens[j].form : &s.words[i].form;
    string* misc = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? &s.multiword_tokens[j].misc : &s.words[i].misc;

    if (previous_form) {
      // Should we add SpaceAfter=No to the previous form?
      int score = difference(*previous_form, *form, true, LOWERCASE);
      if (!score) score = has_letters(*previous_form) && has_letters(*form) ? -1 : 0;
      if (!score) score = only_digits(*previous_form) && only_digits(*form) ? -1 : 0;
      if (!score) score = difference(*previous_form, *form, false, LOWERCASE);
      if (!score) score = difference(*previous_form, *form, false, CATEGORIZE);
      if (!score) score = difference(*previous_form, *form, true, CATEGORIZE);

      if (score > 0)
        previous_misc->append(!previous_misc->empty() ? "|" : "").append(space_after_no);
    }

    // Remove the SpaceAfter attribute if present
    auto nospace_index = misc->find(space_after_no);
    if (nospace_index != string::npos) {
      auto length = space_after_no.size();
      if (nospace_index > 0 && misc->at(nospace_index-1) == '|')
        nospace_index--, length++;
      else if (nospace_index + length < misc->size() && misc->at(nospace_index + length) == '|')
        length++;
      misc->replace(nospace_index, length, "");
    }

    previous_form = form;
    previous_misc = misc;
    if (j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i))
      i = s.multiword_tokens[j++].id_last;
  }
}

int detokenizer::difference(const string& left, const string& right, bool separate, int mode) const {
  auto& data = mode == LOWERCASE ? data_lowercased : data_categorized;
  auto& func = mode == LOWERCASE ? perform_lowercase : perform_categorize;
  auto& sa = mode == LOWERCASE ? sa_lowercased : sa_categorized;

  string left_mapped = func(left);
  string right_mapped = func(right);
  string pattern;

  pattern.assign(separate?" ":"").append(left_mapped).append(right_mapped).append(separate?" ":"");
  int together = sa.count(pattern);

  pattern.assign(separate?" ":"").append(left_mapped).append(" ").append(right_mapped).append(separate?" ":"");
  int apart = sa.count(pattern);

  return together - apart;
}

string detokenizer::perform_lowercase(const string& input) {
  using namespace unilib;

  string output;
  for (auto&& chr : utf8::decoder(input))
    utf8::append(output, unicode::lowercase(chr));
  return output;
}

string detokenizer::perform_categorize(const string& input) {
  using namespace unilib;

  string output;
  for (auto&& chr : utf8::decoder(input)) {
    auto category = unicode::category(chr);
    if (category & unicode::C) output.push_back('C');
    if (category & unicode::L) output.push_back('L');
    if (category & unicode::M) output.push_back('M');
    if (category & unicode::N) output.push_back('N');
    if (category & unicode::Pc) output.push_back('c');
    if (category & unicode::Pd) output.push_back('d');
    if (category & unicode::Pe) output.push_back('e');
    if (category & unicode::Pf) output.push_back('f');
    if (category & unicode::Pi) output.push_back('i');
    if (category & unicode::Po) output.push_back('o');
    if (category & unicode::Ps) output.push_back('s');
    if (category & unicode::S) output.push_back('S');
    if (category & unicode::Zl) output.push_back('Z');
    if (category & unicode::Zp) output.push_back('z');
    if (category & unicode::Zs) output.push_back(' ');
  }
  return output;
}

bool detokenizer::has_letters(const string& word) const {
  using namespace unilib;

  for (auto&& chr : utf8::decoder(word))
    if (unicode::category(chr) & unicode::L)
      return true;
  return false;
}

bool detokenizer::only_digits(const string& word) const {
  using namespace unilib;

  for (auto&& chr : utf8::decoder(word))
    if (unicode::category(chr) & ~unicode::N)
      return false;
  return true;
}

detokenizer::suffix_array::suffix_array(const string& str) : suffix_comparator(str), suffix_lower_finder(str), suffix_upper_finder(str) {
  sa.reserve(str.size());
  for (unsigned i = 0; i < str.size(); i++)
    sa.push_back(i);

  sort(sa.begin(), sa.end(), suffix_comparator);
}

unsigned detokenizer::suffix_array::count(const string& data) const {
  auto lower_it = lower_bound(sa.begin(), sa.end(), data, suffix_lower_finder);
  auto upper_it = upper_bound(sa.begin(), sa.end(), data, suffix_upper_finder);
  return upper_it - lower_it;
}

} // namespace udpipe
} // namespace ufal
