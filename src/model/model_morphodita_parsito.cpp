// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "model_morphodita_parsito.h"
#include "tokenizer/morphodita_tokenizer_wrapper.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "utils/getpara.h"
#include "utils/parse_double.h"
#include "utils/parse_int.h"

namespace ufal {
namespace udpipe {

// Versions:
// 1 - initial version
// 2 - add absolute lemmas (tagger_model::lemma == 2)
//   - use Arabic and space normalization

input_format* model_morphodita_parsito::new_tokenizer(const string& options) const {
  if (!tokenizer_factory)
    return nullptr;

  named_values::map parsed_options;
  string parse_error;
  if (!named_values::parse(options, parsed_options, parse_error))
    return nullptr;

  bool normalized_spaces = parsed_options.count("normalized_spaces");
  bool token_ranges = parsed_options.count("ranges");

  unique_ptr<input_format> result(new morphodita_tokenizer_wrapper(tokenizer_factory->new_tokenizer(), splitter.get(), normalized_spaces, token_ranges));

  // Presegmented
  if (parsed_options.count("presegmented") && result)
    result.reset(input_format::new_presegmented_tokenizer(result.release()));

  // Joint with parsing
  if (parsed_options.count("joint_with_parsing") && result) {
    int max_sentence_len = 20;
    if (parsed_options.count("joint_max_sentence_len") && !parse_int(parsed_options["joint_max_sentence_len"], "joint max sentence len", max_sentence_len, parse_error))
      return nullptr;

    double change_boundary_logprob = -0.5;
    if (parsed_options.count("joint_change_boundary_logprob") && !parse_double(parsed_options["joint_change_boundary_logprob"], "joint change boundary logprob", change_boundary_logprob, parse_error))
      return nullptr;

    double sentence_logprob = -0.5;
    if (parsed_options.count("joint_sentence_logprob") && !parse_double(parsed_options["joint_sentence_logprob"], "joint sentence logprob", sentence_logprob, parse_error))
      return nullptr;

    result.reset(new joint_with_parsing_tokenizer(result.release(), *this, max_sentence_len, change_boundary_logprob, sentence_logprob));
  }

  return result.release();
}

bool model_morphodita_parsito::tag(sentence& s, const string& /*options*/, string& error) const {
  error.clear();

  if (taggers.empty()) return error.assign("No tagger defined for the UDPipe model!"), false;
  if (s.empty()) return true;

  tagger_cache* c = tagger_caches.pop();
  if (!c) c = new tagger_cache();

  // Prepare input forms
  c->forms_normalized.resize(s.words.size() - 1);
  c->forms_string_pieces.resize(s.words.size() - 1);
  for (size_t i = 1; i < s.words.size(); i++)
    c->forms_string_pieces[i - 1] = normalize_form(s.words[i].form, c->forms_normalized[i - 1]);

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

    tagger.tagger->tag(c->forms_string_pieces, c->lemmas);

    for (size_t i = 0; i < c->lemmas.size(); i++)
      fill_word_analysis(c->lemmas[i], tagger.upostag, tagger.lemma, tagger.xpostag, tagger.feats, s.words[i+1]);
  }

  tagger_caches.push(c);
  return true;
}

bool model_morphodita_parsito::parse(sentence& s, const string& options, string& error) const {
  return parse(s, options, error, nullptr);
}

bool model_morphodita_parsito::parse(sentence& s, const string& options, string& error, double* cost) const {
  error.clear();

  if (!parser) return error.assign("No parser defined for the UDPipe model!"), false;
  if (s.empty()) return true;

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
    c->tree.add_node(string());
    normalize_form(s.words[i].form, c->tree.nodes.back().form);
    normalize_lemma(s.words[i].lemma, c->tree.nodes.back().lemma);
    c->tree.nodes.back().upostag.assign(s.words[i].upostag);
    c->tree.nodes.back().xpostag.assign(s.words[i].xpostag);
    c->tree.nodes.back().feats.assign(s.words[i].feats);
    c->tree.nodes.back().deps.assign(s.words[i].deps);
    c->tree.nodes.back().misc.assign(s.words[i].misc);
  }

  parser->parse(c->tree, beam_search, cost);
  for (size_t i = 1; i < s.words.size(); i++)
    s.set_head(i, c->tree.nodes[i].head, c->tree.nodes[i].deprel);

  parser_caches.push(c);
  return true;
}

