// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "english_tokenizer.h"
#include "unilib/unicode.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// The list of lowercased words that when preceding eos do not end sentence.
const unordered_set<string> english_tokenizer::abbreviations = {
  // Titles
  "adj", "adm", "adv", "assoc", "asst", "bart", "bldg", "brig", "bros", "capt",
  "cmdr", "col", "comdr", "con", "corp", "cpl", "d", "dr", "dr", "drs", "ens",
  "gen", "gov", "hon", "hosp", "hr", "insp", "lt", "mm", "mr", "mrs", "ms",
  "maj", "messrs", "mlle", "mme", "mr", "mrs", "ms", "msgr", "op", "ord",
  "pfc", "ph", "phd", "prof", "pvt", "rep", "reps", "res", "rev", "rt", "sen",
  "sens", "sfc", "sgt", "sr", "st", "supt", "surg", "univ",
  // Common abbrevs
  "addr", "approx", "apr", "aug", "calif", "co", "corp", "dec", "def", "e",
  "e.g", "eg", "feb", "fla", "ft", "gen", "gov", "hrs", "i.", "i.e", "ie",
  "inc", "jan", "jr", "ltd", "mar", "max", "min", "mph", "mt", "n", "nov",
  "oct", "ont", "pa", "pres", "rep", "rev", "s", "sec", "sen", "sep", "sept",
  "sgt", "sr", "tel", "un", "univ", "v", "va", "vs", "w", "yrs",
};

%%{
  machine english_tokenizer_split_token;
  write data noerror nofinal;
}%%

void english_tokenizer::split_token(vector<token_range>& tokens) {
  if (tokens.empty() || chars[tokens.back().start].cat & ~unilib::unicode::L) return;

  size_t index = tokens.back().start, end = index + tokens.back().length;
  int cs;
  size_t split_mark = 0, split_len = 0;
  %%{
    include ragel_tokenizer "ragel_tokenizer.rl";
    variable p index;
    variable pe end;
    variable eof end;
    getkey ragel_char(chars[tokens.back().start + end - index - 1]);

    # For the split_mark to work, two marks must never appear in one token.
    action mark { split_mark = index - tokens.back().start + 1; }
    action split_mark { split_len = split_mark; fbreak; }

    apo = "'" | u_apo;
    main :=
      (('s'i | 'm'i | 'd'i) apo | ('ll'i | 'er'i | 'ev'i) apo | 't'i apo 'n'i) @mark @split_mark |
      ('ton'i @mark 'nac'i | 'ey'i @mark apo 'd'i | 'em'i @mark 'mig'i | 'an'i @mark 'nog'i |
       'at'i @mark 'tog'i | 'em'i @mark 'mel'i | 'n'i apo @mark 'erom'i | 'an'i @mark 'naw'i) %split_mark
    ;
    write init;
    write exec;
  }%%

  if (split_len && split_len < end) {
    tokens.back().length -= split_len;
    tokens.emplace_back(end - split_len, split_len);
  }
}

%%{
  machine english_tokenizer;
  write data noerror nofinal;
}%%

english_tokenizer::english_tokenizer(unsigned version) : ragel_tokenizer(version <= 1 ? 1 : 2) {}

bool english_tokenizer::next_sentence(vector<token_range>& tokens) {
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
    apo = "'" | u_apo;
    backapo = "`" | u_backapo;

    action unary_minus_allowed { !current || (chars[current-1].cat & ~(unicode::L | unicode::M | unicode::N | unicode::Pd)) }
    action unary_plus_allowed { !current || ((chars[current-1].cat & ~(unicode::L | unicode::M | unicode::N)) && chars[current-1].chr != '+') }
    whitespace = [\r\t\n] | u_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = u_L (u_L | u_M | '-' | apo)* -- ('--' | apo apo);
    number = ('-' when unary_minus_allowed | '+' when unary_plus_allowed)? u_Nd+ (',' (u_Nd{3}))* ([.] u_Nd+)? ([eE] [+\-]? u_Nd+)?;

    multiletter_punctuation = "--" | apo apo | backapo backapo | "...";

    # Segmentation
    action mark_whitespace { whitespace = current; }
    eos = [.!?] | u_3dot;
    closing = '"' | "'" | ';' | u_Pe | u_Pf;
    opening = '"' | '`' | u_Ps | u_Pi;

    main := |*
      word | number | (any - whitespace)
        { tokens.emplace_back(ts, te - ts);
          split_token(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) { fhold; fbreak; }
          while (tokenize_url_email(tokens));
          fhold;
        };

      eos closing* whitespace+ >mark_whitespace opening* (u_Lu | u_Lt)
        {
          bool eos = is_eos(tokens, chars[ts].chr, &abbreviations);
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
