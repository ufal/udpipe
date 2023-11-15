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


static const char _english_tokenizer_split_token_key_offsets[] = {
	0, 0, 16, 20, 22, 26, 28, 30, 
	32, 34, 36, 44, 46, 50, 52, 54, 
	56, 58, 60, 62, 64, 66, 68, 72, 
	74, 76, 78, 80, 82, 82
};

static const unsigned char _english_tokenizer_split_token_trans_keys[] = {
	65u, 68u, 69u, 76u, 77u, 78u, 83u, 84u, 
	97u, 100u, 101u, 108u, 109u, 110u, 115u, 116u, 
	78u, 84u, 110u, 116u, 78u, 110u, 65u, 79u, 
	97u, 111u, 87u, 119u, 71u, 103u, 84u, 116u, 
	79u, 111u, 39u, 161u, 77u, 82u, 86u, 89u, 
	109u, 114u, 118u, 121u, 77u, 109u, 69u, 73u, 
	101u, 105u, 76u, 108u, 39u, 161u, 68u, 100u, 
	76u, 108u, 39u, 161u, 69u, 101u, 82u, 114u, 
	79u, 111u, 77u, 109u, 39u, 79u, 111u, 161u, 
	78u, 110u, 78u, 110u, 78u, 110u, 65u, 97u, 
	67u, 99u, 0
};

static const char _english_tokenizer_split_token_single_lengths[] = {
	0, 16, 4, 2, 4, 2, 2, 2, 
	2, 2, 8, 2, 4, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 4, 2, 
	2, 2, 2, 2, 0, 0
};

static const char _english_tokenizer_split_token_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0
};

static const unsigned char _english_tokenizer_split_token_index_offsets[] = {
	0, 0, 17, 22, 25, 30, 33, 36, 
	39, 42, 45, 54, 57, 62, 65, 68, 
	71, 74, 77, 80, 83, 86, 89, 94, 
	97, 100, 103, 106, 109, 110
};

static const char _english_tokenizer_split_token_indicies[] = {
	0, 2, 3, 4, 2, 5, 2, 6, 
	0, 2, 3, 4, 2, 5, 2, 6, 
	1, 7, 8, 7, 8, 1, 9, 9, 
	1, 10, 11, 10, 11, 1, 12, 12, 
	1, 12, 12, 1, 13, 13, 1, 11, 
	11, 1, 14, 14, 1, 15, 2, 2, 
	16, 15, 2, 2, 16, 1, 17, 17, 
	1, 18, 11, 18, 11, 1, 12, 12, 
	1, 19, 19, 1, 12, 12, 1, 2, 
	2, 1, 20, 20, 1, 21, 21, 1, 
	22, 22, 1, 23, 23, 1, 12, 12, 
	1, 24, 25, 25, 24, 1, 14, 14, 
	1, 26, 26, 1, 27, 27, 1, 28, 
	28, 1, 12, 12, 1, 1, 1, 0
};

static const char _english_tokenizer_split_token_trans_targs[] = {
	2, 0, 9, 10, 16, 17, 22, 3, 
	7, 4, 5, 6, 28, 8, 29, 11, 
	14, 12, 13, 15, 18, 19, 20, 21, 
	23, 24, 25, 26, 27
};

static const char _english_tokenizer_split_token_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 0, 0, 0, 0, 0, 2, 1, 
	1, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 1, 0, 0
};

static const char _english_tokenizer_split_token_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 3, 0
};

static const int english_tokenizer_split_token_start = 1;





void english_tokenizer::split_token(vector<token_range>& tokens) {
  if (tokens.empty() || chars[tokens.back().start].cat & ~unilib::unicode::L) return;

  size_t index = tokens.back().start, end = index + tokens.back().length;
  int cs;
  size_t split_mark = 0, split_len = 0;
  
	{
	cs = english_tokenizer_split_token_start;
	}

	{
	int _klen;
	const unsigned char *_keys;
	int _trans;

	if ( ( index) == ( end) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _english_tokenizer_split_token_trans_keys + _english_tokenizer_split_token_key_offsets[cs];
	_trans = _english_tokenizer_split_token_index_offsets[cs];

	_klen = _english_tokenizer_split_token_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( ragel_char(chars[tokens.back().start + end - index - 1])) < *_mid )
				_upper = _mid - 1;
			else if ( ( ragel_char(chars[tokens.back().start + end - index - 1])) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _english_tokenizer_split_token_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( ragel_char(chars[tokens.back().start + end - index - 1])) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( ragel_char(chars[tokens.back().start + end - index - 1])) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _english_tokenizer_split_token_indicies[_trans];
	cs = _english_tokenizer_split_token_trans_targs[_trans];

	if ( _english_tokenizer_split_token_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _english_tokenizer_split_token_trans_actions[_trans] ) {
	case 1:
	{ split_mark = index - tokens.back().start + 1; }
	break;
	case 2:
	{ split_mark = index - tokens.back().start + 1; }
	{ split_len = split_mark; {( index)++; goto _out; } }
	break;
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++( index) != ( end) )
		goto _resume;
	_test_eof: {}
	if ( ( index) == ( end) )
	{
	switch ( _english_tokenizer_split_token_eof_actions[cs] ) {
	case 3:
	{ split_len = split_mark; {( index)++; goto _out; } }
	break;
	}
	}

	_out: {}
	}



  if (split_len && split_len < end) {
    tokens.back().length -= split_len;
    tokens.emplace_back(end - split_len, split_len);
  }
}


static const char _english_tokenizer_cond_offsets[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2
};

static const char _english_tokenizer_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 2, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0
};

