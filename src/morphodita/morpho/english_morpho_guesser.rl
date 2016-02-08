// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// This code is a reimplementation of morphologic analyzer Morphium
// by Johanka Spoustová (Treex::Tool::EnglishMorpho::Analysis Perl module)
// and reimplementation of morphologic lemmatizer by Martin Popel
// (Treex::Tool::EnglishMorpho::Lemmatizer Perl module). The latter is based
// on morpha:
//   Minnen, G., J. Carroll and D. Pearce (2001). Applied morphological
//   processing of English, Natural Language Engineering, 7(3). 207-223.
// Morpha has been released under LGPL as a part of RASP system
//   http://ilexir.co.uk/applications/rasp/.

#include "english_morpho_guesser.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void english_morpho_guesser::load(binary_decoder& data) {
  unsigned tags = data.next_2B();
  exceptions_tags.clear();
  exceptions_tags.reserve(tags);
  while (tags--) {
    unsigned len = data.next_1B();
    exceptions_tags.emplace_back(string(data.next<char>(len), len));
  }

  exceptions.load(data);
  negations.load(data);
}

%% machine tag_guesser; write data noerror nofinal;
void english_morpho_guesser::analyze(string_piece form, string_piece form_lc, vector<tagged_lemma>& lemmas) const {
  // Try exceptions list
  auto* exception = exceptions.at(form_lc.str, form_lc.len, [](pointer_decoder& data){
    for (unsigned len = data.next_1B(); len; len--) {
      data.next<char>(data.next_1B());
      data.next<uint16_t>(data.next_1B());
    }
  });

  if (exception) {
    // Found in exceptions list
    pointer_decoder data(exception);
    for (unsigned len = data.next_1B(); len; len--) {
      unsigned lemma_len = data.next_1B();
      string lemma(data.next<char>(lemma_len), lemma_len);
      for (unsigned tags = data.next_1B(); tags; tags--)
        lemmas.emplace_back(lemma, exceptions_tags[data.next_2B()]);
    }
  } else {
    // Try stripping negative prefix and use rule guesser
    string lemma_lc(form_lc.str, form_lc.len);
    // Try finding negative prefix
    unsigned negation_len = 0;
    for (unsigned prefix = 1; prefix <= form_lc.len; prefix++) {
      auto found = negations.at(form_lc.str, prefix, [](pointer_decoder& data){ data.next<unsigned char>(TOTAL); });
      if (!found) break;
      if (found[NEGATION_LEN]) {
        if (form_lc.len - prefix >= found[TO_FOLLOW]) negation_len = found[NEGATION_LEN];
      }
    }

    // Add default tags
    add(FW, lemma_lc, lemmas);
    add(JJ, lemma_lc, negation_len, lemmas);
    add(RB, lemma_lc, negation_len, lemmas);
    add(NN, lemma_lc, negation_len, lemmas);
    add_NNS(lemma_lc, negation_len, lemmas);

    // Add specialized tags
    const char* p = form_lc.str; int cs;
    bool added_JJR_RBR = false, added_JJS_RBS = false, added_SYM = false, added_CD = false;
    %%{
      getkey form_lc.str[form_lc.len - 1 - (p - form_lc.str)];
      variable pe (form_lc.str + form_lc.len);
      variable eof (form_lc.str + form_lc.len);

      action add_JJR_RBR { if (!added_JJR_RBR) added_JJR_RBR = true, add_JJR_RBR(lemma_lc, negation_len, lemmas); } JJR_RBR = 're' @ add_JJR_RBR | any* '-' ('re'|'erom'|'ssel') @ add_JJR_RBR;
      action add_JJS_RBS { if (!added_JJS_RBS) added_JJS_RBS = true, add_JJS_RBS(lemma_lc, negation_len, lemmas); } JJS_RBS = 'tse' @ add_JJS_RBS | any* '-' ('tse'|'tsom'|'tsael') @ add_JJS_RBS;
      action add_VBG { add_VBG(lemma_lc, lemmas); } VBG = ('gni' | 'ni' [^aeiouy]) @ add_VBG;
      action add_VBD_VBN { add_VBD_VBN(lemma_lc, lemmas); } VBD_VBN = 'de' @ add_VBD_VBN;
      action add_VBZ { add_VBZ(lemma_lc, lemmas); } VBZ = 's' [^s] @ add_VBZ;
      action add_VB_VBP { add(VB, lemma_lc, lemmas); add(VBP, lemma_lc, lemmas); } VB_VBP = ([^s] | 'ss') @ add_VB_VBP;
      action add_SYM { if (!added_SYM) added_SYM = true, add(SYM, lemma_lc, lemmas); } SYM = any* [^a-zA-Z0-9] @ add_SYM;
      action add_CD { if (!added_CD) added_CD = true, add(CD, lemma_lc, lemmas); } CD = any* [0-9\-] @ add_CD | [ixvcmd\.]+ % add_CD;

      main := JJR_RBR | JJS_RBS | VBG | VBD_VBN | VBZ | VB_VBP | SYM | CD;
      write init;
      write exec;
    }%%
  }

  // Add proper names
  analyze_proper_names(form, form_lc, lemmas);
}

