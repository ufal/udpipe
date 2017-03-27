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

class output_format {
 public:
  virtual ~output_format() {}

  virtual void write_sentence(const sentence& s, ostream& os) = 0;
  virtual void finish_document(ostream& /*os*/) {}

  // Static factory methods
  static output_format* new_output_format(const string& name);
  static output_format* new_conllu_output_format(const string& options = string());
  static output_format* new_matxin_output_format(const string& options = string());
  static output_format* new_horizontal_output_format(const string& options = string());
  static output_format* new_plaintext_output_format(const string& options = string());
  static output_format* new_vertical_output_format(const string& options = string());

  static const string CONLLU_V1;
  static const string CONLLU_V2;
  static const string HORIZONTAL_PARAGRAPHS;
  static const string PLAINTEXT_NORMALIZED_SPACES;
  static const string VERTICAL_PARAGRAPHS;
};

} // namespace udpipe
} // namespace ufal
