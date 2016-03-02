// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "common.h"
#include "model_morphodita_parsito.h"
#include "utils/parse_int.h"

namespace ufal {
namespace udpipe {

tokenizer* model_morphodita_parsito::new_tokenizer(const string& /*options*/) const {
  return new tokenizer_morphodita(this);
}

bool model_morphodita_parsito::tag(sentence& s, const string& /*options*/, string& error) const {
  error.clear();

  tagger_cache* c = tagger_caches.pop();
  if (!c) c = new tagger_cache();

  c->forms.clear();
  for (size_t i = 1; i < s.words.size(); i++)
    c->forms.emplace_back(s.words[i].form.c_str(), s.words[i].form.size());

  tagger->tag(c->forms, c->lemmas);

  for (size_t i = 0; i < c->lemmas.size(); i++) {
    // Lemma
    s.words[i + 1].lemma.assign(c->lemmas[i].lemma);

    // UPOSTag
    size_t start = 0, end = min(c->lemmas[i].tag.find('|'), c->lemmas[i].tag.size());
    s.words[i+1].upostag.assign(c->lemmas[i].tag, start, end - start);

    // XPOSTag
    start = min(end + 1, c->lemmas[i].tag.size());
    end = min(c->lemmas[i].tag.find('|', start), c->lemmas[i].tag.size());
    s.words[i+1].xpostag.assign(c->lemmas[i].tag, start, end - start);

    // Features
    start = min(end + 1, c->lemmas[i].tag.size());
    s.words[i+1].feats.assign(c->lemmas[i].tag, start, c->lemmas[i].tag.size() - start);
  }

  tagger_caches.push(c);
  return true;
}

bool model_morphodita_parsito::parse(sentence& s, const string& options, string& error) const {
  error.clear();

  parser_cache* c = parser_caches.pop();
  if (!c) c = new parser_cache();

  int beam_search = 1;
  if (!named_options::parse(options, c->options, error))
    return false;
  if (c->options.count("beam_search"))
    if (!parse_int(c->options["beam_search"], "beam_search", beam_search, error))
      return false;

  c->tree.clear();
  for (size_t i = 1; i < s.words.size(); i++) {
    c->tree.add_node(s.words[i].form);
    c->tree.nodes.back().lemma.assign(s.words[i].lemma);
    c->tree.nodes.back().upostag.assign(s.words[i].upostag);
    c->tree.nodes.back().xpostag.assign(s.words[i].xpostag);
    c->tree.nodes.back().feats.assign(s.words[i].feats);
    c->tree.nodes.back().deps.assign(s.words[i].deps);
    c->tree.nodes.back().misc.assign(s.words[i].misc);
  }

  parser->parse(c->tree, beam_search);
  for (size_t i = 1; i < s.words.size(); i++)
    s.set_head(i, c->tree.nodes[i].head, c->tree.nodes[i].deprel);

  parser_caches.push(c);
  return true;
}

model* model_morphodita_parsito::load(istream& is) {
  char version;
  if (!is.get(version)) return nullptr;

  unique_ptr<model_morphodita_parsito> m(new model_morphodita_parsito());
  if (!m) return nullptr;

  m->tagger.reset(morphodita::tagger::load(is));
  if (!m->tagger) return nullptr;

  m->parser.reset(parsito::parser::load(is));
  if (!m->parser) return nullptr;

  return m.release();
}

model_morphodita_parsito::tokenizer_morphodita::tokenizer_morphodita(const model_morphodita_parsito* m)
  : tokenizer(m->tagger->new_tokenizer()) {}

bool model_morphodita_parsito::tokenizer_morphodita::read_block(istream& is, string& block) const {
  block.clear();

  string line;
  while (getline(is, line)) {
    block.append(line).push_back('\n');
    if (line.empty()) break;
  }

  return !block.empty();
}

void model_morphodita_parsito::tokenizer_morphodita::set_text(string_piece text, bool make_copy) {
  tokenizer->set_text(text, make_copy);
}

bool model_morphodita_parsito::tokenizer_morphodita::next_sentence(sentence& s, string& error) {
  s.clear();
  error.clear();
  if (tokenizer->next_sentence(&forms, nullptr)) {
    for (size_t i = 0; i < forms.size(); i++) {
      s.add_word(string(forms[i].str, forms[i].len));
      if (i+1 < forms.size() && forms[i+1].str == forms[i].str + forms[i].len)
        s.words.back().misc.assign("SpaceAfter=No");
    }
    return true;
  }

  return false;
}

} // namespace udpipe
} // namespace ufal