static const short _english_tokenizer_cond_keys[] = {
	43u, 43u, 45u, 45u, 0
};

static const char _english_tokenizer_cond_spaces[] = {
	1, 0, 0
};

static const unsigned char _english_tokenizer_key_offsets[] = {
	0, 0, 17, 29, 43, 46, 49, 52, 
	55, 60, 63, 98, 103, 108, 109, 114, 
	115, 120, 134, 141, 145, 150, 153, 168, 
	181, 195
};

static const short _english_tokenizer_trans_keys[] = {
	13u, 32u, 34u, 40u, 91u, 96u, 123u, 129u, 
	133u, 135u, 147u, 150u, 162u, 9u, 10u, 65u, 
	90u, 34u, 40u, 91u, 96u, 123u, 129u, 133u, 
	135u, 150u, 162u, 65u, 90u, 13u, 32u, 34u, 
	39u, 41u, 59u, 93u, 125u, 139u, 141u, 147u, 
	161u, 9u, 10u, 159u, 48u, 57u, 159u, 48u, 
	57u, 159u, 48u, 57u, 159u, 48u, 57u, 43u, 
	45u, 159u, 48u, 57u, 159u, 48u, 57u, 9u, 
	10u, 13u, 32u, 33u, 44u, 46u, 47u, 63u, 
	129u, 131u, 135u, 142u, 147u, 157u, 159u, 160u, 
	301u, 557u, 811u, 1067u, 0u, 42u, 48u, 57u, 
	58u, 64u, 65u, 90u, 91u, 96u, 97u, 122u, 
	123u, 255u, 9u, 10u, 13u, 32u, 147u, 9u, 
	10u, 13u, 32u, 147u, 13u, 9u, 10u, 13u, 
	32u, 147u, 10u, 9u, 10u, 13u, 32u, 147u, 
	13u, 32u, 34u, 39u, 41u, 59u, 93u, 125u, 
	139u, 141u, 147u, 161u, 9u, 10u, 44u, 46u, 
	69u, 101u, 159u, 48u, 57u, 44u, 46u, 69u, 
	101u, 69u, 101u, 159u, 48u, 57u, 159u, 48u, 
	57u, 39u, 45u, 129u, 131u, 135u, 151u, 155u, 
	157u, 161u, 65u, 90u, 97u, 122u, 142u, 143u, 
	45u, 129u, 131u, 135u, 151u, 155u, 157u, 65u, 
	90u, 97u, 122u, 142u, 143u, 39u, 129u, 131u, 
	135u, 151u, 155u, 157u, 161u, 65u, 90u, 97u, 
	122u, 142u, 143u, 159u, 48u, 57u, 0
};

static const char _english_tokenizer_single_lengths[] = {
	0, 13, 10, 12, 1, 1, 1, 1, 
	3, 1, 21, 5, 5, 1, 5, 1, 
	5, 12, 5, 4, 3, 1, 9, 7, 
	8, 1
};

static const char _english_tokenizer_range_lengths[] = {
	0, 2, 1, 1, 1, 1, 1, 1, 
	1, 1, 7, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 3, 3, 
	3, 1
};

static const unsigned char _english_tokenizer_index_offsets[] = {
	0, 0, 16, 28, 42, 45, 48, 51, 
	54, 59, 62, 91, 97, 103, 105, 111, 
	113, 119, 133, 140, 145, 150, 153, 166, 
	177, 189
};

