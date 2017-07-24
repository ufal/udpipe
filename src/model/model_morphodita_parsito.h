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
#include "morphodita/tokenizer/tokenizer_factory.h"
#include "morphodita/tagger/tagger.h"
#include "parsito/parser/parser.h"
#include "tokenizer/multiword_splitter.h"
#include "utils/named_values.h"
#include "utils/threadsafe_stack.h"

namespace ufal {
namespace udpipe {

class model_morphodita_parsito : public model {
 public:
  virtual input_format* new_tokenizer(const string& options) const override;
  virtual bool tag(sentence& s, const string& options, string& error) const override;
  virtual bool parse(sentence& s, const string& options, string& error) const override;

  static model* load(istream& is);

 private:
  model_morphodita_parsito(unsigned version);
  unsigned version;
  enum { VERSION_LATEST = 3 };

  unique_ptr<morphodita::tokenizer_factory> tokenizer_factory;
  unique_ptr<multiword_splitter> splitter;
  struct tagger_model {
    bool upostag; int lemma; bool xpostag, feats;
    unique_ptr<morphodita::tagger> tagger;

    tagger_model(bool upostag, int lemma, bool xpostag, bool feats, morphodita::tagger* tagger)
        : upostag(upostag), lemma(lemma), xpostag(xpostag), feats(feats), tagger(tagger) {}
  };
  vector<tagger_model> taggers;
  unique_ptr<parsito::parser> parser;

  struct tagger_cache {
    vector<string> forms_normalized;
    vector<string_piece> forms_string_pieces;
    vector<morphodita::tagged_lemma> lemmas;
  };
  mutable threadsafe_stack<tagger_cache> tagger_caches;

  struct parser_cache {
    parsito::tree tree;
    named_values::map options;
  };
  mutable threadsafe_stack<parser_cache> parser_caches;

  bool parse(sentence& s, const string& options, string& error, double* cost) const;

  void fill_word_analysis(const morphodita::tagged_lemma& analysis, bool upostag, int lemma, bool xpostag, bool feats, word& word) const;
  const string& normalize_form(string_piece form, string& output) const;
  const string& normalize_lemma(string_piece lemma, string& output) const;
  friend class trainer_morphodita_parsito;
};

} // namespace udpipe
} // namespace ufal