model* model_morphodita_parsito::load(istream& is) {
  char version;
  if (!is.get(version)) return nullptr;
  if (!(version >= 1 && version <= VERSION_LATEST)) return nullptr;

  // Because UDPipe 1.0 does not check the model version,
  // a specific sentinel was added since version 2 so that
  // loading of such model fail on UDPipe 1.0
  if (version >= 2) {
    char sentinel;
    if (!is.get(sentinel) || sentinel != 0x7F) return nullptr;
    if (!is.get(sentinel) || sentinel != 0x7F) return nullptr;
  }

  unique_ptr<model_morphodita_parsito> m(new model_morphodita_parsito((unsigned char)version));
  if (!m) return nullptr;

  char tokenizer;
  if (!is.get(tokenizer)) return nullptr;
  m->tokenizer_factory.reset(tokenizer ? morphodita::tokenizer_factory::load(is) : nullptr);
  if (tokenizer && !m->tokenizer_factory) return nullptr;
  m->splitter.reset(tokenizer ? multiword_splitter::load(is) : nullptr);
  if (tokenizer && !m->splitter) return nullptr;

  m->taggers.clear();
  char taggers; if (!is.get(taggers)) return nullptr;
  for (char i = 0; i < taggers; i++) {
    char lemma; if (!is.get(lemma)) return nullptr;
    char xpostag; if (!is.get(xpostag)) return nullptr;
    char feats; if (!is.get(feats)) return nullptr;
    morphodita::tagger* tagger = morphodita::tagger::load(is);
    if (!tagger) return nullptr;
    m->taggers.emplace_back(i == 0, int(lemma), bool(xpostag), bool(feats), tagger);
  }

  char parser;
  if (!is.get(parser)) return nullptr;
  m->parser.reset(parser ? parsito::parser::load(is) : nullptr);
  if (parser && !m->parser) return nullptr;

  return m.release();
}

model_morphodita_parsito::model_morphodita_parsito(unsigned version) : version(version) {}

bool model_morphodita_parsito::joint_with_parsing_tokenizer::read_block(istream& is, string& block) const {
  block.clear();

  for (string line; getline(is, line); ) {
    block.append(line);
    block.push_back('\n');
  }

  if (is.eof() && !block.empty()) is.clear(istream::eofbit);
  return bool(is);
}

void model_morphodita_parsito::joint_with_parsing_tokenizer::reset_document(string_piece id) {
  new_document = true;
  document_id.assign(id.str, id.len);
  sentence_id = 1;
  set_text("");
  sentences.clear();
  sentences_index = 0;
}

void model_morphodita_parsito::joint_with_parsing_tokenizer::set_text(string_piece text, bool make_copy) {
  if (make_copy) {
    text_copy.assign(text.str, text.len);
    text.str = text_copy.c_str();
  }
  this->text = text;
}

bool model_morphodita_parsito::joint_with_parsing_tokenizer::next_sentence(sentence& s, string& error) {
  error.clear();

  if (text.len) {
    sentences.clear();
    sentences_index = 0;

    tokenizer->set_text(text, false);

    sentence input;
    vector<sentence> paragraph;
    while (tokenizer->next_sentence(input, error)) {
      if (input.get_new_par() && !paragraph.empty()) {
        if (!parse_paragraph(paragraph, error)) return false;
        for (auto&& sentence : paragraph)
          sentences.push_back(sentence);
        paragraph.clear();
      }
      paragraph.push_back(input);
    }
    if (!error.empty()) return false;

    if (!paragraph.empty()) {
      if (!parse_paragraph(paragraph, error)) return false;
      for (auto&& sentence : paragraph)
        sentences.push_back(sentence);
    }

    text.len = 0;
  }

  if (sentences_index < sentences.size()) {
    s = sentences[sentences_index++];
    return true;
  }

  return false;
}

