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
#include "utils/getpara.h"
#include "utils/parse_int.h"

namespace ufal {
namespace udpipe {

tokenizer* model_morphodita_parsito::new_tokenizer(const string& /*options*/) const {
  return tokenizer_factory ? new tokenizer_morphodita(tokenizer_factory->new_tokenizer()) : nullptr;
}

bool model_morphodita_parsito::tag(sentence& s, const string& /*options*/, string& error) const {
  error.clear();

  if (!taggers.empty()) return error.assign("No tagger defined for the UDPipe model!"), false;

  tagger_cache* c = tagger_caches.pop();
  if (!c) c = new tagger_cache();

  // Prepare input forms
  c->forms.clear();
  for (size_t i = 1; i < s.words.size(); i++)
    c->forms.emplace_back(s.words[i].form);

  // Clear first
  for (size_t i = 1; i < s.words.size(); i++) {
    s.words[i].lemma.assign("_");
    s.words[i].upostag.clear();
    s.words[i].xpostag.clear();
    s.words[i].feats.clear();
  }

  // Fill information from the tagger models
  for (auto&& tagger : taggers) {
    if (!tagger.tagger) return error.assign("No tagger defined for the UDPipe model!"), false;

    tagger.tagger->tag(c->forms, c->lemmas);

    for (size_t i = 0; i < c->lemmas.size(); i++)
      fill_word_analysis(c->lemmas[i], tagger.upostag, tagger.lemma, tagger.xpostag, tagger.feats, s.words[i+1]);
  }

  tagger_caches.push(c);
  return true;
}

bool model_morphodita_parsito::parse(sentence& s, const string& options, string& error) const {
  error.clear();

  if (!parser) return error.assign("No parser defined for the UDPipe model!"), false;

  parser_cache* c = parser_caches.pop();
  if (!c) c = new parser_cache();

  int beam_search = 5;
  if (!named_values::parse(options, c->options, error))
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

  char tokenizer;
  if (!is.get(tokenizer)) return nullptr;
  m->tokenizer_factory.reset(tokenizer ? morphodita::tokenizer_factory::load(is) : nullptr);
  if (tokenizer && !m->tokenizer_factory) return nullptr;

  m->taggers.clear();
  char taggers; if (!is.get(taggers)) return nullptr;
  for (char i = 0; i < taggers; i++) {
    char lemma; if (!is.get(lemma)) return nullptr;
    char xpostag; if (!is.get(xpostag)) return nullptr;
    char feats; if (!is.get(feats)) return nullptr;
    morphodita::tagger* tagger = morphodita::tagger::load(is);
    if (!tagger) return nullptr;
    m->taggers.emplace_back(i == 0, bool(lemma), bool(xpostag), bool(feats), tagger);
  }

  char parser;
  if (!is.get(parser)) return nullptr;
  m->parser.reset(parser ? parsito::parser::load(is) : nullptr);
  if (parser && !m->parser) return nullptr;

  return m.release();
}

model_morphodita_parsito::tokenizer_morphodita::tokenizer_morphodita(morphodita::tokenizer* tokenizer)
  : tokenizer(tokenizer) {}

bool model_morphodita_parsito::tokenizer_morphodita::read_block(istream& is, string& block) const {
  return bool(getpara(is, block));
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

void model_morphodita_parsito::fill_word_analysis(const morphodita::tagged_lemma& analysis, bool upostag, bool lemma, bool xpostag, bool feats, word& word){
  // Lemma
  if (lemma) word.lemma.assign(analysis.lemma);

  if (!upostag && !xpostag && !feats) return;

  // UPOSTag
  char separator = analysis.tag[0];
  size_t start = min(size_t(1), analysis.tag.size()), end = min(analysis.tag.find(separator, 1), analysis.tag.size());
  if (upostag) word.upostag.assign(analysis.tag, start, end - start);

  if (!xpostag && !feats) return;

  // XPOSTag
  start = min(end + 1, analysis.tag.size());
  end = min(analysis.tag.find(separator, start), analysis.tag.size());
  if (xpostag) word.xpostag.assign(analysis.tag, start, end - start);

  if (!feats) return;

  // Features
  start = min(end + 1, analysis.tag.size());
  word.feats.assign(analysis.tag, start, analysis.tag.size() - start);
}

} // namespace udpipe
} // namespace ufal
