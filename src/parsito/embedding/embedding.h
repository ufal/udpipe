// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "utils/binary_decoder.h"
#include "utils/binary_encoder.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class embedding {
 public:
  unsigned dimension;

  int lookup_word(const string& word, string& buffer) const;
  int unknown_word() const;
  float* weight(int id); // nullptr for wrong id
  const float* weight(int id) const; // nullpt for wrong id

  bool can_update_weights(int id) const;

  void load(binary_decoder& data);
  void save(binary_encoder& enc) const;
  void create(unsigned dimension, int updatable_index, const vector<pair<string, vector<float>>>& words, const vector<float>& unknown_weights);
 private:
  int updatable_index, unknown_index;

  unordered_map<string, int> dictionary;
  vector<float> weights;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
