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


static const char _czech_tokenizer_cond_offsets[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2
};

static const char _czech_tokenizer_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 2, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const short _czech_tokenizer_cond_keys[] = {
	43u, 43u, 45u, 45u, 0
};

static const char _czech_tokenizer_cond_spaces[] = {
	1, 0, 0
};

static const unsigned char _czech_tokenizer_key_offsets[] = {
	0, 0, 17, 29, 43, 46, 51, 54, 
	89, 94, 98, 101, 105, 110, 111, 116, 
	117, 122, 136, 143, 148, 151, 163
};

static const short _czech_tokenizer_trans_keys[] = {
	13u, 32u, 34u, 40u, 91u, 96u, 123u, 129u, 
	133u, 135u, 147u, 150u, 162u, 9u, 10u, 65u, 
	90u, 34u, 40u, 91u, 96u, 123u, 129u, 133u, 
	135u, 150u, 162u, 65u, 90u, 13u, 32u, 34u, 
	39u, 41u, 59u, 93u, 125u, 139u, 141u, 147u, 
	161u, 9u, 10u, 159u, 48u, 57u, 43u, 45u, 
	159u, 48u, 57u, 159u, 48u, 57u, 9u, 10u, 
	13u, 32u, 33u, 44u, 46u, 47u, 63u, 129u, 
	131u, 135u, 142u, 147u, 157u, 159u, 160u, 301u, 
	557u, 811u, 1067u, 0u, 42u, 48u, 57u, 58u, 
	64u, 65u, 90u, 91u, 96u, 97u, 122u, 123u, 
	255u, 9u, 10u, 13u, 32u, 147u, 9u, 13u, 
	32u, 147u, 9u, 32u, 147u, 9u, 10u, 32u, 
	147u, 9u, 10u, 13u, 32u, 147u, 13u, 9u, 
	10u, 13u, 32u, 147u, 10u, 9u, 10u, 13u, 
	32u, 147u, 13u, 32u, 34u, 39u, 41u, 59u, 
	93u, 125u, 139u, 141u, 147u, 161u, 9u, 10u, 
	44u, 46u, 69u, 101u, 159u, 48u, 57u, 69u, 
	101u, 159u, 48u, 57u, 159u, 48u, 57u, 129u, 
	131u, 135u, 151u, 155u, 157u, 65u, 90u, 97u, 
	122u, 142u, 143u, 159u, 48u, 57u, 0
};

static const char _czech_tokenizer_single_lengths[] = {
	0, 13, 10, 12, 1, 3, 1, 21, 
	5, 4, 3, 4, 5, 1, 5, 1, 
	5, 12, 5, 3, 1, 6, 1
};

static const char _czech_tokenizer_range_lengths[] = {
	0, 2, 1, 1, 1, 1, 1, 7, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 1, 1, 1, 3, 1
};

static const unsigned char _czech_tokenizer_index_offsets[] = {
	0, 0, 16, 28, 42, 45, 50, 53, 
	82, 88, 93, 97, 102, 108, 110, 116, 
	118, 124, 138, 145, 150, 153, 163
};

static const char _czech_tokenizer_indicies[] = {
	1, 1, 2, 2, 2, 2, 2, 3, 
	2, 3, 1, 2, 2, 1, 3, 0, 
	2, 2, 2, 2, 2, 3, 2, 3, 
	2, 2, 3, 0, 4, 4, 5, 5, 
	5, 5, 5, 5, 5, 5, 4, 5, 
	4, 0, 6, 6, 0, 7, 7, 8, 
	8, 0, 8, 8, 0, 10, 11, 12, 
	10, 13, 9, 13, 9, 13, 16, 16, 
	16, 16, 10, 16, 15, 13, 9, 17, 
	9, 17, 9, 15, 9, 16, 9, 16, 
	9, 14, 10, 19, 20, 10, 10, 18, 
	10, 21, 10, 10, 18, 10, 10, 10, 
	18, 10, 21, 10, 10, 18, 10, 22, 
	23, 10, 10, 18, 25, 24, 10, 22, 
	26, 10, 10, 18, 25, 24, 10, 23, 
	26, 10, 10, 18, 4, 4, 5, 5, 
	5, 5, 5, 5, 5, 5, 4, 5, 
	4, 27, 28, 28, 29, 29, 15, 15, 
	27, 29, 29, 6, 6, 27, 8, 8, 
	27, 16, 16, 16, 16, 16, 16, 16, 
	16, 16, 27, 15, 15, 27, 0
};

