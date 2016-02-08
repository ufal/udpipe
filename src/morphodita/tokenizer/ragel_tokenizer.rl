// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ragel_tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

%%{
  machine ragel_tokenizer;

  variable p current;
  variable pe (chars.size() - 1);
  variable eof (chars.size() - 1);

  alphtype unsigned char;
  getkey ragel_char(chars[current]);

  u_3dot = 160;
  u_apo = 161;
  u_backapo = 162;
  u_hyphen = 163;

  u_Lu = [A-Z] | 129;
  u_Ll = [a-z] | 131;
  u_Lt = 135;
  u_Lm = 142;
  u_Lo = 157;
  u_L = u_Lu | u_Ll | u_Lt | u_Lm | u_Lo;

  u_Mn = 155;
  u_Mc = 151;
  u_Me = 143;
  u_M = u_Mn | u_Mc | u_Me;

  u_Nd = [0-9] | 159;
  u_Nl = 158;
  u_No = 156;
  u_N = u_Nd | u_Nl | u_No;

  u_Pc = '_' | 153;
  u_Pd = '-' | u_hyphen | 146;
  u_Ps = '(' | '[' | '{' | 133;
  u_Pe = ')' | ']' | '}' | 139;
  u_Pi = u_backapo | 150;
  u_Pf = u_apo | 141;
  u_Po = '!' | '"' | '#' | '%' | '&' | "'" | '*' | ',' | '.' | '/' | ':' | ';' | '?' | '@' | '\\' | u_3dot | 154;
  u_P = u_Pc | u_Pd | u_Ps | u_Pe | u_Pi | u_Pf | u_Po;

  u_Sm = '+' | '<' | '=' | '>' | '|' | '~' | 149;
  u_Sc = '$' | 138;
  u_Sk = '^' | '`' | 148;
  u_So = 137;
  u_S = u_Sm | u_Sc | u_Sk | u_So;

  u_Zs = ' ' | 147;
  u_Zl = 134;
  u_Zp = 140;
  u_Z = u_Zs | u_Zl | u_Zp;
}%%

%%{
  machine ragel_url_email;
  write data noerror nofinal;
}%%

vector<uint8_t> ragel_tokenizer::ragel_map;
atomic_flag ragel_tokenizer::ragel_map_flag = ATOMIC_FLAG_INIT;

ragel_tokenizer::ragel_tokenizer(unsigned url_email_tokenizer) : unicode_tokenizer(url_email_tokenizer) {
  while (ragel_map_flag.test_and_set()) {}
  if (ragel_map.empty()) {
    for (uint8_t ascii = 0; ascii < 128; ascii++)
      ragel_map.push_back(ascii);

    ragel_map_add(U'\u2026', 160); // horizontal ellipsis (TRIPLE DOT)
    ragel_map_add(U'\u2019', 161); // right single quotation mark
    ragel_map_add(U'\u2018', 162); // left single quotation mark
    ragel_map_add(U'\u2010', 163); // hyphen
  }
  ragel_map_flag.clear();
}

void ragel_tokenizer::ragel_map_add(char32_t chr, uint8_t mapping) {
  if (chr >= ragel_map.size())
    ragel_map.resize(chr + 1, 128);
  ragel_map[chr] = mapping;
}

bool ragel_tokenizer::ragel_url_email(unsigned version, const vector<char_info>& chars, size_t& current, vector<token_range>& tokens) {
  int cs;

  size_t start = current, end = current, parens = 0;
  %%{
    include ragel_tokenizer;

    action version_2 { version >= 2 }

    uchar = alnum | "$" | "-" | "_" | "." | "+" | "!" | "*" | "'" | "(" | ")" | "," | "%";
    xchar = uchar | ";" | "/" | "?" | ":" | "@" | "&" | "=";
    u_alnum = alnum | u_L | u_M | u_N;

    urlpath = '/' |
        ('/' xchar* (alnum | "'" | ')')) inwhen !version_2 |
        ('/' ('(' >{parens++;} | ')' >{parens-=!!parens;} | xchar | u_alnum)* (')' when {parens} | (xchar -- [.!',;?:)]) | u_alnum)) inwhen version_2;

    port = ':' digit*;

    ip = [0-9] | [1-9] [0-9] | '1' [0-9] [0-9] | '2' [0-4] [0-9] | '25' [0-5];
    hostnumber = ip '.' ip '.' ip '.' ip;
    topdomain = alpha{2,};
    subdomain = alnum | alnum (alnum | "-")* alnum;
    hostname = (subdomain '.')+ topdomain;
    host = hostname | hostnumber;

    username = uchar+;
    password = uchar+;
    user = username (':' password)? '@';

    protocol = alpha{3,} '://'; #('ftp'i|'http'i|'https'i|'gopher'i|'mailto'i|'news'i|'nntp'i|'telnet'i|'wais'i|'file'i|'prosper'i) '://';

    url = protocol? user? host port? urlpath?;
    email = username '@' hostname;

    main := (url | email) @ { end = current + 1; };

    write init;
    write exec;
  }%%

  if (end > start) {
    tokens.emplace_back(start, end - start);
    current = end;
    return true;
  } else {
    current = start;
    return false;
  }
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
