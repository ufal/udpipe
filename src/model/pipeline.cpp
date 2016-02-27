// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "pipeline.h"
#include "sentence/input_format.h"

namespace ufal {
namespace udpipe {

pipeline::pipeline(const model* m, const string& tokenizer, const string& tagger, const string& parser)
    : m(m), tokenizer(tokenizer), tagger(tagger), parser(parser), conllu_output(output_format::new_conllu_output_format()) {}

void pipeline::set_model(const model* m) {
  this->m = m;
}

void pipeline::set_tokenizer(const string& tokenizer) {
  this->tokenizer = tokenizer;
}

void pipeline::set_tagger(const string& tagger) {
  this->tagger = tagger;
}

void pipeline::set_parser(const string& parser) {
  this->parser = parser;
}

bool pipeline::process(const string& input, ostream& os, string& error) const {
  error.clear();

  sentence s;
  if (tokenizer != "none") {
    // Tokenize the input
    unique_ptr<ufal::udpipe::tokenizer> t(m->new_tokenizer(tokenizer));
    if (!t) return error.assign("Cannot allocate new tokenizer!"), false;

    t->set_text(input);
    while (t->next_sentence(s, error))
      if (!process_tokenized(s, os, error))
        return false;
    if (!error.empty()) return false;
  } else {
    // The input is already in CoNLL-U format
    unique_ptr<input_format> conllu_input(input_format::new_conllu_input_format());
    if (!conllu_input || !conllu_output) return error.assign("Cannot allocate CoNLL-U format instance!"), false;

    conllu_input->set_text(input);
    while (conllu_input->next_sentence(s, error))
      if (!process_tokenized(s, os, error))
        return false;
    if (!error.empty()) return false;
  }

  return true;
}

bool pipeline::process_tokenized(sentence& s, ostream& os, string& error) const {
  // Tag
  if (tagger != "none")
    if (!m->tag(s, tagger, error))
      return false;

  // Parse
  if (parser != "none")
    if (!m->parse(s, parser, error))
      return false;

  // Write
  conllu_output->write_sentence(s, os);

  return true;
}

} // namespace udpipe
} // namespace ufal

