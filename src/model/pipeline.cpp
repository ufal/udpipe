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
#include "sentence/output_format.h"
#include "unilib/utf8.h"
#include "utils/getwhole.h"

namespace ufal {
namespace udpipe {

const string pipeline::DEFAULT;
const string pipeline::NONE = "none";

pipeline::pipeline(const model* m, const string& input, const string& tagger, const string& parser, const string& output) : immediate(false) {
  set_model(m);
  set_input(input);
  set_tagger(tagger);
  set_parser(parser);
  set_output(output);
}

void pipeline::set_model(const model* m) {
  this->m = m;
}

void pipeline::set_input(const string& input) {
  tokenizer.clear();

  if (input.empty()) {
    this->input = "conllu";
  } else if (input == "tokenize" || input == "tokenizer") {
    this->input = "tokenizer";
  } else if (input.compare(0, 10, "tokenizer=") == 0) {
    this->input = "tokenizer";
    tokenizer.assign(input, 10, string::npos);
  } else {
    this->input = input;
  }
}

void pipeline::set_tagger(const string& tagger) {
  this->tagger = tagger;
}

void pipeline::set_parser(const string& parser) {
  this->parser = parser;
}

void pipeline::set_output(const string& output) {
  this->output = output.empty() ? "conllu" : output;
}

void pipeline::set_immediate(bool immediate) {
  this->immediate = immediate;
}

void pipeline::set_document_id(const string& document_id) {
  this->document_id = document_id;
}

bool pipeline::process(istream& is, ostream& os, string& error) const {
  error.clear();

  sentence s;

  unique_ptr<input_format> reader;
  if (input == "tokenizer") {
    reader.reset(m->new_tokenizer(tokenizer));
    if (!reader) return error.assign("The model does not have a tokenizer!"), false;
  } else {
    reader.reset(input_format::new_input_format(input));
    if (!reader) return error.assign("The requested input format '").append(input).append("' does not exist!"), false;
  }
  reader->reset_document(document_id);

  unique_ptr<output_format> writer(output_format::new_output_format(output));
  if (!writer) return error.assign("The requested output format '").append(output).append("' does not exist!"), false;

  string block;
  while (immediate ? reader->read_block(is, block) : bool(getwhole(is, block))) {
    reader->set_text(block);
    while (reader->next_sentence(s, error)) {
      if (tagger != NONE)
        if (!m->tag(s, tagger, error))
          return false;

      if (parser != NONE)
        if (!m->parse(s, parser, error))
          return false;

      writer->write_sentence(s, os);
    }
    if (!error.empty()) return false;
  }
  writer->finish_document(os);

  return true;
}

} // namespace udpipe
} // namespace ufal
