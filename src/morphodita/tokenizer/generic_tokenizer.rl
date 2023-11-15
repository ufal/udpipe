// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "generic_tokenizer.h"
#include "unilib/unicode.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

%%{
  machine generic_tokenizer;
  write data noerror nofinal;
}%%

generic_tokenizer::generic_tokenizer(unsigned version) : ragel_tokenizer(version <= 1 ? 1 : 2) {}

bool generic_tokenizer::next_sentence(vector<token_range>& tokens) {
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
    number = ('-' when unary_minus_allowed | '+' when unary_plus_allowed)? u_Nd+ ([.] u_Nd+)? ([eE] [+\-]? u_Nd+)?;

    # Segmentation
    action mark_whitespace { whitespace = current; }
    eos = [.!?] | u_3dot;
    closing = '"' | "'" | ';' | u_Pe | u_Pf;
    opening = '"' | '`' | u_Ps | u_Pi;

    main := |*
      word | number | (any - whitespace)
        { tokens.emplace_back(ts, te - ts);
          current = te;
          do
            if (emergency_sentence_split(tokens)) { fhold; fbreak; }
          while (tokenize_url_email(tokens));
          fhold;
        };

      eos closing* whitespace+ >mark_whitespace opening* (u_Lu | u_Lt)
        {
          bool eos = is_eos(tokens, chars[ts].chr, nullptr);
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

      (whitespace* eol -- eol eol) eol
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