static const char _english_tokenizer_indicies[] = {
	1, 1, 2, 2, 2, 2, 2, 3, 
	2, 3, 1, 2, 2, 1, 3, 0, 
	2, 2, 2, 2, 2, 3, 2, 3, 
	2, 2, 3, 0, 4, 4, 5, 5, 
	5, 5, 5, 5, 5, 5, 4, 5, 
	4, 0, 6, 6, 0, 7, 7, 0, 
	8, 8, 0, 9, 9, 0, 10, 10, 
	11, 11, 0, 11, 11, 0, 13, 14, 
	15, 13, 16, 12, 16, 12, 16, 19, 
	19, 19, 19, 13, 19, 18, 16, 12, 
	20, 12, 20, 12, 18, 12, 19, 12, 
	19, 12, 17, 13, 14, 15, 13, 13, 
	21, 13, 22, 23, 13, 13, 21, 25, 
	24, 13, 22, 26, 13, 13, 21, 25, 
	24, 13, 23, 26, 13, 13, 21, 4, 
	4, 5, 5, 5, 5, 5, 5, 5, 
	5, 4, 5, 4, 27, 28, 29, 30, 
	30, 18, 18, 27, 28, 29, 30, 30, 
	27, 30, 30, 9, 9, 27, 11, 11, 
	27, 31, 32, 19, 19, 19, 19, 19, 
	19, 31, 19, 19, 19, 27, 32, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	27, 31, 19, 19, 19, 19, 19, 19, 
	31, 19, 19, 19, 27, 18, 18, 27, 
	0
};

static const char _english_tokenizer_trans_targs[] = {
	10, 1, 2, 10, 1, 3, 5, 6, 
	19, 20, 9, 21, 10, 11, 12, 16, 
	17, 0, 18, 22, 25, 10, 13, 14, 
	10, 10, 15, 10, 4, 7, 8, 23, 
	24
};

static const char _english_tokenizer_trans_actions[] = {
	1, 0, 0, 2, 3, 0, 0, 0, 
	4, 4, 0, 0, 7, 0, 0, 0, 
	4, 0, 4, 0, 0, 8, 0, 0, 
	9, 10, 0, 11, 0, 0, 0, 0, 
	0
};

static const char _english_tokenizer_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 5, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0
};

static const char _english_tokenizer_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 6, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0
};

static const unsigned char _english_tokenizer_eof_trans[] = {
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 0, 22, 22, 25, 22, 25, 
	22, 28, 28, 28, 28, 28, 28, 28, 
	28, 28
};

static const int english_tokenizer_start = 10;





english_tokenizer::english_tokenizer(unsigned version) : ragel_tokenizer(version <= 1 ? 1 : 2) {}

bool english_tokenizer::next_sentence(vector<token_range>& tokens) {
  using namespace unilib;

  int cs, act;
  size_t ts, te;
  size_t whitespace = 0; // Suppress "may be uninitialized" warning

  while (tokenize_url_email(tokens))
    if (emergency_sentence_split(tokens))
      return true;
  
	{
	cs = english_tokenizer_start;
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
	switch ( _english_tokenizer_from_state_actions[cs] ) {
	case 6:
	{ts = ( current);}
	break;
	}

	_widec = ( ragel_char(chars[current]));
	_klen = _english_tokenizer_cond_lengths[cs];
	_keys = _english_tokenizer_cond_keys + (_english_tokenizer_cond_offsets[cs]*2);
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
				switch ( _english_tokenizer_cond_spaces[_english_tokenizer_cond_offsets[cs] + ((_mid - _keys)>>1)] ) {
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

	_keys = _english_tokenizer_trans_keys + _english_tokenizer_key_offsets[cs];
	_trans = _english_tokenizer_index_offsets[cs];

	_klen = _english_tokenizer_single_lengths[cs];
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

	_klen = _english_tokenizer_range_lengths[cs];
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
	_trans = _english_tokenizer_indicies[_trans];
_eof_trans:
	cs = _english_tokenizer_trans_targs[_trans];

	if ( _english_tokenizer_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _english_tokenizer_trans_actions[_trans] ) {
	case 3:
	{ whitespace = current; }
	break;
	case 4:
	{te = ( current)+1;}
	break;
	case 7:
	{te = ( current)+1;{ tokens.emplace_back(ts, te - ts);
          split_token(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	case 2:
	{te = ( current)+1;{
          bool eos = is_eos(tokens, chars[ts].chr, &abbreviations);
          for (current = ts; current < whitespace; current++)
            tokens.emplace_back(current, 1);
          {( current) = (( whitespace))-1;}
          if (eos) {( current)++; goto _out; }
        }}
	break;
	case 10:
	{te = ( current)+1;{
          if (!tokens.empty()) {( current)++; goto _out; }
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	case 11:
	{te = ( current);( current)--;{ tokens.emplace_back(ts, te - ts);
          split_token(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	case 8:
	{te = ( current);( current)--;{
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	case 9:
	{te = ( current);( current)--;{
          if (!tokens.empty()) {( current)++; goto _out; }
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	case 1:
	{{( current) = ((te))-1;}{ tokens.emplace_back(ts, te - ts);
          split_token(tokens);
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	}

_again:
	switch ( _english_tokenizer_to_state_actions[cs] ) {
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
	if ( _english_tokenizer_eof_trans[cs] > 0 ) {
		_trans = _english_tokenizer_eof_trans[cs] - 1;
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
