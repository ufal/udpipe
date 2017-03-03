// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "czech_tokenizer.h"
#include "unilib/unicode.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

%%{
  machine czech_tokenizer;
  write data noerror nofinal;
}%%

// The list of lower cased words that when preceding eos do not end sentence.
// Note: because of VS, we cannot list the abbreviations directly in UTF-8,
// because the compilation of utf-8 encoded sources fail on some locales
// (e.g., Japanese).
// perl -CS -ple 'use Encode;s/([^[:ascii:]])/join("", map {sprintf "\\%o", ord($_)} split(m@@, encode("utf-8", $1)))/ge'
// perl -CS -ple 'use Encode;s/\\([0-7]{3})\\([0-7]{3})/decode("utf-8", chr(oct($1)).chr(oct($2)))/ge'
const unordered_set<string> czech_tokenizer::abbreviations_czech = {
  // Titles
  "prof", "csc", "drsc", "doc", "phd", "ph", "d",
  "judr", "mddr", "mudr", "mvdr", "paeddr", "paedr", "phdr", "rndr", "rsdr", "dr",
  "ing", "arch", "mgr", "bc", "mag", "mba", "bca", "mga",
  "gen", "plk", "pplk", "npor", "por", "ppor", "kpt", "mjr", "sgt", "pls", "p", "s",
  "p", "p\303\255", "fa", "fy", "mr", "mrs", "ms", "miss", "tr", "sv",
  // Geographic names
  "angl", "fr", "\304\215es", "ces", "\304\215s", "cs", "slov", "n\304\233m", "nem", "it", "pol", "ma\304\217", "mad", "rus",
  "sev", "v\303\275ch", "vych", "ji\305\276", "jiz", "z\303\241p", "zap",
  // Common abbrevs
  "adr", "\304\215", "c", "eg", "ev", "g", "hod", "j", "kr", "m", "max", "min", "mj", "nap\305\231", "napr",
  "okr", "pop\305\231", "popr", "pozn", "r", "\305\231", "red", "rep", "resp", "srov", "st", "st\305\231", "str",
  "sv", "tel", "tj", "tzv", "\303\272", "u", "uh", "ul", "um", "zl", "zn",
};

const unordered_set<string> czech_tokenizer::abbreviations_slovak = {
  // Titles
  "prof", "csc", "drsc", "doc", "phd", "ph", "d",
  "judr", "mddr", "mudr", "mvdr", "paeddr", "paedr", "phdr", "rndr", "rsdr", "dr",
  "ing", "arch", "mgr", "bc", "mag", "mba", "bca", "mga",
  "gen", "plk", "pplk", "npor", "por", "ppor", "kpt", "mjr", "sgt", "pls", "p", "s",
  "p", "p\303\255", "fa", "fy", "mr", "mrs", "ms", "miss", "tr", "sv",
  // Geographic names
  "angl", "fr", "\304\215es", "ces", "\304\215s", "cs", "slov", "nem", "it", "po\304\276", "pol", "ma\304\217", "mad",
  "rus", "sev", "v\303\275ch", "vych", "ju\305\276", "juz", "z\303\241p", "zap",
  // Common abbrevs
  "adr", "\304\215", "c", "eg", "ev", "g", "hod", "j", "kr", "m", "max", "min", "mj", "napr",
  "okr", "popr", "pozn", "r", "red", "rep", "resp", "srov", "st", "str",
  "sv", "tel", "tj", "tzv", "\303\272", "u", "uh", "ul", "um", "zl", "zn",
};

czech_tokenizer::czech_tokenizer(tokenizer_language language, unsigned version, const morpho* m)
  : ragel_tokenizer(version <= 1 ? 1 : 2), m(m) {
  switch (language) {
    case CZECH:
      abbreviations = &abbreviations_czech;
      break;
    case SLOVAK:
      abbreviations = &abbreviations_slovak;
      break;
  }
}

