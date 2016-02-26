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
#include "sentence.h"

namespace ufal {
namespace udpipe {

class input_format {
 public:
  virtual ~input_format() {}

  virtual bool read_block(istream& in, string& block) const = 0;
  virtual void set_text(string_piece text, bool make_copy = false) = 0;
  virtual bool next_sentence(sentence& s) = 0;
  const string& last_error() const;

  // Static factory methods
  static input_format* new_input_format(const string& name);
  static input_format* new_conllu_input_format();

 protected:
  string error;
};

} // namespace udpipe
} // namespace ufal
