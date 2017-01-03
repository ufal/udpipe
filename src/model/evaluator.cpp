// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "evaluator.h"
#include "sentence/input_format.h"
#include "unilib/utf8.h"

namespace ufal {
namespace udpipe {

const string evaluator::DEFAULT;
const string evaluator::NONE = "none";

evaluator::evaluator(const model* m, const string& tokenizer, const string& tagger, const string& parser) {
  set_model(m);
  set_tokenizer(tokenizer);
  set_tagger(tagger);
  set_parser(parser);
}

void evaluator::set_model(const model* m) {
  this->m = m;
}

void evaluator::set_tokenizer(const string& tokenizer) {
  this->tokenizer = tokenizer;
}

void evaluator::set_tagger(const string& tagger) {
  this->tagger = tagger;
}

void evaluator::set_parser(const string& parser) {
  this->parser = parser;
}

bool evaluator::evaluate(istream& is, ostream& os, string& error) const {
  error.clear();

  unique_ptr<input_format> conllu_input(input_format::new_conllu_input_format());
  if (!conllu_input) return error.assign("Cannot allocate CoNLL-U input format instance!"), false;

  string plain_text; unsigned space_after_nos = 0;
  sentence system, gold;
  evaluation_data gold_data, system_goldtok_data, system_goldtok_goldtags_data, system_plaintext_data;

  string block;
  while (conllu_input->read_block(is, block)) {
    conllu_input->set_text(block);
    while (conllu_input->next_sentence(gold, error)) {
      gold_data.add_sentence(gold);

      // Detokenize the input when tokenizing
      if (tokenizer != NONE) {
        bool previous_nospace = true;
        for (size_t i = 1, j = 0; i < gold.words.size(); i++) {
          if (!previous_nospace) plain_text.push_back(' ');
          plain_text.append(j < gold.multiword_tokens.size() && gold.multiword_tokens[j].id_first == int(i) ? gold.multiword_tokens[j].form : gold.words[i].form);
          const string& misc = j < gold.multiword_tokens.size() && gold.multiword_tokens[j].id_first == int(i) ? gold.multiword_tokens[j].misc : gold.words[i].misc;
          previous_nospace = misc.find(space_after_no) != string::npos;
          space_after_nos += previous_nospace ? 1 : 0;
          if (j < gold.multiword_tokens.size() && gold.multiword_tokens[j].id_first == int(i))
            i = gold.multiword_tokens[j++].id_last;
        }
        plain_text.push_back(' ');
      }

      // Goldtok data
      system.clear();
      for (size_t i = 1; i < gold.words.size(); i++)
        system.add_word(gold.words[i].form);

      if (tagger != NONE) {
        if (!m->tag(system, tagger, error))
          return false;
        if (parser != NONE)
          if (!m->parse(system, parser, error))
            return false;
      }
      system_goldtok_data.add_sentence(system);

      // Goldtok_goldtags data
      system.clear();
      for (size_t i = 1; i < gold.words.size(); i++) {
        system.add_word(gold.words[i].form);
        system.words[i].upostag = gold.words[i].upostag;
        system.words[i].xpostag = gold.words[i].xpostag;
        system.words[i].feats = gold.words[i].feats;
        system.words[i].lemma = gold.words[i].lemma;
      }
      if (parser != NONE)
        if (!m->parse(system, parser, error))
          return false;
      system_goldtok_goldtags_data.add_sentence(system);
    }
    if (!error.empty()) return false;
  }
  word_alignment goldtok_alignment, goldtok_goldtags_alignment;
  if (!word_alignment::perfect_alignment(system_goldtok_data, gold_data, goldtok_alignment) ||
      !word_alignment::perfect_alignment(system_goldtok_goldtags_data, gold_data, goldtok_goldtags_alignment))
    return error.assign("Internal UDPipe error (the words of the gold data and goldtok data do not match)!"), false;

  // Tokenize, tag and parse plaintext input
  if (tokenizer != NONE) {
    unique_ptr<input_format> t(m->new_tokenizer(tokenizer));
    if (!t) return error.assign("Cannot allocate new tokenizer!"), false;

    t->set_text(plain_text);
    while (t->next_sentence(system, error)) {
      if (tagger != NONE) {
        if (!m->tag(system, tagger, error))
          return false;

        if (parser != NONE)
          if (!m->parse(system, parser, error))
            return false;
      }
      system_plaintext_data.add_sentence(system);
    }
    if (!error.empty()) return false;
  }

  // Evaluate from plain text
  if (tokenizer != NONE) {
    if (system_plaintext_data.chars != gold_data.chars) {
      os << "Cannot evaluate tokenizer, it returned different sequence of token characters!" << endl;
    } else {
      word_alignment plaintext_alignment;
      if (space_after_nos)
        word_alignment::best_alignment(system_plaintext_data, gold_data, plaintext_alignment);

      if (!space_after_nos) {
        os << "No SpaceAfter=No features in gold data, not evaluating tokenizer performance." << endl;
      } else {
        auto words = plaintext_alignment.evaluate_f1([](const word&, const word&) {return true;});
        os << "Tokenizer words - system: " << words.total_system << ", gold: " << words.total_gold
           << ", precision: " << fixed << setprecision(2) << 100. * words.precision
           << "%, recall: " << 100. * words.recall << "%, f1: " << 100. * words.f1 << "%" << endl;
      }

      auto multiwords = evaluate_f1(system_plaintext_data.multiwords, gold_data.multiwords);
      if (multiwords.total_gold || multiwords.total_system)
        os << "Tokenizer multiwords - system: " << multiwords.total_system << ", gold: " << multiwords.total_gold
           << ", precision: " << fixed << setprecision(2) << 100. * multiwords.precision
           << "%, recall: " << 100. * multiwords.recall << "%, f1: " << 100. * multiwords.f1 << "%" << endl;

      if (space_after_nos) {
        auto sentences = evaluate_f1(system_plaintext_data.sentences, gold_data.sentences);
        os << "Tokenizer sentences - system: " << sentences.total_system << ", gold: " << sentences.total_gold
           << ", precision: " << fixed << setprecision(2) << 100. * sentences.precision
           << "%, recall: " << 100. * sentences.recall << "%, f1: " << 100. * sentences.f1 << "%" << endl;

        if (tagger != NONE) {
          auto upostags = plaintext_alignment.evaluate_f1([](const word& w, const word& u) { return w.upostag == u.upostag; });
          auto xpostags = plaintext_alignment.evaluate_f1([](const word& w, const word& u) { return w.xpostag == u.xpostag; });
          auto feats = plaintext_alignment.evaluate_f1([](const word& w, const word& u) { return w.feats == u.feats; });
          auto alltags = plaintext_alignment.evaluate_f1([](const word& w, const word& u) { return w.upostag == u.upostag && w.xpostag == u.xpostag && w.feats == u.feats; });
          auto lemmas = plaintext_alignment.evaluate_f1([](const word& w, const word& u) { return w.lemma == u.lemma; });
          os << "Tagging from plain text (F1 score) - gold forms: " << upostags.total_gold << ", upostag: "
             << fixed << setprecision(2) << 100. * upostags.f1 << "%, xpostag: "
             << 100. * xpostags.f1 << "%, feats: " << 100. * feats.f1 << "%, alltags: "
             << 100. * alltags.f1 << "%, lemmas: " << 100. * lemmas.f1 << '%' << endl;
        }

        if (tagger != NONE && parser != NONE) {
          auto uas = plaintext_alignment.evaluate_f1([](const word& w, const word& u) { return w.head == u.head; });
          auto las = plaintext_alignment.evaluate_f1([](const word& w, const word& u) { return w.head == u.head && w.deprel == u.deprel; });
          os << "Parsing from plain text with computed tags (F1 score) - gold forms: " << uas.total_gold
             << ", UAS: " << fixed << setprecision(2) << 100. * uas.f1 << "%, LAS: " << 100. * las.f1 << '%' << endl;
        }
      }
    }
  }

  // Evaluate tagger from gold tokenization
  if (tagger != NONE) {
    auto upostags = goldtok_alignment.evaluate_f1([](const word& w, const word& u) { return w.upostag == u.upostag; });
    auto xpostags = goldtok_alignment.evaluate_f1([](const word& w, const word& u) { return w.xpostag == u.xpostag; });
    auto feats = goldtok_alignment.evaluate_f1([](const word& w, const word& u) { return w.feats == u.feats; });
    auto alltags = goldtok_alignment.evaluate_f1([](const word& w, const word& u) { return w.upostag == u.upostag && w.xpostag == u.xpostag && w.feats == u.feats; });
    auto lemmas = goldtok_alignment.evaluate_f1([](const word& w, const word& u) { return w.lemma == u.lemma; });
    os << "Tagging from gold tokenization - forms: " << upostags.total_gold << ", upostag: "
       << fixed << setprecision(2) << 100. * upostags.f1 << "%, xpostag: "
       << 100. * xpostags.f1 << "%, feats: " << 100. * feats.f1 << "%, alltags: "
       << 100. * alltags.f1 << "%, lemmas: " << 100. * lemmas.f1 << '%' << endl;
  }

  // Evaluate parser from gold tokenization
  if (parser != NONE)
    for (int i = tagger==NONE; i < 2; i++) {
      auto& alignment = i==0 ?  goldtok_alignment : goldtok_goldtags_alignment;
      string description = i==0 ? "with computed tags" : "with gold tags";

      auto uas = alignment.evaluate_f1([](const word& w, const word& u) { return w.head == u.head; });
      auto las = alignment.evaluate_f1([](const word& w, const word& u) { return w.head == u.head && w.deprel == u.deprel; });
      os << "Parsing from gold tokenization " << description << " - forms: " << uas.total_gold
         << ", UAS: " << fixed << setprecision(2) << 100. * uas.f1 << "%, LAS: " << 100. * las.f1 << '%' << endl;
    }

  return true;
}

template <class T>
evaluator::f1_info evaluator::evaluate_f1(const vector<pair<size_t, T>>& system, const vector<pair<size_t, T>>& gold) {
  size_t both = 0;
  for (size_t si = 0, gi = 0; si < system.size() || gi < gold.size(); )
    if (si < system.size() && (gi == gold.size() || system[si].first < gold[gi].first))
      si++;
    else if (gi < gold.size() && (si == system.size() || gold[gi].first < system[si].first))
      gi++;
    else
      both += system[si++].second == gold[gi++].second;

  return {system.size(), gold.size(), system.size() ? both / double(system.size()) : 0.,
    gold.size() ? both / double(gold.size()) : 0., system.size()+gold.size() ? 2 * both / double(system.size() + gold.size()) : 0. };
}

void evaluator::evaluation_data::add_sentence(const sentence& s) {
  sentences.emplace_back(chars.size(), chars.size());
  for (size_t i = 1, j = 0; i < s.words.size(); i++) {
    tokens.emplace_back(chars.size(), chars.size());
    const string& form = j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i) ? s.multiword_tokens[j].form : s.words[i].form;
    for (auto&& chr : unilib::utf8::decoder(form))
      chars.push_back(chr);
    tokens.back().second = chars.size();

    if (j < s.multiword_tokens.size() && s.multiword_tokens[j].id_first == int(i)) {
      multiwords.emplace_back(tokens.back().first, form);
      for (size_t k = i; int(k) <= s.multiword_tokens[j].id_last; k++) {
        multiwords.back().second.append(" ").append(s.words[k].form);
        words.emplace_back(tokens.back().first, tokens.back().second, true, s.words[k]);
      }
      i = s.multiword_tokens[j++].id_last;
    } else {
      words.emplace_back(tokens.back().first, tokens.back().second, false, s.words[i]);
    }
  }
  sentences.back().second = chars.size();
}

template <class Equals>
evaluator::f1_info evaluator::word_alignment::evaluate_f1(Equals equals) {
  size_t both = 0;
  for (auto&& match : matched)
    if (equals(match.system, match.gold))
      both++;

  return {total_system, total_gold, total_system ? both / double(total_system) : 0.,
    total_gold ? both / double(total_gold) : 0., total_system+total_gold ? 2 * both / double(total_system + total_gold) : 0. };
}

bool evaluator::word_alignment::perfect_alignment(const evaluation_data& system, const evaluation_data& gold, word_alignment& alignment) {
  alignment.total_system = system.words.size();
  alignment.total_gold = gold.words.size();
  if (alignment.total_system != alignment.total_gold) return false;

  alignment.matched.clear();
  alignment.matched.reserve(alignment.total_system);
  for (size_t i = 0; i < system.words.size(); i++) {
    if (system.words[i].w.form != gold.words[i].w.form)
      return false;
    alignment.matched.emplace_back(system.words[i].w, gold.words[i].w);
  }

  return true;
}

void evaluator::word_alignment::best_alignment(const evaluation_data& system, const evaluation_data& gold, word_alignment& alignment) {
  alignment.total_system = system.words.size();
  alignment.total_gold = gold.words.size();
  alignment.matched.clear();

  for (size_t si = 0, gi = 0; si < system.words.size() && gi < gold.words.size(); )
    if ((system.words[si].start > gold.words[gi].start || !system.words[si].is_multiword) &&
        (gold.words[gi].start > system.words[si].start || !gold.words[gi].is_multiword)) {
      // No multiword, align using start+end indices
      if (system.words[si].start == gold.words[gi].start && system.words[si].end == gold.words[gi].end)
        alignment.matched.emplace_back(system.words[si++].w, gold.words[gi++].w);
      else if (system.words[si].start <= gold.words[gi].start)
        si++;
      else
        gi++;
    } else {
      // We have a multiword
      size_t ss = si, gs = gi, multiword_range_end = system.words[si].is_multiword ? system.words[si].end : gold.words[gi].end;

      // Find all words in the multiword range
      while ((si < system.words.size() && (system.words[si].is_multiword ? system.words[si].start < multiword_range_end :
                                           system.words[si].end <= multiword_range_end)) ||
             (gi < gold.words.size() && (gold.words[gi].is_multiword ? gold.words[gi].start < multiword_range_end :
                                         gold.words[gi].end <= multiword_range_end))) {
        // Extend the multiword range
        if (si < system.words.size() && (gi >= gold.words.size() || system.words[si].start <= gold.words[gi].start)) {
          if (system.words[si].is_multiword) multiword_range_end = max(multiword_range_end, system.words[si].end);
          si++;
        } else {
          if (gold.words[gi].is_multiword) multiword_range_end = max(multiword_range_end, gold.words[gi].end);
          gi++;
        }
      }

      // LCS on the chosen words
      vector<vector<unsigned>> lcs(si - ss);
      for (unsigned s = si - ss; s--; ) {
        lcs[s].resize(gi - gs);
        for (unsigned g = gi - gs; g--; ) {
          lcs[s][g] = max(lcs[s][g], s+1 < lcs.size() ? lcs[s+1][g] : 0);
          lcs[s][g] = max(lcs[s][g], g+1 < lcs[s].size() ? lcs[s][g+1] : 0);
          if (system.words[ss + s].w.form == gold.words[gs + g].w.form)
            lcs[s][g] = max(lcs[s][g], 1 + (s+1 < lcs.size() && g+1 < lcs[s].size() ? lcs[s+1][g+1] : 0));
        }
      }

      for (unsigned s = 0, g = 0; s < si - ss && g < gi - gs; ) {
        if (lcs[s][g] == 1 + (s+1 < lcs.size() && g+1 < lcs[s].size() ? lcs[s+1][g+1] : 0))
          alignment.matched.emplace_back(system.words[ss + s++].w, gold.words[gs + g++].w);
        else if (lcs[s][g] == (s+1 < lcs.size() ? lcs[s+1][g] : 0))
          s++;
        else /* if (lcs[s][g] == (g+1 < lcs[s].size() ? lcs[s][g+1] : 0)) */
          g++;
      }
    }
}

const string evaluator::space_after_no = "SpaceAfter=No";

} // namespace udpipe
} // namespace ufal