static const char _czech_tokenizer_trans_targs[] = {
	7, 1, 2, 7, 1, 3, 19, 6, 
	20, 7, 8, 12, 16, 17, 0, 18, 
	21, 22, 7, 9, 11, 10, 13, 14, 
	7, 7, 15, 7, 4, 5
};

static const char _czech_tokenizer_trans_actions[] = {
	1, 0, 0, 2, 3, 0, 4, 0, 
	0, 7, 0, 0, 0, 4, 0, 4, 
	0, 0, 8, 0, 0, 0, 0, 0, 
	9, 10, 0, 11, 0, 0
};

static const char _czech_tokenizer_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 5, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const char _czech_tokenizer_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 6, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const unsigned char _czech_tokenizer_eof_trans[] = {
	0, 1, 1, 1, 1, 1, 1, 0, 
	19, 19, 19, 19, 19, 25, 19, 25, 
	19, 28, 28, 28, 28, 28, 28
};

static const int czech_tokenizer_start = 7;





// The list of lower cased words that when preceding eos do not end sentence.
const unordered_set<string> czech_tokenizer::abbreviations_czech = {
  // Titles
  "prof", "csc", "drsc", "doc", "phd", "ph", "d",
  "judr", "mddr", "mudr", "mvdr", "paeddr", "paedr", "phdr", "rndr", "rsdr", "dr",
  "ing", "arch", "mgr", "bc", "mag", "mba", "bca", "mga",
  "gen", "plk", "pplk", "npor", "por", "ppor", "kpt", "mjr", "sgt", "pls", "p", "s",
  "p", "pí", "fa", "fy", "mr", "mrs", "ms", "miss", "tr", "sv",
  // Geographic names
  "angl", "fr", "čes", "ces", "čs", "cs", "slov", "něm", "nem", "it", "pol", "maď", "mad", "rus",
  "sev", "vých", "vych", "již", "jiz", "záp", "zap",
  // Common abbrevs
  "adr", "č", "c", "eg", "ev", "g", "hod", "j", "kr", "m", "max", "min", "mj", "např", "napr",
  "okr", "popř", "popr", "pozn", "r", "ř", "red", "rep", "resp", "srov", "st", "stř", "str",
  "sv", "tel", "tj", "tzv", "ú", "u", "uh", "ul", "um", "zl", "zn",
};

