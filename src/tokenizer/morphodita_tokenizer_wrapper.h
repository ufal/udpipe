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
#include "morphodita/tokenizer/tokenizer.h"
#include "multiword_splitter.h"
#include "sentence/input_format.h"

namespace ufal {
namespace udpipe {

class morphodita_tokenizer_wrapper : public input_format {
 public:
  morphodita_tokenizer_wrapper(morphodita::tokenizer* tokenizer, const multiword_splitter* splitter, bool normalized_spaces);

  virtual bool read_block(istream& is, string& block) const override;
  virtual void reset_document(string_piece id) override;
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(sentence& s, string& error) override;

 private:
  unique_ptr<morphodita::tokenizer> tokenizer;
  const multiword_splitter* splitter;
  bool normalized_spaces;

  bool new_document = true;
  string document_id;
  unsigned preceeding_newlines = 2;
  unsigned sentence_id = 1;

  string_piece text;
  string text_copy;
  string saved_spaces;
  vector<string_piece> forms;
  token tok;
};

} // namespace udpipe
} // namespace ufal
