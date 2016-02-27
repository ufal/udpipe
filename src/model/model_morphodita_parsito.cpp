// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "model_morphodita_parsito.h"
#include "utils/iostreams.h"

namespace ufal {
namespace udpipe {

tokenizer* model_morphodita_parsito::new_tokenizer(const string& /*options*/) const {
  return new tokenizer_morphodita(this);
}

bool model_morphodita_parsito::tag(sentence& /*s*/, const string& /*options*/, string& error) const {
  error.clear();

  return false;
}

bool model_morphodita_parsito::parse(sentence& /*s*/, const string& /*options*/, string& error) const {
  error.clear();

  return false;
}

model* model_morphodita_parsito::load(istream& /*is*/) {
  return nullptr;
}

bool model_morphodita_parsito::tokenizer_morphodita::read_block(istream& is, string& block) const {
  return getpara(is, block);
}

void model_morphodita_parsito::tokenizer_morphodita::set_text(string_piece text, bool make_copy) {
  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text = string_piece(text_copy.c_str(), text_copy.size());
  }
  this->text = text;
}

bool model_morphodita_parsito::tokenizer_morphodita::next_sentence(sentence& /*s*/, string& error) {
  error.clear();

  return false;
}

} // namespace udpipe
} // namespace ufal

