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
#include "morphodita/tokenizer/tokenizer.h"
#include "morphodita/tagger/tagger.h"
#include "parsito/parser/parser.h"
#include "utils/named_values.h"
#include "utils/threadsafe_stack.h"

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

  bool have_lemmas;
  unique_ptr<morphodita::tagger> tagger;
  unique_ptr<parsito::parser> parser;

  class tokenizer_morphodita : public tokenizer {
   public:
    tokenizer_morphodita(const model_morphodita_parsito* m);

    virtual bool read_block(istream& is, string& block) const override;
    virtual void set_text(string_piece text, bool make_copy = false) override;
    virtual bool next_sentence(sentence& s, string& error) override;

   private:
    unique_ptr<morphodita::tokenizer> tokenizer;
    vector<string_piece> forms;
  };

  struct tagger_cache {
    vector<string_piece> forms;
    vector<morphodita::tagged_lemma> lemmas;
  };
  mutable threadsafe_stack<tagger_cache> tagger_caches;

  struct parser_cache {
    parsito::tree tree;
    named_values::map options;
  };
  mutable threadsafe_stack<parser_cache> parser_caches;

  static void fill_word_analysis(const morphodita::tagged_lemma& analysis, bool have_lemmas, word& word);
  friend class trainer_morphodita_parsito;
};

} // namespace udpipe
} // namespace ufal
