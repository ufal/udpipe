// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <map>

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
  struct token_info {
    vector<string> words;
    unsigned count = 0;
  };
  map<string, token_info> multiwords;

  string lc_form;
  for (auto&& sentence : data)
    for (auto&& multiword : sentence.multiword_tokens) {
      utf8::map(unicode::lowercase, multiword.form, lc_form);

      auto& info = multiwords[lc_form];
      info.count++;
      if (!info.words.empty()) continue;

      for (int i = multiword.id_first; i <= multiword.id_last; i++) {
        info.words.emplace_back();
        utf8::map(unicode::lowercase, sentence.words[i].form, info.words.back());
      }
    }

  // Remove the rules which triggers too negatively
  for (auto&& sentence : data)
    for (size_t i = 1, j = 0; i < sentence.words.size(); i++) {
      if (j < sentence.multiword_tokens.size() && sentence.multiword_tokens[j].id_first == int(i)) {
        i = sentence.multiword_tokens[j++].id_last;
        continue;
      }

      utf8::map(unicode::lowercase, sentence.words[i].form, lc_form);
      auto it = multiwords.find(lc_form);
      if (it != multiwords.end())
        if (!--it->second.count)
          multiwords.erase(it);
    }

  // Encode
  binary_encoder enc;
  enc.add_4B(multiwords.size());
  for (auto&& multiword : multiwords) {
    enc.add_str(multiword.first);
    enc.add_1B(multiword.second.words.size());
    for (auto& word : multiword.second.words)
      enc.add_str(word);
  }

  // Save
  os.put(1);
  if (!compressor::save(os, enc)) return error.assign("Cannot encode multiword_splitter!"), false;

  return true;
}

} // namespace udpipe
} // namespace ufal

