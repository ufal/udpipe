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

  unique_ptr<ufal::udpipe::tokenizer> t;
  unique_ptr<input_format> conllu_input;
  if (tokenizer != "none") {
    t.reset(m->new_tokenizer(tokenizer));
    if (!t) return error.assign("Cannot allocate new tokenizer!"), false;
    t->set_text(input);
  } else {
    conllu_input.reset(input_format::new_conllu_input_format());
    if (!conllu_input) return error.assign("Cannot allocate CoNLL-U input format instance!"), false;
    conllu_input->set_text(input);
  }

  while (t ? t->next_sentence(s, error) : conllu_input->next_sentence(s, error)) {
    if (tagger != "none")
      if (!m->tag(s, tagger, error))
        return false;

    if (parser != "none")
      if (!m->parse(s, parser, error))
        return false;

    conllu_output->write_sentence(s, os);
  }
  if (!error.empty()) return false;

  return true;
}

} // namespace udpipe
} // namespace ufal