bool english_morpho_guesser::analyze_proper_names(string_piece form, string_piece form_lc, vector<tagged_lemma>& lemmas) const {
  // NNP if form_lc != form or form.str[0] =~ /[0-9']/, NNPS if form_lc != form
  bool is_NNP = form.str != form_lc.str || (form.len && (*form.str == '\'' || (*form.str >= '0' && *form.str <= '9')));
  bool is_NNPS = form.str != form_lc.str;
  if (!is_NNP && !is_NNPS) return false;

  bool was_NNP = false, was_NNPS = false;
  for (auto&& lemma : lemmas) {
    was_NNP |= lemma.tag == NNP;
    was_NNPS |= lemma.tag == NNPS;
  }
  if (!((is_NNP && !was_NNP) || (is_NNPS && !was_NNPS))) return false;

  string lemma(form.str, form.len);
  if (is_NNP && !was_NNP) add(NNP, lemma, lemmas);
  if (is_NNPS && !was_NNPS) add_NNPS(lemma, lemmas);
  return true;
}

inline void english_morpho_guesser::add(const string& tag, const string& form, vector<tagged_lemma>& lemmas) const {
  lemmas.emplace_back(form, tag);
}

inline void english_morpho_guesser::add(const string& tag, const string& tag2, const string& form, vector<tagged_lemma>& lemmas) const {
  add(tag, form, lemmas);
  add(tag2, form, lemmas);
}

inline void english_morpho_guesser::add(const string& tag, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  lemmas.emplace_back(negation_len ? form.substr(negation_len) + "^" + form.substr(0, negation_len) : form, tag);
}

inline void english_morpho_guesser::add(const string& tag, const string& tag2, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  add(tag, form, negation_len, lemmas);
  add(tag2, form, negation_len, lemmas);
}

// Common definitions (written backwards)
#define REM(str, len) (str.substr(0, str.size() - len))
#define REM_ADD(str, len, add) (str.substr(0, str.size() - len).append(add))
%% machine common;
%% V = [aeiou];
%% VY = [aeiouy];
%% C = [bcdfghjklmnpqrstvwxyz];
%% C2 = 'bb'|'cc'|'dd'|'ff'|'gg'|'hh'|'jj'|'kk'|'ll'|'mm'|'nn'|'pp'|'qq'|'rr'|'ss'|'tt'|'vv'|'ww'|'xx'|'yy'|'zz';
%% CXY = C - 'y';
%% CXY2 = C2 - 'yy';
%% S = ([sxz] | 'h'[cs]);
%% SO = ([sxz] | [cs]'h');
%% S2 = ('ss' | 'zz');
%% PRE = ('eb' | 'xe' | 'ni' | 'sim' | 'erp' | 'orp' | 'er');
%% PREO = ('be' | 'ex' | 'in' | 'mis' | 'pre' | 'pro' | 're');

%% machine NNS; include common; write data noerror nofinal;
void english_morpho_guesser::add_NNS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  %%{
    getkey form[form.size() - 1 - (p - form.c_str() - negation_len)];
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { if (best > 'a') best = 'a', remove = 2, append = "an";    } a = 'nem' @ add_a;
    action add_b { if (best > 'b') best = 'b', remove = 1, append = nullptr; } b = 'seohs' @ add_b;
    action add_c { if (best > 'c') best = 'c', remove = 3, append = "fe";    } c = 'seviw' @ add_c;
    action add_d { if (best > 'd') best = 'd', remove = 2, append = nullptr; } d = 'sesu' C @ add_d;
    action add_e { if (best > 'e') best = 'e', remove = 1, append = nullptr; } e = 'ses' V @ add_e;
    action add_f { if (best > 'f') best = 'f', remove = 2, append = nullptr; } f = 'sez' CXY any @ add_f;
    action add_g { if (best > 'g') best = 'g', remove = 1, append = nullptr; } g = 'sez' VY @ add_g;
    action add_h { if (best > 'h') best = 'h', remove = 2, append = nullptr; } h = 'se' S2 @ add_h;
    action add_i { if (best > 'i') best = 'i', remove = 1, append = nullptr; } i = 'sesr' V any @ add_i;
    action add_j { if (best > 'j') best = 'j', remove = 1, append = nullptr; } j = 'sesno' @ add_j;
    action add_k { if (best > 'k') best = 'k', remove = 2, append = nullptr; } k = 'se' S @ add_k;
    action add_l { if (best > 'l') best = 'l', remove = 3, append = "y";     } l = 'sei' C any @ add_l;
    action add_m { if (best > 'm') best = 'm', remove = 2, append = nullptr; } m = 'seo' CXY @ add_m;
    action add_n { if (best > 'n') best = 'n', remove = 1, append = nullptr; } n = 's' any @ add_n;

    main := a | b | c | d | e | f | g | h | i | j | k | l | m | n;
    write init;
    write exec;
  }%%
  add(NNS, form.substr(0, form.size() - remove).append(append ? append : ""), negation_len, lemmas);
}