bool model_morphodita_parsito::joint_with_parsing_tokenizer::parse_paragraph(vector<sentence>& paragraph, string& error) {
  sentence all_words;
  vector<bool> sentence_boundary(1, true);
  vector<bool> token_boundary(1, true);

  for (auto&& s : paragraph) {
    unsigned offset = all_words.words.size() - 1;
    for (unsigned i = 1; i < s.words.size(); i++) {
      all_words.words.push_back(s.words[i]);
      all_words.words.back().id += offset;
      sentence_boundary.push_back(i+1 == s.words.size());
      token_boundary.push_back(true);
    }

    for (auto&& mwt : s.multiword_tokens) {
      all_words.multiword_tokens.push_back(mwt);
      all_words.multiword_tokens.back().id_first += offset;
      all_words.multiword_tokens.back().id_last += offset;
      for (int i = all_words.multiword_tokens.back().id_first; i < all_words.multiword_tokens.back().id_last; i++)
        token_boundary[i] = false;
    }
  }

  vector<double> best_logprob(all_words.words.size(), -numeric_limits<double>::infinity()); best_logprob[0] = 0.;
  vector<unsigned> best_length(all_words.words.size(), 0);
  sentence s;

  for (unsigned start = 1; start < all_words.words.size(); start++) {
    if (!token_boundary[start - 1]) continue;
    s.clear();
    for (unsigned end = start + 1; end <= all_words.words.size() && (end - start) <= unsigned(max_sentence_len); end++) {
      s.words.push_back(all_words.words[end - 1]);
      s.words.back().id -= start - 1;
      if (!token_boundary[end - 1]) continue;

      for (unsigned i = 1; i < s.words.size(); i++) {
        s.words[i].head = -1;
        s.words[i].children.clear();
      }

      double cost;
      if (!model.tag(s, DEFAULT, error)) return false;
      if (!model.parse(s, DEFAULT, error, &cost)) return false;
      cost += sentence_logprob + change_boundary_logprob * (2 - int(sentence_boundary[start - 1]) - int(sentence_boundary[end - 1]));
      if (best_logprob[start - 1] + cost > best_logprob[end - 1]) {
        best_logprob[end - 1] = best_logprob[start - 1] + cost;
        best_length[end - 1] = end - start;
      }
    }
  }

  vector<unsigned> sentence_lengths;
  for (unsigned end = all_words.words.size(); end > 1; end -= best_length[end - 1])
    sentence_lengths.push_back(best_length[end - 1]);

  paragraph.clear();

  sentence_lengths.push_back(1);
  reverse(sentence_lengths.begin(), sentence_lengths.end());
  for (unsigned i = 1; i < sentence_lengths.size(); i++) {
    sentence_lengths[i] += sentence_lengths[i - 1];

    paragraph.emplace_back();
    while (!all_words.multiword_tokens.empty() && unsigned(all_words.multiword_tokens.front().id_first) < sentence_lengths[i]) {
      paragraph.back().multiword_tokens.push_back(all_words.multiword_tokens.front());
      paragraph.back().multiword_tokens.back().id_first -= sentence_lengths[i-1] - 1;
      paragraph.back().multiword_tokens.back().id_last -= sentence_lengths[i-1] - 1;
      all_words.multiword_tokens.erase(all_words.multiword_tokens.begin());
    }

    for (unsigned word = sentence_lengths[i - 1]; word < sentence_lengths[i]; word++) {
      paragraph.back().words.push_back(all_words.words[word]);
      paragraph.back().words.back().id -= sentence_lengths[i-1] - 1;
      paragraph.back().words.back().head = -1;
      paragraph.back().words.back().children.clear();
    }
  }

  if (!paragraph.empty()) {
    if (new_document) {
      paragraph.front().set_new_doc(true, document_id);
      new_document = false;
    }

    paragraph.front().set_new_par(true);
  }

  return true;
}


