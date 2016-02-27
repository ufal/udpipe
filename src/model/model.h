// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "sentence/sentence.h"
#include "tokenizer.h"

namespace ufal {
namespace udpipe {

class model {
 public:
  virtual ~model() {}

  static model* load(const char* fname);
  static model* load(istream& is);

  virtual tokenizer* new_tokenizer(const string& options) const = 0;
  virtual bool tag(sentence& s, const string& options, string& error) const = 0;
  virtual bool parse(sentence& s, const string& options, string& error) const = 0;
};

} // namespace udpipe
} // namespace ufal