const unordered_set<string> czech_tokenizer::abbreviations_slovak = {
  // Titles
  "prof", "csc", "drsc", "doc", "phd", "ph", "d",
  "judr", "mddr", "mudr", "mvdr", "paeddr", "paedr", "phdr", "rndr", "rsdr", "dr",
  "ing", "arch", "mgr", "bc", "mag", "mba", "bca", "mga",
  "gen", "plk", "pplk", "npor", "por", "ppor", "kpt", "mjr", "sgt", "pls", "p", "s",
  "p", "pí", "fa", "fy", "mr", "mrs", "ms", "miss", "tr", "sv",
  // Geographic names
  "angl", "fr", "čes", "ces", "čs", "cs", "slov", "nem", "it", "poľ", "pol", "maď", "mad",
  "rus", "sev", "vých", "vych", "juž", "juz", "záp", "zap",
  // Common abbrevs
  "adr", "č", "c", "eg", "ev", "g", "hod", "j", "kr", "m", "max", "min", "mj", "napr",
  "okr", "popr", "pozn", "r", "red", "rep", "resp", "srov", "st", "str",
  "sv", "tel", "tj", "tzv", "ú", "u", "uh", "ul", "um", "zl", "zn",
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
  
	{
	cs = czech_tokenizer_start;
	ts = 0;
	te = 0;
	act = 0;
	}

	{
	int _klen;
	const short *_keys;
	int _trans;
	short _widec;

	if ( ( current) == ( (chars.size() - 1)) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	switch ( _czech_tokenizer_from_state_actions[cs] ) {
	case 6:
	{ts = ( current);}
	break;
	}

	_widec = ( ragel_char(chars[current]));
	_klen = _czech_tokenizer_cond_lengths[cs];
	_keys = _czech_tokenizer_cond_keys + (_czech_tokenizer_cond_offsets[cs]*2);
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				switch ( _czech_tokenizer_cond_spaces[_czech_tokenizer_cond_offsets[cs] + ((_mid - _keys)>>1)] ) {
	case 0: {
		_widec = (short)(256u + (( ragel_char(chars[current])) - 0u));
		if ( 
 !current || (chars[current-1].cat & ~(unicode::L | unicode::M | unicode::N | unicode::Pd))  ) _widec += 256;
		break;
	}
	case 1: {
		_widec = (short)(768u + (( ragel_char(chars[current])) - 0u));
		if ( 
 !current || ((chars[current-1].cat & ~(unicode::L | unicode::M | unicode::N)) && chars[current-1].chr != '+')  ) _widec += 256;
		break;
	}
				}
				break;
			}
		}
	}

	_keys = _czech_tokenizer_trans_keys + _czech_tokenizer_key_offsets[cs];
	_trans = _czech_tokenizer_index_offsets[cs];

	_klen = _czech_tokenizer_single_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( _widec < *_mid )
				_upper = _mid - 1;
			else if ( _widec > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _czech_tokenizer_range_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _czech_tokenizer_indicies[_trans];
_eof_trans:
	cs = _czech_tokenizer_trans_targs[_trans];

	if ( _czech_tokenizer_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _czech_tokenizer_trans_actions[_trans] ) {
	case 3:
	{ whitespace = current; }
	break;
	case 4:
	{te = ( current)+1;}
	break;
	case 7:
	{te = ( current)+1;{ tokens.emplace_back(ts, te - ts);
          merge_hyphenated(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) {( current)++; goto _out; }
          while (tokenize_url_email(tokens));
          {( current) = (( current))-1;}
        }}
	break;
	case 2:
	{te = ( current)+1;{
          bool eos = is_eos(tokens, chars[ts].chr, abbreviations);
          for (current = ts; current < whitespace; current++)
            tokens.emplace_back(current, 1);
          {( current) = (( whitespace))-1;}
          if (eos) {( current)++; goto _out; }
        }}
	break;
	case 10:
	{te = ( current)+1;{ if (!tokens.empty()) {( current)++; goto _out; } }}
	break;
	case 11:
	{te = ( current);( current)--;{ tokens.emplace_back(ts, te - ts);
          merge_hyphenated(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) {( current)++; goto _out; }
          while (tokenize_url_email(tokens));
          {( current) = (( current))-1;}
        }}
	break;
	case 8:
	{te = ( current);( current)--;}
	break;
	case 9:
	{te = ( current);( current)--;{ if (!tokens.empty()) {( current)++; goto _out; } }}
	break;
	case 1:
	{{( current) = ((te))-1;}{ tokens.emplace_back(ts, te - ts);
          merge_hyphenated(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) {( current)++; goto _out; }
          while (tokenize_url_email(tokens));
          {( current) = (( current))-1;}
        }}
	break;
	}

_again:
	switch ( _czech_tokenizer_to_state_actions[cs] ) {
	case 5:
	{ts = 0;}
	break;
	}

	if ( cs == 0 )
		goto _out;
	if ( ++( current) != ( (chars.size() - 1)) )
		goto _resume;
	_test_eof: {}
	if ( ( current) == ( (chars.size() - 1)) )
	{
	if ( _czech_tokenizer_eof_trans[cs] > 0 ) {
		_trans = _czech_tokenizer_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}


  (void)act; // Suppress unused variable warning

  return !tokens.empty();
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
