// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <utility>

#include "common.h"
#include "model.h"

namespace ufal {
namespace udpipe {

class evaluator {
 public:
  evaluator(const model* m, const string& tokenizer, const string& tagger, const string& parser);

  void set_model(const model* m);
  void set_tokenizer(const string& tokenizer);
  void set_tagger(const string& tagger);
  void set_parser(const string& parser);

  bool evaluate(istream& is, ostream& os, string& error) const;

  static const string DEFAULT;
  static const string NONE;

 private:
  const model* m;
  string tokenizer, tagger, parser;

  struct f1_info { size_t total_system, total_gold; double precision, recall, f1; };
  template <class T>
  static f1_info evaluate_f1(const vector<pair<size_t, T>>& system, const vector<pair<size_t, T>>& gold);

  class evaluation_data {
   public:
    struct word_data {
      size_t start, end;
      bool is_multiword;
      word w;

      word_data(size_t start, size_t end, bool is_multiword, const word& w)
          : start(start), end(end), is_multiword(is_multiword), w(w) {}
    };

    void add_sentence(const sentence& s);

    u32string chars;
    vector<pair<size_t, size_t>> sentences, tokens;
    vector<pair<size_t, string>> multiwords;
    vector<word_data> words;
  };

  class word_alignment {
   public:
    struct pair_system_gold {
      const word& system; const word& gold;
      pair_system_gold(const word& system, const word& gold) : system(system), gold(gold) {}
    };
    vector<pair_system_gold> matched;
    size_t total_system, total_gold;

    template <class Equals>
    f1_info evaluate_f1(Equals equals);

    static bool perfect_alignment(const evaluation_data& system, const evaluation_data& gold, word_alignment& alignment);
    static void best_alignment(const evaluation_data& system, const evaluation_data& gold, word_alignment& alignment);
  };
};

} // namespace udpipe
} // namespace ufal