%% machine NNPS; include common; write data noerror nofinal;
void english_morpho_guesser::add_NNPS(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  %%{
    getkey form[form.size() - 1 - (p - form.c_str())];
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());
    Vi = V | [AEIOU];
    VYi = VY | [AEIOUY];
    Ci = C | [BCDFGHJKLMNPQRSTVWXYZ];
    CXYi = CXY | [BCDFGHJKLMNPQRSTVWXZ];
    Si = S | ([SXZ] | 'h'i [csCS]);
    S2i = S2 | ('ss'i | 'zz'i);

    action add_a { if (best > 'a') best = 'a', remove = 2, append = "AN";    } a = 'NEM' @ add_a;
    action add_b { if (best > 'b') best = 'b', remove = 2, append = "an";    } b = 'nem'i @ add_b;
    action add_c { if (best > 'c') best = 'c', remove = 1, append = nullptr; } c = 'seohs'i @ add_c;
    action add_d { if (best > 'd') best = 'd', remove = 3, append = "FE";    } d = 'SEVIW' @ add_d;
    action add_e { if (best > 'e') best = 'e', remove = 3, append = "fe";    } e = 'seviw'i @ add_e;
    action add_f { if (best > 'f') best = 'f', remove = 2, append = nullptr; } f = 'sesu'i Ci @ add_f;
    action add_g { if (best > 'g') best = 'g', remove = 1, append = nullptr; } g = 'ses'i Vi @ add_g;
    action add_h { if (best > 'h') best = 'h', remove = 2, append = nullptr; } h = 'sez'i CXYi any @ add_h;
    action add_i { if (best > 'i') best = 'i', remove = 1, append = nullptr; } i = 'sez'i VYi @ add_i;
    action add_j { if (best > 'j') best = 'j', remove = 2, append = nullptr; } j = 'se'i S2i @ add_j;
    action add_k { if (best > 'k') best = 'k', remove = 1, append = nullptr; } k = 'sesr'i Vi any @ add_k;
    action add_l { if (best > 'l') best = 'l', remove = 1, append = nullptr; } l = 'sesno'i @ add_l;
    action add_m { if (best > 'm') best = 'm', remove = 2, append = nullptr; } m = 'se'i Si @ add_m;
    action add_n { if (best > 'n') best = 'n', remove = 3, append = "Y";     } n = 'SEI' Ci any @ add_n;
    action add_o { if (best > 'o') best = 'o', remove = 3, append = "y";     } o = 'sei'i Ci any @ add_o;
    action add_p { if (best > 'p') best = 'p', remove = 2, append = nullptr; } p = 'seo'i CXYi @ add_p;
    action add_q { if (best > 'q') best = 'q', remove = 1, append = nullptr; } q = 's'i any @ add_q;

    main := a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q;
    write init;
    write exec;
  }%%
  add(NNPS, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}