void model_morphodita_parsito::fill_word_analysis(const morphodita::tagged_lemma& analysis, bool upostag, int lemma, bool xpostag, bool feats, word& word) const {
  // Lemma
  if (lemma == 1) {
    word.lemma.assign(analysis.lemma);
  } else if (lemma == 2) {
    word.lemma.assign(analysis.lemma);

    // Lemma matching ~replacement~normalized_form is changed to replacement.
    if (analysis.lemma[0] == '~') {
      auto end = analysis.lemma.find('~', 1);
      if (end != string::npos) {
        normalize_form(word.form, word.lemma);
        if (analysis.lemma.compare(end + 1, string::npos, word.lemma) == 0)
          word.lemma.assign(analysis.lemma, 1, end - 1);
        else
          word.lemma.assign(analysis.lemma);
      }
    }
  }
  if (version == 2) {
    // Replace '\001' back to spaces
    for (auto && chr : word.lemma)
      if (chr == '\001')
        chr = ' ';
  } else if (version >= 3) {
    // Replace '0xC2 0xA0' back to spaces
    for (size_t i = 0; i + 1 < word.lemma.size(); i++)
      if (word.lemma[i] == char(0xC2) && word.lemma[i+1] == char(0xA0))
        word.lemma.replace(i, 2, 1, ' ');
  }

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

const string& model_morphodita_parsito::normalize_form(string_piece form, string& output) const {
  using unilib::utf8;

  // No normalization on version 1
  if (version <= 1) return output.assign(form.str, form.len);

  // If requested, replace space by \001 in version 2 and by &nbsp; (\u00a0) since version 3

  // Arabic normalization since version 2, implementation resulted from
  // discussion with Otakar Smrz and Nasrin Taghizadeh.
  // 1. Remove https://codepoints.net/U+0640 without any reasonable doubt :)
  // 2. Remove https://codepoints.net/U+0652
  // 3. Remove https://codepoints.net/U+0670
  // 4. Remove everything from https://codepoints.net/U+0653 to
  //    https://codepoints.net/U+0657 though they are probably very rare in date
  // 5. Remove everything from https://codepoints.net/U+064B to
  //    https://codepoints.net/U+0650
  // 6. Remove https://codepoints.net/U+0651
  // 7. Replace https://codepoints.net/U+0671 with https://codepoints.net/U+0627
  // 8. Replace https://codepoints.net/U+0622 with https://codepoints.net/U+0627
  // 9. Replace https://codepoints.net/U+0623 with https://codepoints.net/U+0627
  // 10. Replace https://codepoints.net/U+0625 with https://codepoints.net/U+0627
  // 11. Replace https://codepoints.net/U+0624 with https://codepoints.net/U+0648
  // 12. Replace https://codepoints.net/U+0626 with https://codepoints.net/U+064A
  // One might also consider replacing some Farsi characters that might be typed
  // unintentionally (by Iranians writing Arabic language texts):
  // 13. Replace https://codepoints.net/U+06CC with https://codepoints.net/U+064A
  // 14. Replace https://codepoints.net/U+06A9 with https://codepoints.net/U+0643
  // 15. Replace https://codepoints.net/U+06AA with https://codepoints.net/U+0643
  //
  // Not implemented:
  // There is additional challenge with data coming from Egypt (such as printed
  // or online newspapers), where the word-final https://codepoints.net/U+064A
  // may be switched for https://codepoints.net/U+0649 and visa versa. Also, the
  // word-final https://codepoints.net/U+0647 could actually represent https://
  // codepoints.net/U+0629. You can experiment with the following replacements,
  // but I would rather apply them only after classifying the whole document as
  // following such convention:
  // 1. Replace https://codepoints.net/U+0629 with https://codepoints.net/U+0647
  //    (frequent femine ending markers would appear like a third-person
  //    masculine pronoun clitic instead)
  // 2. Replace https://codepoints.net/U+0649 with https://codepoints.net/U+064A
  //    (some "weak" words would become even more ambiguous or appear as if
  //    with a first-person pronoun clitic)

  output.clear();
  for (auto&& chr : utf8::decoder(form.str, form.len)) {
    // Arabic normalization
    if (chr == 0x640 || (chr >= 0x64B && chr <= 0x657) || chr == 0x670) {}
    else if (chr == 0x622) utf8::append(output, 0x627);
    else if (chr == 0x623) utf8::append(output, 0x627);
    else if (chr == 0x624) utf8::append(output, 0x648);
    else if (chr == 0x625) utf8::append(output, 0x627);
    else if (chr == 0x626) utf8::append(output, 0x64A);
    else if (chr == 0x671) utf8::append(output, 0x627);
    else if (chr == 0x6A9) utf8::append(output, 0x643);
    else if (chr == 0x6AA) utf8::append(output, 0x643);
    else if (chr == 0x6CC) utf8::append(output, 0x64A);
    // Space normalization
    else if (chr == ' ' && version == 2) utf8::append(output, 0x01);
    else if (chr == ' ' && version >= 3) utf8::append(output, 0xA0);
    // Default
    else utf8::append(output, chr);
  }

  // Make sure we do not remove everything
  if (output.empty() && form.len)
    utf8::append(output, utf8::first(form.str, form.len));

  return output;
}

const string& model_morphodita_parsito::normalize_lemma(string_piece lemma, string& output) const {
  using unilib::utf8;

  // No normalization on version 1 and 2
  if (version <= 2) return output.assign(lemma.str, lemma.len);

  // Normalize spaces by &nbsp; since version 3
  output.clear();
  for (size_t i = 0; i < lemma.len; i++) {
    // Space normalization
    if (lemma.str[i] == ' ') utf8::append(output, 0xA0);
    // Default
    else output.push_back(lemma.str[i]);
  }

  return output;
}

} // namespace udpipe
} // namespace ufal
