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

bool pipeline::evaluate(const string& input, ostream& os, string& error) const {
  error.clear();

  sentence system, gold;

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

  int words = 0, upostag = 0, xpostag = 0, feats = 0, all_tags = 0, lemma = 0;
  int punct = 0, punct_uas = 0, punct_las = 0, nopunct = 0, nopunct_uas = 0, nopunct_las = 0;
  while (t ? t->next_sentence(gold, error) : conllu_input->next_sentence(gold, error)) {
    // Create empty copy
    system.clear();
    for (size_t i = 1; i < gold.words.size(); i++)
      system.add_word(gold.words[i].form);

    if (tagger != "none") {
      if (!m->tag(system, tagger, error))
        return false;
      for (size_t i = 1; i < gold.words.size(); i++) {
        words++;
        upostag += gold.words[i].upostag == system.words[i].upostag;
        xpostag += gold.words[i].xpostag == system.words[i].xpostag;
        feats += gold.words[i].feats == system.words[i].feats;
        all_tags += gold.words[i].upostag == system.words[i].upostag && gold.words[i].xpostag == system.words[i].xpostag && gold.words[i].feats == system.words[i].feats;
        lemma += gold.words[i].lemma == system.words[i].lemma;
      }
    } else {
      for (size_t i = 1; i < gold.words.size(); i++) {
        system.words[i].upostag = gold.words[i].upostag;
        system.words[i].xpostag = gold.words[i].xpostag;
        system.words[i].feats = gold.words[i].feats;
        system.words[i].lemma = gold.words[i].lemma;
      }
    }

    if (parser != "none") {
      if (!m->parse(system, parser, error))
        return false;
      for (size_t i = 1; i < gold.words.size(); i++) {
        punct++;
        punct_uas += gold.words[i].head == system.words[i].head;
        punct_las += gold.words[i].head == system.words[i].head && gold.words[i].deprel == system.words[i].deprel;
        if (gold.words[i].upostag != "PUNCT") {
          nopunct++;
          nopunct_uas += gold.words[i].head == system.words[i].head;
          nopunct_las += gold.words[i].head == system.words[i].head && gold.words[i].deprel == system.words[i].deprel;
        }
      }
    }
  }
  if (!error.empty()) return false;

  if (tagger != "none")
    os << "Tagging - forms: " << words << ", upostag: " << fixed << setprecision(2) << 100. * upostag / words << "%, xpostag: " << 100. * xpostag / words
       << "%, feats: " << 100. * feats / words << "%, all tags: " << 100. * all_tags / words << "%, lemma: " << 100. * lemma / words << '%' << endl;
  if (parser != "none")
    os << "Parsing - UAS: " << 100. * punct_uas / punct << "%, LAS: " << 100. * punct_las / punct << "%, "
       << "without punctuation - UAS: " << 100. * nopunct_uas / nopunct << "%, LAS: " << 100. * nopunct_las / nopunct << '%' << endl;

  return true;
}

} // namespace udpipe
} // namespace ufal