%% machine VBG; include common; write data noerror nofinal;
void english_morpho_guesser::add_VBG(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  %%{
    getkey form[form.size() - 1 - (p - form.c_str())];
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { if (best > 'a') best = 'a', remove = 3, append = nullptr; } a = 'gniz' CXY @ add_a;
    action add_b { if (best > 'b') best = 'b', remove = 3, append = "e";     } b = 'gniz' VY @ add_b;
    action add_c { if (best > 'c') best = 'c', remove = 3, append = nullptr; } c = ('gnikci' | 'gni' CXY2 V C | 'gnill' V C | 'gni' S2) @ add_c | ('gnign' V C PRE* | 'gni' CXY ) % add_c;
    action add_d { if (best > 'd') best = 'd', remove = 3, append = "e";     } d = 'gnini' C @ add_d;
    action add_e { if (best > 'e') best = 'e', remove = 3, append = nullptr; } e = 'gni' [npwx] V C @ add_e;
    action add_f { if (best > 'f') best = 'f', remove = 3, append = "e";     } f = ('gnitel' C | 'gnid' V 'u' | 'gni' C V 'uq') @ add_f | 'gnit' [ei] C+ PRE* % add_f;
    action add_g { if (best > 'g') best = 'g', remove = 3, append = nullptr; } g = ('gnitae' CXY CXY PRE | 'gnit' [ei]) @ add_g;
    action add_h { if (best > 'h') best = 'h', remove = 3, append = "e";     } h = 'gnitae' CXY CXY V @ add_h;
    action add_i { if (best > 'i') best = 'i', remove = 3, append = nullptr; } i = 'gnita' [eo] any @ add_i;
    action add_j { if (best > 'j') best = 'j', remove = 3, append = "e";     } j = ('gni' [cgsv] V V | 'gnita' V any) @ add_j;
    action add_k { if (best > 'k') best = 'k', remove = 3, append = nullptr; } k = ('gnil' [rw] any | 'gni' C V V) @ add_k;
    action add_l { if (best > 'l') best = 'l', remove = 3, append = "e";     } l = ('gni' [cglsv] CXY | 'gniht' any) @ add_l;
    action add_m { if (best > 'm') best = 'm', remove = 3, append = nullptr; } m = 'gni' CXY CXY @ add_m;
    action add_n { if (best > 'n') best = 'n', remove = 3, append = "e";     } n = 'gniu' @ add_n;
    action add_o { if (best > 'o') best = 'o', remove = 3, append = nullptr; } o = ('gnio' CXY | 'gniy' | 'gni' VY VY) @ add_o;
    action add_p { if (best > 'p') best = 'p', remove = 3, append = "e";     } p = 'gniro' [clt] C @ add_p | 'gniro' C+ PRE* % add_p;
    action add_q { if (best > 'q') best = 'q', remove = 3, append = nullptr; } q = 'gnir' [eo] @ add_q;
    action add_r { if (best > 'r') best = 'r', remove = 3, append = "e";     } r = 'gni' @ add_r;

    main := a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r;
    write init;
    write exec;
  }%%
  add(VBG, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}

%% machine VBD_VBN; include common; write data noerror nofinal;
void english_morpho_guesser::add_VBD_VBN(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  %%{
    getkey form[form.size() - 1 - (p - form.c_str())];
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { if (best > 'a') best = 'a', remove = 1, append = nullptr; } a = 'ne' @ add_a;
    action add_b { if (best > 'b') best = 'b', remove = 2, append = nullptr; } b = 'dez' CXY @ add_b;
    action add_c { if (best > 'c') best = 'c', remove = 1, append = nullptr; } c = 'dez' VY @ add_c;
    action add_d { if (best > 'd') best = 'd', remove = 2, append = nullptr; } d = ('dekci' | 'de' CXY2 V C | 'dell' V C | 'de' S2) @ add_d | ('degn' V C PRE* | 'de' CXY) % add_d;
    action add_e { if (best > 'e') best = 'e', remove = 1, append = nullptr; } e = ('dero' [clnt] C | 'deni' C) @ add_e;
    action add_f { if (best > 'f') best = 'f', remove = 2, append = nullptr; } f = 'de' [npwx] V C @ add_f;
    action add_g { if (best > 'g') best = 'g', remove = 1, append = nullptr; } g = 'dero' C+ PRE* % add_g;
    action add_h { if (best > 'h') best = 'h', remove = 2, append = nullptr; } h = 'der' [eo] @ add_h;
    action add_i { if (best > 'i') best = 'i', remove = 3, append = "y";     } i = 'dei' C @ add_i;
    action add_j { if (best > 'j') best = 'j', remove = 1, append = nullptr; } j = ('detel' C | 'ded' V 'u' | 'de' C V 'uq') @ add_j | 'det' [ei] C+ PRE* % add_j;
    action add_k { if (best > 'k') best = 'k', remove = 2, append = nullptr; } k = ('detae' CXY CXY PRE | 'det' [ei]) @ add_k;
    action add_l { if (best > 'l') best = 'l', remove = 1, append = nullptr; } l = 'detae' CXY CXY V @ add_l;
    action add_m { if (best > 'm') best = 'm', remove = 2, append = nullptr; } m = 'deta' [eo] any @ add_m;
    action add_n { if (best > 'n') best = 'n', remove = 1, append = nullptr; } n = ('de' [cgsv] V V | 'deta' V any) @ add_n;
    action add_o { if (best > 'o') best = 'o', remove = 2, append = nullptr; } o = ('del' [rw] any | 'de' C V V) @ add_o;
    action add_p { if (best > 'p') best = 'p', remove = 1, append = nullptr; } p = ('de' [cglsv] CXY | 'deu' | 'deht' any) @ add_p;
    action add_q { if (best > 'q') best = 'q', remove = 2, append = nullptr; } q = ('de' VY VY | 'de' CXY CXY) @ add_q;
    action add_r { if (best > 'r') best = 'r', remove = 1, append = nullptr; } r = 'de' @ add_r;

    main := a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r;
    write init;
    write exec;
  }%%
  add(VBD, VBN, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}

%% machine VBZ; include common; write data noerror nofinal;
void english_morpho_guesser::add_VBZ(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  %%{
    getkey form[form.size() - 1 - (p - form.c_str())];
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { if (best > 'a') best = 'a', remove = 1, append = nullptr; } a = 'ses' V @ add_a;
    action add_b { if (best > 'b') best = 'b', remove = 2, append = nullptr; } b = 'sez' CXY any @ add_b;
    action add_c { if (best > 'c') best = 'c', remove = 1, append = nullptr; } c = 'sez' VY @ add_c;
    action add_d { if (best > 'd') best = 'd', remove = 2, append = nullptr; } d = 'se' S2 @ add_d;
    action add_e { if (best > 'e') best = 'e', remove = 1, append = nullptr; } e = ('sesno' | 'sesr' V any) @ add_e;
    action add_f { if (best > 'f') best = 'f', remove = 2, append = nullptr; } f = 'se' S @ add_f;
    action add_g { if (best > 'g') best = 'g', remove = 3, append = "y";     } g = 'sei' C any @ add_g;
    action add_h { if (best > 'h') best = 'h', remove = 2, append = nullptr; } h = 'seo' CXY @ add_h;
    action add_i { if (best > 'i') best = 'i', remove = 1, append = nullptr; } i = 's' any @ add_i;

    main := a | b | c | d | e | f | g | h | i;
    write init;
    write exec;
  }%%
  add(VBZ, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}

%% machine JJR_RBR; include common; write data noerror nofinal;
void english_morpho_guesser::add_JJR_RBR(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  %%{
    getkey form[form.size() - 1 - (p - form.c_str() - negation_len)];
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { if (best > 'a') best = 'a', remove = 2, append = nullptr; } a = 'rell' [^e] @ add_a;
    action add_b { if (best > 'b') best = 'b', remove = 3, append = nullptr; } b = 're' C2 @ add_b;
    action add_c { if (best > 'c') best = 'c', remove = 3, append = "y";     } c = 'rei' @ add_c;
    action add_d { if (best > 'd') best = 'd', remove = 2, append = nullptr; } d = ('re' [npwx] V C | 're' C V V) @ add_d;
    action add_e { if (best > 'e') best = 'e', remove = 1, append = nullptr; } e = ('re' [eu] | 're' [cglsv] (CXY - 'n') | 're' C V) @ add_e;
    action add_f { if (best > 'f') best = 'f', remove = 2, append = nullptr; } f = 're' any @ add_f;

    main := a | b | c | d | e | f;
    write init;
    write exec;
  }%%
  add(JJR, RBR, form.substr(0, form.size() - remove).append(append ? append : ""), negation_len, lemmas);
}

%% machine JJS_RBS; include common; write data noerror nofinal;
void english_morpho_guesser::add_JJS_RBS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  %%{
    getkey form[form.size() - 1 - (p - form.c_str() - negation_len)];
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { if (best > 'a') best = 'a', remove = 3, append = nullptr; } a = 'tsell' [^e] @ add_a;
    action add_b { if (best > 'b') best = 'b', remove = 4, append = nullptr; } b = 'tse' C2 @ add_b;
    action add_c { if (best > 'c') best = 'c', remove = 4, append = "y";     } c = 'tsei' @ add_c;
    action add_d { if (best > 'd') best = 'd', remove = 3, append = nullptr; } d = ('tse' [npwx] V C | 'tse' C V V) @ add_d;
    action add_e { if (best > 'e') best = 'e', remove = 2, append = nullptr; } e = ('tse' [cglsv] (CXY - 'n') | 'tse' C V) @ add_e;
    action add_f { if (best > 'f') best = 'f', remove = 3, append = nullptr; } f = 'tse' any{3} @ add_f;

    main := a | b | c | d | e | f;
    write init;
    write exec;
  }%%
  add(JJS, RBS, form.substr(0, form.size() - remove).append(append ? append : ""), negation_len, lemmas);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
