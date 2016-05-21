// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "sentence/sentence.h"

namespace ufal {
namespace udpipe {

class detokenizer {
 public:
  detokenizer(const string& plain_text);

  void detokenize(sentence& s) const;
 private:
  enum { LOWERCASE, CATEGORIZE, TOTAL };

  int difference(const string& left, const string& right, bool separate, int mode) const;

  static string perform_lowercase(const string& input);
  static string perform_categorize(const string& input);
  bool has_letters(const string& word) const;
  bool only_digits(const string& word) const;

  class suffix_array {
   public:
    suffix_array(const string& str);
    suffix_array(suffix_array&& other) = default;

    unsigned count(const string& data) const;

   private:
    vector<unsigned> sa;

    struct suffix_compare {
      suffix_compare(const string& str) : str(str) {}
      bool operator()(unsigned a, unsigned b) const { return str.compare(a, string::npos, str, b, string::npos) < 0; }
     private:
      const string& str;
    } suffix_comparator;

    struct suffix_lower_find {
      suffix_lower_find(const string& str) : str(str) {}
      bool operator()(unsigned a, const string& data) const { return str.compare(a, data.size(), data) < 0; }

     private:
      const string& str;
    } suffix_lower_finder;

    struct suffix_upper_find {
      suffix_upper_find(const string& str) : str(str) {}
      bool operator()(const string& data, unsigned a) const { return str.compare(a, data.size(), data) > 0; }

     private:
      const string& str;
    } suffix_upper_finder;
  };

  string data_lowercased, data_categorized;
  suffix_array sa_lowercased, sa_categorized;

  static const string space_after_no;
};

} // namespace udpipe
} // namespace ufal
