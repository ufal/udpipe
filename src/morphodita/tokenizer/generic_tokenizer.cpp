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


static const char _generic_tokenizer_cond_offsets[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2
};

static const char _generic_tokenizer_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 2, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const short _generic_tokenizer_cond_keys[] = {
	43u, 43u, 45u, 45u, 0
};

static const char _generic_tokenizer_cond_spaces[] = {
	1, 0, 0
};

static const unsigned char _generic_tokenizer_key_offsets[] = {
	0, 0, 17, 29, 43, 46, 51, 54, 
	89, 94, 98, 101, 105, 110, 111, 116, 
	117, 122, 136, 142, 147, 150, 162
};

static const short _generic_tokenizer_trans_keys[] = {
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
	46u, 69u, 101u, 159u, 48u, 57u, 69u, 101u, 
	159u, 48u, 57u, 159u, 48u, 57u, 129u, 131u, 
	135u, 151u, 155u, 157u, 65u, 90u, 97u, 122u, 
	142u, 143u, 159u, 48u, 57u, 0
};

static const char _generic_tokenizer_single_lengths[] = {
	0, 13, 10, 12, 1, 3, 1, 21, 
	5, 4, 3, 4, 5, 1, 5, 1, 
	5, 12, 4, 3, 1, 6, 1
};

static const char _generic_tokenizer_range_lengths[] = {
	0, 2, 1, 1, 1, 1, 1, 7, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 1, 1, 1, 3, 1
};

static const unsigned char _generic_tokenizer_index_offsets[] = {
	0, 0, 16, 28, 42, 45, 50, 53, 
	82, 88, 93, 97, 102, 108, 110, 116, 
	118, 124, 138, 144, 149, 152, 162
};

static const char _generic_tokenizer_indicies[] = {
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
	4, 27, 28, 29, 29, 15, 15, 27, 
	29, 29, 6, 6, 27, 8, 8, 27, 
	16, 16, 16, 16, 16, 16, 16, 16, 
	16, 27, 15, 15, 27, 0
};

static const char _generic_tokenizer_trans_targs[] = {
	7, 1, 2, 7, 1, 3, 19, 6, 
	20, 7, 8, 12, 16, 17, 0, 18, 
	21, 22, 7, 9, 11, 10, 13, 14, 
	7, 7, 15, 7, 4, 5
};

static const char _generic_tokenizer_trans_actions[] = {
	1, 0, 0, 2, 3, 0, 4, 0, 
	0, 7, 0, 0, 0, 4, 0, 4, 
	0, 0, 8, 0, 0, 0, 0, 0, 
	9, 10, 0, 11, 0, 0
};

static const char _generic_tokenizer_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 5, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const char _generic_tokenizer_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 6, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const unsigned char _generic_tokenizer_eof_trans[] = {
	0, 1, 1, 1, 1, 1, 1, 0, 
	19, 19, 19, 19, 19, 25, 19, 25, 
	19, 28, 28, 28, 28, 28, 28
};

static const int generic_tokenizer_start = 7;





generic_tokenizer::generic_tokenizer(unsigned version) : ragel_tokenizer(version <= 1 ? 1 : 2) {}

bool generic_tokenizer::next_sentence(vector<token_range>& tokens) {
  using namespace unilib;

  int cs, act;
  size_t ts, te;
  size_t whitespace = 0; // Suppress "may be uninitialized" warning

  while (tokenize_url_email(tokens))
    if (emergency_sentence_split(tokens))
      return true;
  
	{
	cs = generic_tokenizer_start;
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
	switch ( _generic_tokenizer_from_state_actions[cs] ) {
	case 6:
	{ts = ( current);}
	break;
	}

	_widec = ( ragel_char(chars[current]));
	_klen = _generic_tokenizer_cond_lengths[cs];
	_keys = _generic_tokenizer_cond_keys + (_generic_tokenizer_cond_offsets[cs]*2);
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
				switch ( _generic_tokenizer_cond_spaces[_generic_tokenizer_cond_offsets[cs] + ((_mid - _keys)>>1)] ) {
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

	_keys = _generic_tokenizer_trans_keys + _generic_tokenizer_key_offsets[cs];
	_trans = _generic_tokenizer_index_offsets[cs];

	_klen = _generic_tokenizer_single_lengths[cs];
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

	_klen = _generic_tokenizer_range_lengths[cs];
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
	_trans = _generic_tokenizer_indicies[_trans];
_eof_trans:
	cs = _generic_tokenizer_trans_targs[_trans];

	if ( _generic_tokenizer_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _generic_tokenizer_trans_actions[_trans] ) {
	case 3:
	{ whitespace = current; }
	break;
	case 4:
	{te = ( current)+1;}
	break;
	case 7:
	{te = ( current)+1;{ tokens.emplace_back(ts, te - ts);
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	case 2:
	{te = ( current)+1;{
          bool eos = is_eos(tokens, chars[ts].chr, nullptr);
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
          current = te;
          do
            if (emergency_sentence_split(tokens)) { ( current)--; {( current)++; goto _out; } }
          while (tokenize_url_email(tokens));
          ( current)--;
        }}
	break;
	}

_again:
	switch ( _generic_tokenizer_to_state_actions[cs] ) {
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
	if ( _generic_tokenizer_eof_trans[cs] > 0 ) {
		_trans = _generic_tokenizer_eof_trans[cs] - 1;
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
