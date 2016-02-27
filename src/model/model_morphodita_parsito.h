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
#include "model.h"

namespace ufal {
namespace udpipe {

class model_morphodita_parsito : public model {
 public:
  virtual tokenizer* new_tokenizer(const string& options) const override;
  virtual bool tag(sentence& s, const string& options, string& error) const override;
  virtual bool parse(sentence& s, const string& options, string& error) const override;

 private:
  static model* load(istream& is);
  friend class model;

  class tokenizer_morphodita : public tokenizer {
   public:
    tokenizer_morphodita(const model_morphodita_parsito* m) : m(m) {}

    virtual bool read_block(istream& is, string& block) const override;
    virtual void set_text(string_piece text, bool make_copy = false) override;
    virtual bool next_sentence(sentence& s, string& error) override;

   private:
    const model_morphodita_parsito* m;
    string_piece text;
    string text_copy;
  };
};

} // namespace udpipe
} // namespace ufal