void czech_tokenizer::merge_hyphenated(vector<token_range>& tokens) {
  using namespace unilib;

  if (!m) return;
  if (tokens.empty() || chars[tokens.back().start].cat & ~unicode::L) return;

  unsigned matched_hyphens = 0;
  for (unsigned hyphens = 1; hyphens <= 2; hyphens++) {
    // Are the tokens a sequence of 'hyphens' hyphenated tokens?
    if (tokens.size() < 2*hyphens + 1) break;
    unsigned first_hyphen = tokens.size() - 2*hyphens;
    if (tokens[first_hyphen].length != 1 || chars[tokens[first_hyphen].start].cat & ~unicode::P ||
        tokens[first_hyphen].start + tokens[first_hyphen].length != tokens[first_hyphen + 1].start ||
        tokens[first_hyphen-1].start + tokens[first_hyphen-1].length != tokens[first_hyphen].start ||
        chars[tokens[first_hyphen-1].start].cat & ~unicode::L)
      break;

    if (m->analyze(string_piece(chars[tokens[first_hyphen-1].start].str, chars[tokens.back().start + tokens.back().length].str - chars[tokens[first_hyphen-1].start].str), morpho::NO_GUESSER, lemmas) >= 0)
      matched_hyphens = hyphens;
  }

  if (matched_hyphens) {
    unsigned first = tokens.size() - 2*matched_hyphens - 1;
    tokens[first].length = tokens.back().start + tokens.back().length - tokens[first].start;
    tokens.resize(first + 1);
  }
}

bool czech_tokenizer::next_sentence(vector<token_range>& tokens) {
  using namespace unilib;

  int cs, act;
  size_t ts, te;
  size_t whitespace = 0; // Suppress "may be uninitialized" warning

  while (tokenize_url_email(tokens))
    if (emergency_sentence_split(tokens))
      return true;
  %%{
    include ragel_tokenizer "ragel_tokenizer.rl";

    # Tokenization
    action unary_minus_allowed { !current || (chars[current-1].cat & ~(unicode::L | unicode::M | unicode::N | unicode::Pd)) }
    action unary_plus_allowed { !current || ((chars[current-1].cat & ~(unicode::L | unicode::M | unicode::N)) && chars[current-1].chr != '+') }
    whitespace = [\r\t\n] | u_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = u_L (u_L | u_M)*;
    number = ('-' when unary_minus_allowed | '+' when unary_plus_allowed)? u_Nd+ ([.,] u_Nd+)? ([eE] [+\-]? u_Nd+)?;

    # Segmentation
    action mark_whitespace { whitespace = current; }
    eos = [.!?] | u_3dot;
    closing = '"' | "'" | ';' | u_Pe | u_Pf;
    opening = '"' | '`' | u_Ps | u_Pi;

    main := |*
      word | number | (any - whitespace)
        { tokens.emplace_back(ts, te - ts);
          merge_hyphenated(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) { fhold; fbreak; }
          while (tokenize_url_email(tokens));
          fhold;
        };

      eos closing* whitespace+ >mark_whitespace opening* (u_Lu | u_Lt)
        {
          bool eos = is_eos(tokens, chars[ts].chr, abbreviations);
          for (current = ts; current < whitespace; current++)
            tokens.emplace_back(current, 1);
          fexec whitespace;
          if (eos) fbreak;
        };

      whitespace+ -- eol eol
        {
          current = te;
          do
            if (emergency_sentence_split(tokens)) { fhold; fbreak; }
          while (tokenize_url_email(tokens));
          fhold;
        };

      eol eol
        {
          if (!tokens.empty()) fbreak;
          current = te;
          do
            if (emergency_sentence_split(tokens)) { fhold; fbreak; }
          while (tokenize_url_email(tokens));
          fhold;
        };
    *|;

    write init;
    write exec;
  }%%
  (void)act; // Suppress unused variable warning

  return !tokens.empty();
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
