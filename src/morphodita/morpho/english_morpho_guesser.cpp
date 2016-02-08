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


static const char _tag_guesser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 2, 2, 6, 2, 2, 7, 2, 
	4, 6, 2, 4, 7, 2, 5, 6, 
	2, 5, 7, 2, 6, 7, 3, 2, 
	6, 7, 3, 4, 6, 7, 3, 5, 
	6, 7
};

static const unsigned char _tag_guesser_key_offsets[] = {
	0, 19, 26, 34, 42, 50, 58, 66, 
	74, 82, 90, 100, 108, 116, 124, 132, 
	145, 153, 161, 168, 179, 195, 212, 220, 
	228, 236
};

static const char _tag_guesser_trans_keys[] = {
	45, 46, 99, 100, 103, 105, 109, 110, 
	114, 115, 116, 118, 120, 48, 57, 65, 
	90, 97, 122, 45, 48, 57, 65, 90, 
	97, 122, 45, 114, 48, 57, 65, 90, 
	97, 122, 45, 111, 48, 57, 65, 90, 
	97, 122, 45, 109, 48, 57, 65, 90, 
	97, 122, 45, 101, 48, 57, 65, 90, 
	97, 122, 45, 115, 48, 57, 65, 90, 
	97, 122, 45, 101, 48, 57, 65, 90, 
	97, 122, 45, 108, 48, 57, 65, 90, 
	97, 122, 45, 115, 48, 57, 65, 90, 
	97, 122, 45, 97, 101, 111, 48, 57, 
	65, 90, 98, 122, 45, 101, 48, 57, 
	65, 90, 97, 122, 45, 108, 48, 57, 
	65, 90, 97, 122, 45, 109, 48, 57, 
	65, 90, 97, 122, 45, 105, 48, 57, 
	65, 90, 97, 122, 45, 97, 101, 105, 
	111, 117, 121, 48, 57, 65, 90, 98, 
	122, 45, 115, 48, 57, 65, 90, 97, 
	122, 45, 101, 48, 57, 65, 90, 97, 
	122, 45, 48, 57, 65, 90, 97, 122, 
	45, 101, 114, 115, 116, 48, 57, 65, 
	90, 97, 122, 45, 46, 105, 109, 118, 
	120, 48, 57, 65, 90, 97, 98, 99, 
	100, 101, 122, 45, 46, 101, 105, 109, 
	118, 120, 48, 57, 65, 90, 97, 98, 
	99, 100, 102, 122, 45, 110, 48, 57, 
	65, 90, 97, 122, 45, 105, 48, 57, 
	65, 90, 97, 122, 45, 101, 48, 57, 
	65, 90, 97, 122, 45, 115, 48, 57, 
	65, 90, 97, 122, 0
};

static const char _tag_guesser_single_lengths[] = {
	13, 1, 2, 2, 2, 2, 2, 2, 
	2, 2, 4, 2, 2, 2, 2, 7, 
	2, 2, 1, 5, 6, 7, 2, 2, 
	2, 2
};

static const char _tag_guesser_range_lengths[] = {
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 5, 5, 3, 3, 
	3, 3
};

static const unsigned char _tag_guesser_index_offsets[] = {
	0, 17, 22, 28, 34, 40, 46, 52, 
	58, 64, 70, 78, 84, 90, 96, 102, 
	113, 119, 125, 130, 139, 151, 164, 170, 
	176, 182
};

static const char _tag_guesser_indicies[] = {
	1, 2, 5, 6, 7, 5, 5, 8, 
	9, 10, 11, 5, 5, 3, 4, 4, 
	0, 13, 14, 15, 15, 12, 13, 16, 
	14, 15, 15, 12, 13, 17, 14, 15, 
	15, 12, 13, 18, 14, 15, 15, 12, 
	13, 18, 14, 15, 15, 12, 13, 19, 
	14, 15, 15, 12, 13, 20, 14, 15, 
	15, 12, 13, 18, 14, 15, 15, 12, 
	13, 21, 14, 15, 15, 12, 13, 22, 
	23, 24, 14, 15, 15, 12, 13, 25, 
	14, 15, 15, 12, 13, 23, 14, 15, 
	15, 12, 13, 23, 14, 15, 15, 12, 
	13, 26, 14, 15, 15, 12, 28, 15, 
	15, 15, 15, 15, 15, 29, 26, 26, 
	27, 31, 4, 32, 33, 33, 30, 13, 
	23, 14, 15, 15, 12, 13, 14, 15, 
	15, 12, 13, 34, 35, 36, 37, 14, 
	15, 15, 12, 13, 38, 39, 39, 39, 
	39, 14, 15, 15, 39, 15, 12, 13, 
	38, 40, 39, 39, 39, 39, 14, 15, 
	15, 39, 15, 12, 13, 41, 14, 15, 
	15, 12, 13, 42, 14, 15, 15, 12, 
	13, 18, 14, 15, 15, 12, 13, 43, 
	14, 15, 15, 12, 0
};

static const char _tag_guesser_trans_targs[] = {
	18, 19, 20, 18, 18, 20, 21, 22, 
	23, 24, 16, 25, 18, 19, 18, 1, 
	3, 4, 18, 7, 8, 10, 11, 18, 
	13, 12, 18, 18, 19, 18, 18, 19, 
	18, 18, 2, 5, 6, 9, 20, 20, 
	18, 14, 15, 17
};

static const char _tag_guesser_trans_actions[] = {
	29, 46, 29, 32, 11, 11, 11, 11, 
	11, 11, 0, 11, 13, 35, 15, 0, 
	0, 0, 1, 0, 0, 0, 0, 3, 
	0, 0, 5, 17, 38, 20, 23, 42, 
	26, 9, 0, 0, 0, 0, 13, 0, 
	7, 0, 0, 0
};

static const char _tag_guesser_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 15, 15, 0, 0, 
	0, 0
};

static const int tag_guesser_start = 0;



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
    
	{
	cs = tag_guesser_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form_lc.str + form_lc.len)) )
		goto _test_eof;
_resume:
	_keys = _tag_guesser_trans_keys + _tag_guesser_key_offsets[cs];
	_trans = _tag_guesser_index_offsets[cs];

	_klen = _tag_guesser_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form_lc.str[form_lc.len - 1 - (p - form_lc.str)]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form_lc.str[form_lc.len - 1 - (p - form_lc.str)]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _tag_guesser_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form_lc.str[form_lc.len - 1 - (p - form_lc.str)]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form_lc.str[form_lc.len - 1 - (p - form_lc.str)]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _tag_guesser_indicies[_trans];
	cs = _tag_guesser_trans_targs[_trans];

	if ( _tag_guesser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _tag_guesser_actions + _tag_guesser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (!added_JJR_RBR) added_JJR_RBR = true, add_JJR_RBR(lemma_lc, negation_len, lemmas); }
	break;
	case 1:
	{ if (!added_JJS_RBS) added_JJS_RBS = true, add_JJS_RBS(lemma_lc, negation_len, lemmas); }
	break;
	case 2:
	{ add_VBG(lemma_lc, lemmas); }
	break;
	case 3:
	{ add_VBD_VBN(lemma_lc, lemmas); }
	break;
	case 4:
	{ add_VBZ(lemma_lc, lemmas); }
	break;
	case 5:
	{ add(VB, lemma_lc, lemmas); add(VBP, lemma_lc, lemmas); }
	break;
	case 6:
	{ if (!added_SYM) added_SYM = true, add(SYM, lemma_lc, lemmas); }
	break;
	case 7:
	{ if (!added_CD) added_CD = true, add(CD, lemma_lc, lemmas); }
	break;
		}
	}

_again:
	if ( ++p != ( (form_lc.str + form_lc.len)) )
		goto _resume;
	_test_eof: {}
	if ( p == ( (form_lc.str + form_lc.len)) )
	{
	const char *__acts = _tag_guesser_actions + _tag_guesser_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 7:
	{ if (!added_CD) added_CD = true, add(CD, lemma_lc, lemmas); }
	break;
		}
	}
	}

	}


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














static const char _NNS_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13
};

static const char _NNS_key_offsets[] = {
	0, 0, 2, 3, 4, 5, 7, 17, 
	17, 29, 30, 35, 35, 36, 37, 37, 
	37, 44, 45, 53, 63, 72
};

static const char _NNS_trans_keys[] = {
	110, 115, 101, 109, 101, 99, 115, 98, 
	100, 102, 104, 106, 110, 112, 116, 118, 
	122, 104, 122, 98, 100, 102, 103, 106, 
	110, 112, 116, 118, 120, 111, 97, 101, 
	105, 111, 117, 105, 119, 104, 105, 111, 
	115, 118, 120, 122, 115, 97, 101, 105, 
	110, 111, 114, 115, 117, 98, 100, 102, 
	104, 106, 110, 112, 116, 118, 122, 97, 
	101, 105, 111, 117, 121, 122, 98, 120, 
	0
};

static const char _NNS_single_lengths[] = {
	0, 2, 1, 1, 1, 2, 0, 0, 
	2, 1, 5, 0, 1, 1, 0, 0, 
	7, 1, 8, 0, 7, 0
};

static const char _NNS_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 5, 0, 
	5, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 5, 1, 0
};

static const char _NNS_index_offsets[] = {
	0, 0, 3, 5, 7, 9, 12, 18, 
	19, 27, 29, 35, 36, 38, 40, 41, 
	42, 50, 52, 61, 67, 76
};

static const char _NNS_indicies[] = {
	0, 2, 1, 3, 1, 4, 1, 6, 
	5, 7, 7, 1, 8, 8, 8, 8, 
	8, 1, 9, 11, 10, 10, 10, 10, 
	10, 10, 1, 12, 1, 13, 13, 13, 
	13, 13, 1, 14, 15, 1, 16, 1, 
	17, 1, 18, 19, 20, 21, 22, 7, 
	23, 1, 24, 1, 25, 25, 25, 26, 
	25, 27, 28, 29, 1, 30, 30, 30, 
	30, 30, 1, 31, 31, 31, 31, 31, 
	31, 33, 32, 1, 17, 0
};

static const char _NNS_trans_targs[] = {
	2, 0, 4, 3, 15, 15, 16, 15, 
	7, 15, 15, 17, 15, 11, 15, 13, 
	15, 15, 5, 6, 8, 18, 12, 20, 
	15, 15, 9, 10, 15, 19, 15, 15, 
	14, 21
};

static const char _NNS_trans_actions[] = {
	0, 0, 0, 0, 1, 27, 27, 21, 
	0, 23, 25, 25, 19, 0, 17, 0, 
	5, 11, 0, 0, 0, 21, 0, 21, 
	3, 9, 0, 0, 15, 9, 7, 13, 
	0, 15
};

static const int NNS_start = 1;



void english_morpho_guesser::add_NNS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  
	{
	cs = NNS_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form.c_str() + form.size())) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _NNS_trans_keys + _NNS_key_offsets[cs];
	_trans = _NNS_index_offsets[cs];

	_klen = _NNS_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _NNS_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _NNS_indicies[_trans];
	cs = _NNS_trans_targs[_trans];

	if ( _NNS_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _NNS_actions + _NNS_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (best > 'a') best = 'a', remove = 2, append = "an";    }
	break;
	case 1:
	{ if (best > 'b') best = 'b', remove = 1, append = nullptr; }
	break;
	case 2:
	{ if (best > 'c') best = 'c', remove = 3, append = "fe";    }
	break;
	case 3:
	{ if (best > 'd') best = 'd', remove = 2, append = nullptr; }
	break;
	case 4:
	{ if (best > 'e') best = 'e', remove = 1, append = nullptr; }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 2, append = nullptr; }
	break;
	case 6:
	{ if (best > 'g') best = 'g', remove = 1, append = nullptr; }
	break;
	case 7:
	{ if (best > 'h') best = 'h', remove = 2, append = nullptr; }
	break;
	case 8:
	{ if (best > 'i') best = 'i', remove = 1, append = nullptr; }
	break;
	case 9:
	{ if (best > 'j') best = 'j', remove = 1, append = nullptr; }
	break;
	case 10:
	{ if (best > 'k') best = 'k', remove = 2, append = nullptr; }
	break;
	case 11:
	{ if (best > 'l') best = 'l', remove = 3, append = "y";     }
	break;
	case 12:
	{ if (best > 'm') best = 'm', remove = 2, append = nullptr; }
	break;
	case 13:
	{ if (best > 'n') best = 'n', remove = 1, append = nullptr; }
	break;
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != ( (form.c_str() + form.size())) )
		goto _resume;
	_test_eof: {}
	_out: {}
	}


  add(NNS, form.substr(0, form.size() - remove).append(append ? append : ""), negation_len, lemmas);
}


static const char _NNPS_actions[] = {
	0, 1, 1, 1, 2, 1, 4, 1, 
	5, 1, 6, 1, 7, 1, 8, 1, 
	9, 1, 10, 1, 11, 1, 12, 1, 
	14, 1, 15, 1, 16, 2, 0, 1, 
	2, 3, 4, 2, 13, 14
};

static const unsigned char _NNPS_key_offsets[] = {
	0, 0, 4, 6, 8, 10, 12, 16, 
	36, 36, 60, 62, 72, 72, 74, 76, 
	78, 78, 98, 98, 100, 102, 104, 104, 
	118, 120, 136, 156, 174, 174
};

static const char _NNPS_trans_keys[] = {
	78, 83, 110, 115, 69, 101, 77, 109, 
	77, 109, 69, 101, 67, 83, 99, 115, 
	66, 68, 70, 72, 74, 78, 80, 84, 
	86, 90, 98, 100, 102, 104, 106, 110, 
	112, 116, 118, 122, 72, 90, 104, 122, 
	66, 68, 70, 71, 74, 78, 80, 84, 
	86, 88, 98, 100, 102, 103, 106, 110, 
	112, 116, 118, 120, 79, 111, 65, 69, 
	73, 79, 85, 97, 101, 105, 111, 117, 
	73, 105, 87, 119, 87, 119, 66, 68, 
	70, 72, 74, 78, 80, 84, 86, 90, 
	98, 100, 102, 104, 106, 110, 112, 116, 
	118, 122, 73, 105, 69, 101, 69, 101, 
	72, 73, 79, 83, 86, 88, 90, 104, 
	105, 111, 115, 118, 120, 122, 83, 115, 
	65, 69, 73, 78, 79, 82, 83, 85, 
	97, 101, 105, 110, 111, 114, 115, 117, 
	66, 68, 70, 72, 74, 78, 80, 84, 
	86, 90, 98, 100, 102, 104, 106, 110, 
	112, 116, 118, 122, 65, 69, 73, 79, 
	85, 89, 90, 97, 101, 105, 111, 117, 
	121, 122, 66, 88, 98, 120, 72, 73, 
	79, 83, 86, 88, 90, 104, 105, 111, 
	115, 118, 120, 122, 0
};

static const char _NNPS_single_lengths[] = {
	0, 4, 2, 2, 2, 2, 4, 0, 
	0, 4, 2, 10, 0, 2, 2, 2, 
	0, 0, 0, 2, 2, 2, 0, 14, 
	2, 16, 0, 14, 0, 14
};

static const char _NNPS_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 10, 
	0, 10, 0, 0, 0, 0, 0, 0, 
	0, 10, 0, 0, 0, 0, 0, 0, 
	0, 0, 10, 2, 0, 0
};

static const unsigned char _NNPS_index_offsets[] = {
	0, 0, 5, 8, 11, 14, 17, 22, 
	33, 34, 49, 52, 63, 64, 67, 70, 
	73, 74, 85, 86, 89, 92, 95, 96, 
	111, 114, 131, 142, 159, 160
};

static const char _NNPS_indicies[] = {
	0, 2, 3, 4, 1, 5, 6, 1, 
	7, 8, 1, 8, 8, 1, 10, 11, 
	9, 12, 12, 12, 12, 1, 13, 13, 
	13, 13, 13, 13, 13, 13, 13, 13, 
	1, 14, 16, 15, 16, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	1, 17, 17, 1, 18, 18, 18, 18, 
	18, 18, 18, 18, 18, 18, 1, 19, 
	20, 21, 1, 22, 23, 1, 23, 23, 
	1, 24, 25, 25, 25, 25, 25, 25, 
	25, 25, 25, 25, 1, 26, 21, 21, 
	1, 6, 6, 1, 11, 11, 9, 1, 
	27, 28, 29, 30, 31, 12, 32, 27, 
	33, 29, 30, 34, 12, 32, 1, 35, 
	35, 1, 36, 36, 36, 37, 36, 38, 
	39, 40, 36, 36, 36, 37, 36, 38, 
	39, 40, 1, 41, 41, 41, 41, 41, 
	41, 41, 41, 41, 41, 1, 42, 42, 
	42, 42, 42, 42, 44, 42, 42, 42, 
	42, 42, 42, 44, 43, 43, 1, 24, 
	27, 33, 29, 30, 34, 12, 32, 27, 
	33, 29, 30, 34, 12, 32, 1, 0
};

static const char _NNPS_trans_targs[] = {
	2, 0, 5, 20, 21, 3, 4, 22, 
	22, 22, 23, 29, 22, 8, 22, 22, 
	24, 22, 12, 22, 14, 15, 22, 22, 
	22, 18, 22, 6, 7, 9, 25, 13, 
	27, 17, 19, 22, 22, 10, 11, 22, 
	26, 22, 22, 16, 28
};

static const char _NNPS_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 29, 
	1, 27, 27, 27, 21, 0, 35, 25, 
	25, 19, 0, 17, 0, 0, 32, 5, 
	11, 0, 23, 0, 0, 0, 21, 0, 
	21, 0, 0, 3, 9, 0, 0, 15, 
	9, 7, 13, 0, 15
};

static const int NNPS_start = 1;



void english_morpho_guesser::add_NNPS(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  
	{
	cs = NNPS_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form.c_str() + form.size())) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _NNPS_trans_keys + _NNPS_key_offsets[cs];
	_trans = _NNPS_index_offsets[cs];

	_klen = _NNPS_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _NNPS_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _NNPS_indicies[_trans];
	cs = _NNPS_trans_targs[_trans];

	if ( _NNPS_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _NNPS_actions + _NNPS_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (best > 'a') best = 'a', remove = 2, append = "AN";    }
	break;
	case 1:
	{ if (best > 'b') best = 'b', remove = 2, append = "an";    }
	break;
	case 2:
	{ if (best > 'c') best = 'c', remove = 1, append = nullptr; }
	break;
	case 3:
	{ if (best > 'd') best = 'd', remove = 3, append = "FE";    }
	break;
	case 4:
	{ if (best > 'e') best = 'e', remove = 3, append = "fe";    }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 2, append = nullptr; }
	break;
	case 6:
	{ if (best > 'g') best = 'g', remove = 1, append = nullptr; }
	break;
	case 7:
	{ if (best > 'h') best = 'h', remove = 2, append = nullptr; }
	break;
	case 8:
	{ if (best > 'i') best = 'i', remove = 1, append = nullptr; }
	break;
	case 9:
	{ if (best > 'j') best = 'j', remove = 2, append = nullptr; }
	break;
	case 10:
	{ if (best > 'k') best = 'k', remove = 1, append = nullptr; }
	break;
	case 11:
	{ if (best > 'l') best = 'l', remove = 1, append = nullptr; }
	break;
	case 12:
	{ if (best > 'm') best = 'm', remove = 2, append = nullptr; }
	break;
	case 13:
	{ if (best > 'n') best = 'n', remove = 3, append = "Y";     }
	break;
	case 14:
	{ if (best > 'o') best = 'o', remove = 3, append = "y";     }
	break;
	case 15:
	{ if (best > 'p') best = 'p', remove = 2, append = nullptr; }
	break;
	case 16:
	{ if (best > 'q') best = 'q', remove = 1, append = nullptr; }
	break;
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != ( (form.c_str() + form.size())) )
		goto _resume;
	_test_eof: {}
	_out: {}
	}


  add(NNPS, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}


static const char _VBG_actions[] = {
	0, 1, 1, 1, 2, 1, 4, 1, 
	5, 1, 6, 1, 7, 1, 9, 1, 
	10, 1, 11, 1, 12, 1, 13, 1, 
	14, 1, 15, 1, 16, 1, 17, 2, 
	0, 12, 2, 3, 4, 2, 5, 9, 
	2, 5, 10, 2, 8, 9, 2, 9, 
	10, 2, 11, 12, 3, 0, 2, 12, 
	3, 2, 11, 12
};

static const short _VBG_key_offsets[] = {
	0, 0, 1, 2, 3, 9, 14, 24, 
	29, 34, 44, 46, 47, 48, 49, 50, 
	51, 52, 59, 66, 68, 70, 71, 72, 
	73, 74, 75, 76, 81, 89, 90, 91, 
	92, 93, 94, 96, 97, 98, 99, 100, 
	101, 102, 127, 127, 136, 137, 142, 153, 
	162, 171, 181, 186, 191, 197, 207, 207, 
	216, 228, 229, 240, 240, 249, 258, 267, 
	276, 285, 290, 302, 313, 318, 324, 334, 
	344, 355, 362, 373, 382, 391, 391, 402, 
	413, 415, 416, 417, 417, 418, 426, 437, 
	442, 448, 458, 468, 479, 486, 497, 504, 
	510, 519, 528, 537, 543
};

static const char _VBG_trans_keys[] = {
	103, 110, 105, 97, 101, 105, 111, 117, 
	121, 97, 101, 105, 111, 117, 98, 100, 
	102, 104, 106, 110, 112, 116, 118, 122, 
	97, 101, 105, 111, 117, 97, 101, 105, 
	111, 117, 98, 100, 102, 104, 106, 110, 
	112, 116, 118, 122, 98, 114, 105, 114, 
	112, 105, 109, 101, 97, 101, 105, 111, 
	117, 98, 122, 97, 101, 105, 111, 117, 
	98, 122, 97, 122, 98, 114, 105, 114, 
	112, 105, 109, 101, 97, 101, 105, 111, 
	117, 97, 101, 105, 110, 111, 115, 117, 
	120, 105, 112, 105, 109, 101, 98, 114, 
	105, 114, 112, 105, 109, 101, 98, 99, 
	100, 102, 103, 104, 106, 107, 108, 109, 
	110, 111, 112, 113, 114, 115, 116, 117, 
	118, 119, 120, 121, 122, 97, 105, 97, 
	98, 101, 105, 111, 117, 122, 99, 120, 
	113, 97, 101, 105, 111, 117, 98, 99, 
	100, 105, 111, 117, 122, 97, 101, 102, 
	120, 97, 100, 101, 105, 111, 117, 122, 
	98, 120, 97, 101, 102, 105, 111, 117, 
	122, 98, 120, 97, 101, 103, 105, 110, 
	111, 117, 122, 98, 120, 97, 101, 105, 
	111, 117, 101, 110, 111, 115, 120, 101, 
	110, 111, 112, 115, 120, 97, 101, 104, 
	105, 111, 116, 117, 122, 98, 120, 97, 
	101, 105, 106, 111, 117, 122, 98, 120, 
	98, 99, 100, 105, 107, 111, 117, 122, 
	97, 101, 102, 120, 105, 97, 101, 105, 
	108, 111, 114, 117, 119, 122, 98, 120, 
	97, 101, 105, 109, 111, 117, 122, 98, 
	120, 97, 101, 105, 110, 111, 117, 122, 
	98, 120, 97, 101, 105, 111, 112, 117, 
	122, 98, 120, 97, 101, 105, 111, 113, 
	117, 122, 98, 120, 97, 101, 105, 111, 
	114, 117, 122, 98, 120, 97, 101, 105, 
	111, 117, 98, 99, 100, 105, 108, 111, 
	116, 117, 97, 101, 102, 122, 101, 110, 
	111, 115, 120, 98, 104, 106, 116, 118, 
	122, 101, 110, 111, 115, 120, 101, 110, 
	111, 112, 115, 120, 101, 105, 110, 111, 
	115, 120, 98, 116, 118, 122, 101, 105, 
	110, 111, 115, 120, 98, 116, 118, 122, 
	101, 110, 111, 115, 120, 98, 104, 106, 
	116, 118, 122, 98, 101, 110, 111, 114, 
	115, 120, 101, 110, 111, 115, 120, 98, 
	104, 106, 116, 118, 122, 97, 101, 105, 
	111, 115, 117, 122, 98, 120, 97, 101, 
	105, 111, 116, 117, 122, 98, 120, 122, 
	98, 100, 102, 104, 106, 110, 112, 116, 
	118, 120, 122, 98, 100, 102, 104, 106, 
	110, 112, 116, 118, 120, 98, 114, 112, 
	114, 113, 97, 101, 105, 108, 111, 117, 
	98, 122, 101, 110, 111, 115, 120, 98, 
	104, 106, 116, 118, 122, 101, 110, 111, 
	115, 120, 101, 110, 111, 112, 115, 120, 
	101, 105, 110, 111, 115, 120, 98, 116, 
	118, 122, 101, 105, 110, 111, 115, 120, 
	98, 116, 118, 122, 101, 110, 111, 115, 
	120, 98, 104, 106, 116, 118, 122, 98, 
	101, 110, 111, 114, 115, 120, 101, 110, 
	111, 115, 120, 98, 104, 106, 116, 118, 
	122, 97, 101, 105, 111, 117, 98, 122, 
	97, 101, 105, 111, 117, 121, 97, 101, 
	105, 111, 117, 118, 122, 98, 120, 97, 
	101, 105, 111, 117, 119, 122, 98, 120, 
	97, 101, 105, 111, 117, 120, 122, 98, 
	119, 97, 101, 105, 111, 117, 121, 97, 
	101, 105, 111, 117, 121, 122, 98, 120, 
	0
};

static const char _VBG_single_lengths[] = {
	0, 1, 1, 1, 6, 5, 0, 5, 
	5, 0, 2, 1, 1, 1, 1, 1, 
	1, 5, 5, 0, 2, 1, 1, 1, 
	1, 1, 1, 5, 8, 1, 1, 1, 
	1, 1, 2, 1, 1, 1, 1, 1, 
	1, 23, 0, 7, 1, 5, 7, 7, 
	7, 8, 5, 5, 6, 8, 0, 7, 
	8, 1, 9, 0, 7, 7, 7, 7, 
	7, 5, 8, 5, 5, 6, 6, 6, 
	5, 7, 5, 7, 7, 0, 1, 1, 
	2, 1, 1, 0, 1, 6, 5, 5, 
	6, 6, 6, 5, 7, 5, 5, 6, 
	7, 7, 7, 6, 7
};

static const char _VBG_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 5, 0, 
	0, 5, 0, 0, 0, 0, 0, 0, 
	0, 1, 1, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 1, 0, 0, 2, 1, 
	1, 1, 0, 0, 0, 1, 0, 1, 
	2, 0, 1, 0, 1, 1, 1, 1, 
	1, 0, 2, 3, 0, 0, 2, 2, 
	3, 0, 3, 1, 1, 0, 5, 5, 
	0, 0, 0, 0, 0, 1, 3, 0, 
	0, 2, 2, 3, 0, 3, 1, 0, 
	1, 1, 1, 0, 1
};

static const short _VBG_index_offsets[] = {
	0, 0, 2, 4, 6, 13, 19, 25, 
	31, 37, 43, 46, 48, 50, 52, 54, 
	56, 58, 65, 72, 74, 77, 79, 81, 
	83, 85, 87, 89, 95, 104, 106, 108, 
	110, 112, 114, 117, 119, 121, 123, 125, 
	127, 129, 154, 155, 164, 166, 172, 182, 
	191, 200, 210, 216, 222, 229, 239, 240, 
	249, 260, 262, 273, 274, 283, 292, 301, 
	310, 319, 325, 336, 345, 351, 358, 367, 
	376, 385, 393, 402, 411, 420, 421, 428, 
	435, 438, 440, 442, 443, 445, 453, 462, 
	468, 475, 484, 493, 502, 510, 519, 526, 
	533, 542, 551, 560, 567
};

static const unsigned char _VBG_indicies[] = {
	0, 1, 2, 1, 3, 1, 4, 4, 
	4, 4, 4, 4, 1, 5, 5, 5, 
	5, 6, 1, 7, 7, 7, 7, 7, 
	1, 8, 8, 8, 8, 9, 1, 5, 
	5, 5, 5, 10, 1, 11, 11, 11, 
	11, 11, 1, 11, 12, 1, 11, 1, 
	13, 1, 11, 1, 14, 1, 11, 1, 
	11, 1, 5, 5, 5, 5, 6, 15, 
	1, 5, 5, 5, 5, 6, 16, 1, 
	4, 1, 17, 18, 1, 17, 1, 19, 
	1, 17, 1, 20, 1, 17, 1, 17, 
	1, 21, 22, 21, 23, 24, 1, 25, 
	26, 25, 27, 28, 29, 25, 30, 1, 
	31, 1, 31, 1, 32, 1, 31, 1, 
	31, 1, 33, 34, 1, 33, 1, 35, 
	1, 33, 1, 36, 1, 33, 1, 33, 
	1, 38, 39, 40, 41, 42, 43, 44, 
	45, 46, 47, 48, 49, 50, 51, 52, 
	53, 54, 55, 56, 57, 58, 59, 60, 
	37, 1, 1, 61, 62, 61, 61, 61, 
	61, 63, 63, 1, 64, 1, 65, 65, 
	65, 65, 65, 1, 67, 68, 67, 66, 
	66, 66, 67, 66, 67, 1, 69, 62, 
	69, 69, 69, 69, 63, 63, 1, 61, 
	61, 62, 61, 61, 61, 63, 63, 1, 
	66, 66, 68, 66, 70, 66, 66, 67, 
	67, 1, 71, 71, 71, 71, 71, 1, 
	72, 73, 74, 75, 76, 1, 72, 73, 
	74, 11, 75, 76, 1, 61, 61, 62, 
	61, 61, 77, 61, 63, 63, 1, 78, 
	61, 61, 61, 62, 61, 61, 63, 63, 
	1, 63, 79, 63, 61, 62, 61, 61, 
	63, 61, 63, 1, 7, 1, 61, 61, 
	61, 68, 61, 80, 61, 80, 67, 67, 
	1, 5, 61, 61, 61, 62, 61, 61, 
	63, 63, 1, 81, 81, 82, 62, 81, 
	81, 63, 63, 1, 81, 81, 81, 81, 
	62, 81, 63, 63, 1, 61, 61, 61, 
	61, 62, 61, 63, 63, 1, 61, 83, 
	61, 84, 62, 61, 63, 63, 1, 5, 
	5, 5, 5, 6, 1, 85, 86, 85, 
	5, 86, 5, 86, 6, 5, 85, 1, 
	87, 88, 89, 90, 91, 85, 85, 85, 
	1, 87, 92, 89, 93, 94, 1, 87, 
	92, 89, 17, 93, 94, 1, 87, 17, 
	88, 89, 90, 91, 85, 85, 1, 87, 
	20, 88, 89, 90, 91, 85, 85, 1, 
	95, 88, 89, 90, 91, 85, 85, 85, 
	1, 17, 87, 92, 89, 18, 93, 94, 
	1, 87, 97, 89, 98, 99, 96, 96, 
	96, 1, 66, 66, 66, 66, 100, 66, 
	67, 67, 1, 101, 102, 103, 61, 62, 
	61, 63, 63, 1, 104, 106, 106, 106, 
	106, 106, 106, 105, 107, 107, 107, 107, 
	107, 107, 1, 31, 108, 1, 31, 1, 
	109, 1, 105, 110, 104, 5, 5, 5, 
	112, 5, 6, 111, 1, 113, 114, 115, 
	116, 117, 111, 111, 111, 1, 113, 118, 
	115, 119, 120, 1, 113, 118, 115, 33, 
	119, 120, 1, 113, 33, 114, 115, 116, 
	117, 111, 111, 1, 113, 36, 114, 115, 
	116, 117, 111, 111, 1, 121, 114, 115, 
	116, 117, 111, 111, 111, 1, 33, 113, 
	118, 115, 34, 119, 120, 1, 113, 123, 
	115, 124, 125, 122, 122, 122, 1, 5, 
	5, 5, 5, 6, 111, 1, 4, 4, 
	4, 4, 4, 4, 1, 66, 66, 66, 
	66, 66, 68, 67, 67, 1, 81, 81, 
	81, 81, 81, 62, 63, 63, 1, 81, 
	81, 81, 81, 81, 62, 63, 63, 1, 
	126, 126, 126, 126, 126, 4, 1, 127, 
	127, 127, 127, 127, 129, 130, 128, 1, 
	0
};

static const char _VBG_trans_targs[] = {
	2, 0, 3, 41, 42, 42, 44, 42, 
	42, 44, 44, 51, 52, 13, 15, 42, 
	42, 68, 69, 23, 25, 77, 78, 83, 
	84, 42, 80, 29, 82, 31, 33, 42, 
	32, 87, 88, 37, 39, 4, 43, 46, 
	47, 48, 49, 53, 55, 56, 58, 60, 
	61, 19, 62, 63, 64, 75, 76, 95, 
	96, 97, 98, 99, 100, 5, 45, 42, 
	42, 6, 7, 42, 45, 8, 50, 9, 
	10, 11, 12, 14, 16, 54, 42, 57, 
	59, 17, 18, 65, 66, 67, 74, 20, 
	70, 22, 71, 72, 21, 24, 26, 73, 
	67, 70, 71, 72, 45, 27, 85, 94, 
	42, 42, 79, 28, 81, 30, 42, 86, 
	93, 34, 89, 36, 90, 91, 35, 38, 
	40, 92, 86, 89, 90, 91, 65, 65, 
	42, 42, 45
};

static const char _VBG_trans_actions[] = {
	0, 0, 0, 29, 23, 15, 15, 3, 
	46, 46, 40, 0, 0, 0, 0, 5, 
	34, 0, 0, 0, 0, 15, 15, 15, 
	15, 11, 11, 0, 11, 0, 0, 9, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 21, 
	0, 0, 0, 23, 0, 0, 19, 19, 
	7, 0, 0, 49, 49, 0, 49, 0, 
	0, 0, 0, 0, 0, 19, 17, 19, 
	49, 0, 0, 27, 27, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	25, 25, 25, 25, 56, 0, 9, 9, 
	13, 43, 43, 0, 9, 0, 37, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 7, 7, 7, 7, 23, 1, 
	31, 1, 52
};

static const char _VBG_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 3, 0, 0, 3, 3, 
	3, 3, 0, 3, 3, 3, 0, 3, 
	3, 0, 3, 0, 3, 3, 3, 3, 
	3, 0, 0, 25, 25, 25, 25, 25, 
	25, 25, 25, 3, 3, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 7, 7, 
	7, 7, 7, 7, 7, 7, 0, 0, 
	3, 3, 3, 0, 3
};

static const int VBG_start = 1;



void english_morpho_guesser::add_VBG(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  
	{
	cs = VBG_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form.c_str() + form.size())) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _VBG_trans_keys + _VBG_key_offsets[cs];
	_trans = _VBG_index_offsets[cs];

	_klen = _VBG_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _VBG_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _VBG_indicies[_trans];
	cs = _VBG_trans_targs[_trans];

	if ( _VBG_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _VBG_actions + _VBG_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (best > 'a') best = 'a', remove = 3, append = nullptr; }
	break;
	case 1:
	{ if (best > 'b') best = 'b', remove = 3, append = "e";     }
	break;
	case 2:
	{ if (best > 'c') best = 'c', remove = 3, append = nullptr; }
	break;
	case 3:
	{ if (best > 'd') best = 'd', remove = 3, append = "e";     }
	break;
	case 4:
	{ if (best > 'e') best = 'e', remove = 3, append = nullptr; }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 3, append = "e";     }
	break;
	case 6:
	{ if (best > 'g') best = 'g', remove = 3, append = nullptr; }
	break;
	case 7:
	{ if (best > 'h') best = 'h', remove = 3, append = "e";     }
	break;
	case 8:
	{ if (best > 'i') best = 'i', remove = 3, append = nullptr; }
	break;
	case 9:
	{ if (best > 'j') best = 'j', remove = 3, append = "e";     }
	break;
	case 10:
	{ if (best > 'k') best = 'k', remove = 3, append = nullptr; }
	break;
	case 11:
	{ if (best > 'l') best = 'l', remove = 3, append = "e";     }
	break;
	case 12:
	{ if (best > 'm') best = 'm', remove = 3, append = nullptr; }
	break;
	case 13:
	{ if (best > 'n') best = 'n', remove = 3, append = "e";     }
	break;
	case 14:
	{ if (best > 'o') best = 'o', remove = 3, append = nullptr; }
	break;
	case 15:
	{ if (best > 'p') best = 'p', remove = 3, append = "e";     }
	break;
	case 16:
	{ if (best > 'q') best = 'q', remove = 3, append = nullptr; }
	break;
	case 17:
	{ if (best > 'r') best = 'r', remove = 3, append = "e";     }
	break;
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != ( (form.c_str() + form.size())) )
		goto _resume;
	_test_eof: {}
	if ( p == ( (form.c_str() + form.size())) )
	{
	const char *__acts = _VBG_actions + _VBG_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 2:
	{ if (best > 'c') best = 'c', remove = 3, append = nullptr; }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 3, append = "e";     }
	break;
	case 15:
	{ if (best > 'p') best = 'p', remove = 3, append = "e";     }
	break;
		}
	}
	}

	_out: {}
	}


  add(VBG, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}


static const char _VBD_VBN_actions[] = {
	0, 1, 0, 1, 2, 1, 3, 1, 
	4, 1, 5, 1, 6, 1, 7, 1, 
	8, 1, 9, 1, 10, 1, 11, 1, 
	13, 1, 14, 1, 15, 1, 16, 1, 
	17, 2, 1, 16, 2, 4, 5, 2, 
	8, 16, 2, 9, 13, 2, 9, 14, 
	2, 12, 13, 2, 13, 14, 2, 15, 
	16, 3, 1, 3, 16, 3, 3, 15, 
	16
};

static const short _VBD_VBN_key_offsets[] = {
	0, 0, 2, 3, 9, 14, 24, 29, 
	34, 44, 46, 47, 48, 49, 50, 51, 
	52, 60, 67, 74, 76, 77, 78, 79, 
	80, 81, 82, 87, 95, 96, 97, 98, 
	99, 100, 102, 103, 104, 105, 106, 107, 
	108, 114, 115, 140, 140, 149, 150, 155, 
	166, 175, 184, 194, 199, 204, 210, 220, 
	220, 229, 241, 242, 253, 253, 262, 271, 
	280, 289, 298, 303, 316, 327, 332, 338, 
	348, 358, 369, 376, 387, 396, 405, 405, 
	416, 427, 429, 430, 431, 431, 432, 440, 
	451, 456, 462, 472, 482, 493, 500, 511, 
	518, 524, 533, 542, 551
};

static const char _VBD_VBN_trans_keys[] = {
	100, 110, 101, 97, 101, 105, 111, 117, 
	121, 97, 101, 105, 111, 117, 98, 100, 
	102, 104, 106, 110, 112, 116, 118, 122, 
	97, 101, 105, 111, 117, 97, 101, 105, 
	111, 117, 98, 100, 102, 104, 106, 110, 
	112, 116, 118, 122, 98, 114, 105, 114, 
	112, 105, 109, 101, 97, 101, 105, 111, 
	117, 121, 98, 122, 97, 101, 105, 111, 
	117, 98, 122, 97, 101, 105, 111, 117, 
	98, 122, 98, 114, 105, 114, 112, 105, 
	109, 101, 97, 101, 105, 111, 117, 97, 
	101, 105, 110, 111, 115, 117, 120, 105, 
	112, 105, 109, 101, 98, 114, 105, 114, 
	112, 105, 109, 101, 97, 101, 105, 111, 
	117, 121, 101, 98, 99, 100, 102, 103, 
	104, 105, 106, 107, 108, 109, 110, 112, 
	113, 114, 115, 116, 117, 118, 119, 120, 
	121, 122, 97, 111, 97, 98, 101, 105, 
	111, 117, 122, 99, 120, 113, 97, 101, 
	105, 111, 117, 98, 99, 100, 105, 111, 
	117, 122, 97, 101, 102, 120, 97, 100, 
	101, 105, 111, 117, 122, 98, 120, 97, 
	101, 102, 105, 111, 117, 122, 98, 120, 
	97, 101, 103, 105, 110, 111, 117, 122, 
	98, 120, 97, 101, 105, 111, 117, 101, 
	110, 111, 115, 120, 101, 110, 111, 112, 
	115, 120, 97, 101, 104, 105, 111, 116, 
	117, 122, 98, 120, 97, 101, 105, 106, 
	111, 117, 122, 98, 120, 98, 99, 100, 
	105, 107, 111, 117, 122, 97, 101, 102, 
	120, 105, 97, 101, 105, 108, 111, 114, 
	117, 119, 122, 98, 120, 97, 101, 105, 
	109, 111, 117, 122, 98, 120, 97, 101, 
	105, 110, 111, 117, 122, 98, 120, 97, 
	101, 105, 111, 112, 117, 122, 98, 120, 
	97, 101, 105, 111, 113, 117, 122, 98, 
	120, 97, 101, 105, 111, 114, 117, 122, 
	98, 120, 97, 101, 105, 111, 117, 98, 
	99, 100, 105, 108, 110, 111, 116, 117, 
	97, 101, 102, 122, 101, 110, 111, 115, 
	120, 98, 104, 106, 116, 118, 122, 101, 
	110, 111, 115, 120, 101, 110, 111, 112, 
	115, 120, 101, 105, 110, 111, 115, 120, 
	98, 116, 118, 122, 101, 105, 110, 111, 
	115, 120, 98, 116, 118, 122, 101, 110, 
	111, 115, 120, 98, 104, 106, 116, 118, 
	122, 98, 101, 110, 111, 114, 115, 120, 
	101, 110, 111, 115, 120, 98, 104, 106, 
	116, 118, 122, 97, 101, 105, 111, 115, 
	117, 122, 98, 120, 97, 101, 105, 111, 
	116, 117, 122, 98, 120, 122, 98, 100, 
	102, 104, 106, 110, 112, 116, 118, 120, 
	122, 98, 100, 102, 104, 106, 110, 112, 
	116, 118, 120, 98, 114, 112, 114, 113, 
	97, 101, 105, 108, 111, 117, 98, 122, 
	101, 110, 111, 115, 120, 98, 104, 106, 
	116, 118, 122, 101, 110, 111, 115, 120, 
	101, 110, 111, 112, 115, 120, 101, 105, 
	110, 111, 115, 120, 98, 116, 118, 122, 
	101, 105, 110, 111, 115, 120, 98, 116, 
	118, 122, 101, 110, 111, 115, 120, 98, 
	104, 106, 116, 118, 122, 98, 101, 110, 
	111, 114, 115, 120, 101, 110, 111, 115, 
	120, 98, 104, 106, 116, 118, 122, 97, 
	101, 105, 111, 117, 98, 122, 97, 101, 
	105, 111, 117, 121, 97, 101, 105, 111, 
	117, 118, 122, 98, 120, 97, 101, 105, 
	111, 117, 119, 122, 98, 120, 97, 101, 
	105, 111, 117, 120, 122, 98, 119, 97, 
	101, 105, 111, 117, 121, 122, 98, 120, 
	0
};

static const char _VBD_VBN_single_lengths[] = {
	0, 2, 1, 6, 5, 0, 5, 5, 
	0, 2, 1, 1, 1, 1, 1, 1, 
	6, 5, 5, 2, 1, 1, 1, 1, 
	1, 1, 5, 8, 1, 1, 1, 1, 
	1, 2, 1, 1, 1, 1, 1, 1, 
	6, 1, 23, 0, 7, 1, 5, 7, 
	7, 7, 8, 5, 5, 6, 8, 0, 
	7, 8, 1, 9, 0, 7, 7, 7, 
	7, 7, 5, 9, 5, 5, 6, 6, 
	6, 5, 7, 5, 7, 7, 0, 1, 
	1, 2, 1, 1, 0, 1, 6, 5, 
	5, 6, 6, 6, 5, 7, 5, 5, 
	6, 7, 7, 7, 7
};

static const char _VBD_VBN_range_lengths[] = {
	0, 0, 0, 0, 0, 5, 0, 0, 
	5, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 0, 1, 0, 0, 2, 
	1, 1, 1, 0, 0, 0, 1, 0, 
	1, 2, 0, 1, 0, 1, 1, 1, 
	1, 1, 0, 2, 3, 0, 0, 2, 
	2, 3, 0, 3, 1, 1, 0, 5, 
	5, 0, 0, 0, 0, 0, 1, 3, 
	0, 0, 2, 2, 3, 0, 3, 1, 
	0, 1, 1, 1, 1
};

static const short _VBD_VBN_index_offsets[] = {
	0, 0, 3, 5, 12, 18, 24, 30, 
	36, 42, 45, 47, 49, 51, 53, 55, 
	57, 65, 72, 79, 82, 84, 86, 88, 
	90, 92, 94, 100, 109, 111, 113, 115, 
	117, 119, 122, 124, 126, 128, 130, 132, 
	134, 141, 143, 168, 169, 178, 180, 186, 
	196, 205, 214, 224, 230, 236, 243, 253, 
	254, 263, 274, 276, 287, 288, 297, 306, 
	315, 324, 333, 339, 351, 360, 366, 373, 
	382, 391, 400, 408, 417, 426, 435, 436, 
	443, 450, 453, 455, 457, 458, 460, 468, 
	477, 483, 490, 499, 508, 517, 525, 534, 
	541, 548, 557, 566, 575
};

static const unsigned char _VBD_VBN_indicies[] = {
	0, 2, 1, 3, 1, 4, 4, 4, 
	4, 4, 4, 1, 5, 5, 5, 5, 
	6, 1, 7, 7, 7, 7, 7, 1, 
	8, 8, 8, 8, 9, 1, 5, 5, 
	5, 5, 10, 1, 11, 11, 11, 11, 
	11, 1, 11, 12, 1, 11, 1, 13, 
	1, 11, 1, 14, 1, 11, 1, 11, 
	1, 4, 4, 4, 4, 4, 16, 15, 
	1, 5, 5, 5, 5, 6, 17, 1, 
	5, 5, 5, 5, 6, 18, 1, 19, 
	20, 1, 19, 1, 21, 1, 19, 1, 
	22, 1, 19, 1, 19, 1, 23, 24, 
	23, 25, 26, 1, 27, 28, 27, 29, 
	30, 31, 27, 32, 1, 33, 1, 33, 
	1, 34, 1, 33, 1, 33, 1, 35, 
	36, 1, 35, 1, 37, 1, 35, 1, 
	38, 1, 35, 1, 35, 1, 39, 39, 
	39, 39, 39, 4, 1, 40, 1, 42, 
	43, 44, 45, 46, 47, 48, 49, 50, 
	51, 52, 53, 54, 55, 56, 57, 58, 
	59, 60, 61, 62, 63, 64, 41, 1, 
	1, 65, 66, 65, 65, 65, 65, 4, 
	4, 1, 67, 1, 68, 68, 68, 68, 
	68, 1, 70, 71, 70, 69, 69, 69, 
	70, 69, 70, 1, 72, 66, 72, 72, 
	72, 72, 4, 4, 1, 65, 65, 66, 
	65, 65, 65, 4, 4, 1, 69, 69, 
	71, 69, 73, 69, 69, 70, 70, 1, 
	74, 74, 74, 74, 74, 1, 75, 76, 
	77, 78, 79, 1, 75, 76, 77, 11, 
	78, 79, 1, 65, 65, 66, 65, 65, 
	80, 65, 4, 4, 1, 81, 65, 65, 
	65, 66, 65, 65, 4, 4, 1, 4, 
	82, 4, 65, 66, 65, 65, 4, 65, 
	4, 1, 7, 1, 65, 65, 65, 71, 
	65, 83, 65, 83, 70, 70, 1, 5, 
	65, 65, 65, 66, 65, 65, 4, 4, 
	1, 84, 84, 85, 66, 84, 84, 4, 
	4, 1, 84, 84, 84, 84, 66, 84, 
	4, 4, 1, 65, 65, 65, 65, 66, 
	65, 4, 4, 1, 65, 86, 65, 87, 
	66, 65, 4, 4, 1, 5, 5, 5, 
	5, 6, 1, 88, 89, 88, 5, 89, 
	89, 5, 89, 6, 5, 88, 1, 90, 
	91, 92, 93, 94, 88, 88, 88, 1, 
	90, 95, 92, 96, 97, 1, 90, 95, 
	92, 19, 96, 97, 1, 90, 19, 91, 
	92, 93, 94, 88, 88, 1, 90, 22, 
	91, 92, 93, 94, 88, 88, 1, 98, 
	91, 92, 93, 94, 88, 88, 88, 1, 
	19, 90, 95, 92, 20, 96, 97, 1, 
	90, 100, 92, 101, 102, 99, 99, 99, 
	1, 69, 69, 69, 69, 103, 69, 70, 
	70, 1, 104, 105, 106, 65, 66, 65, 
	4, 4, 1, 107, 109, 109, 109, 109, 
	109, 109, 108, 110, 110, 110, 110, 110, 
	110, 1, 33, 111, 1, 33, 1, 112, 
	1, 108, 113, 107, 5, 5, 5, 115, 
	5, 6, 114, 1, 116, 117, 118, 119, 
	120, 114, 114, 114, 1, 116, 121, 118, 
	122, 123, 1, 116, 121, 118, 35, 122, 
	123, 1, 116, 35, 117, 118, 119, 120, 
	114, 114, 1, 116, 38, 117, 118, 119, 
	120, 114, 114, 1, 124, 117, 118, 119, 
	120, 114, 114, 114, 1, 35, 116, 121, 
	118, 36, 122, 123, 1, 116, 126, 118, 
	127, 128, 125, 125, 125, 1, 5, 5, 
	5, 5, 6, 114, 1, 4, 4, 4, 
	4, 4, 4, 1, 69, 69, 69, 69, 
	69, 71, 70, 70, 1, 84, 84, 84, 
	84, 84, 66, 4, 4, 1, 84, 84, 
	84, 84, 84, 66, 4, 4, 1, 129, 
	129, 129, 129, 129, 131, 132, 130, 1, 
	0
};

static const char _VBD_VBN_trans_targs[] = {
	2, 0, 41, 42, 43, 43, 45, 43, 
	43, 45, 45, 52, 53, 12, 14, 43, 
	43, 43, 43, 69, 70, 22, 24, 78, 
	79, 84, 85, 43, 81, 28, 83, 30, 
	32, 43, 31, 88, 89, 36, 38, 66, 
	43, 3, 44, 47, 48, 49, 50, 54, 
	16, 56, 57, 59, 61, 62, 63, 64, 
	65, 76, 77, 96, 97, 98, 99, 40, 
	100, 4, 46, 43, 5, 6, 43, 46, 
	7, 51, 8, 9, 10, 11, 13, 15, 
	55, 43, 58, 60, 17, 18, 66, 67, 
	68, 75, 19, 71, 21, 72, 73, 20, 
	23, 25, 74, 68, 71, 72, 73, 46, 
	26, 86, 95, 43, 43, 80, 27, 82, 
	29, 43, 87, 94, 33, 90, 35, 91, 
	92, 34, 37, 39, 93, 87, 90, 91, 
	92, 66, 43, 43, 46
};

static const char _VBD_VBN_trans_actions[] = {
	0, 0, 0, 31, 29, 25, 25, 5, 
	51, 51, 45, 0, 0, 0, 0, 15, 
	39, 9, 36, 0, 0, 0, 0, 25, 
	25, 25, 25, 21, 21, 0, 21, 0, 
	0, 19, 0, 0, 0, 0, 0, 29, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 27, 0, 0, 0, 0, 
	0, 0, 29, 17, 0, 0, 54, 54, 
	0, 54, 0, 0, 0, 0, 0, 0, 
	29, 27, 29, 54, 0, 0, 13, 13, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 7, 7, 7, 7, 61, 
	0, 19, 19, 23, 48, 48, 0, 19, 
	0, 42, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 17, 17, 17, 
	17, 3, 33, 3, 57
};

static const char _VBD_VBN_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 0, 5, 
	5, 5, 5, 0, 5, 5, 5, 0, 
	5, 5, 0, 5, 0, 5, 5, 5, 
	5, 5, 0, 0, 11, 11, 11, 11, 
	11, 11, 11, 11, 5, 5, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 17, 
	17, 17, 17, 17, 17, 17, 17, 0, 
	0, 5, 5, 5, 5
};

static const int VBD_VBN_start = 1;



void english_morpho_guesser::add_VBD_VBN(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  
	{
	cs = VBD_VBN_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form.c_str() + form.size())) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _VBD_VBN_trans_keys + _VBD_VBN_key_offsets[cs];
	_trans = _VBD_VBN_index_offsets[cs];

	_klen = _VBD_VBN_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _VBD_VBN_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _VBD_VBN_indicies[_trans];
	cs = _VBD_VBN_trans_targs[_trans];

	if ( _VBD_VBN_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _VBD_VBN_actions + _VBD_VBN_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (best > 'a') best = 'a', remove = 1, append = nullptr; }
	break;
	case 1:
	{ if (best > 'b') best = 'b', remove = 2, append = nullptr; }
	break;
	case 2:
	{ if (best > 'c') best = 'c', remove = 1, append = nullptr; }
	break;
	case 3:
	{ if (best > 'd') best = 'd', remove = 2, append = nullptr; }
	break;
	case 4:
	{ if (best > 'e') best = 'e', remove = 1, append = nullptr; }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 2, append = nullptr; }
	break;
	case 7:
	{ if (best > 'h') best = 'h', remove = 2, append = nullptr; }
	break;
	case 8:
	{ if (best > 'i') best = 'i', remove = 3, append = "y";     }
	break;
	case 9:
	{ if (best > 'j') best = 'j', remove = 1, append = nullptr; }
	break;
	case 10:
	{ if (best > 'k') best = 'k', remove = 2, append = nullptr; }
	break;
	case 11:
	{ if (best > 'l') best = 'l', remove = 1, append = nullptr; }
	break;
	case 12:
	{ if (best > 'm') best = 'm', remove = 2, append = nullptr; }
	break;
	case 13:
	{ if (best > 'n') best = 'n', remove = 1, append = nullptr; }
	break;
	case 14:
	{ if (best > 'o') best = 'o', remove = 2, append = nullptr; }
	break;
	case 15:
	{ if (best > 'p') best = 'p', remove = 1, append = nullptr; }
	break;
	case 16:
	{ if (best > 'q') best = 'q', remove = 2, append = nullptr; }
	break;
	case 17:
	{ if (best > 'r') best = 'r', remove = 1, append = nullptr; }
	break;
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != ( (form.c_str() + form.size())) )
		goto _resume;
	_test_eof: {}
	if ( p == ( (form.c_str() + form.size())) )
	{
	const char *__acts = _VBD_VBN_actions + _VBD_VBN_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 3:
	{ if (best > 'd') best = 'd', remove = 2, append = nullptr; }
	break;
	case 6:
	{ if (best > 'g') best = 'g', remove = 1, append = nullptr; }
	break;
	case 9:
	{ if (best > 'j') best = 'j', remove = 1, append = nullptr; }
	break;
		}
	}
	}

	_out: {}
	}


  add(VBD, VBN, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}


static const char _VBZ_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8
};

static const char _VBZ_key_offsets[] = {
	0, 0, 1, 2, 4, 14, 14, 25, 
	26, 31, 31, 31, 31, 37, 45, 54
};

static const char _VBZ_trans_keys[] = {
	115, 101, 99, 115, 98, 100, 102, 104, 
	106, 110, 112, 116, 118, 122, 122, 98, 
	100, 102, 104, 106, 110, 112, 116, 118, 
	120, 111, 97, 101, 105, 111, 117, 104, 
	105, 111, 115, 120, 122, 97, 101, 105, 
	110, 111, 114, 115, 117, 97, 101, 105, 
	111, 117, 121, 122, 98, 120, 0
};

static const char _VBZ_single_lengths[] = {
	0, 1, 1, 2, 0, 0, 1, 1, 
	5, 0, 0, 0, 6, 8, 7, 0
};

static const char _VBZ_range_lengths[] = {
	0, 0, 0, 0, 5, 0, 5, 0, 
	0, 0, 0, 0, 0, 0, 1, 0
};

static const char _VBZ_index_offsets[] = {
	0, 0, 2, 4, 7, 13, 14, 21, 
	23, 29, 30, 31, 32, 39, 48, 57
};

static const char _VBZ_indicies[] = {
	0, 1, 3, 2, 4, 4, 1, 5, 
	5, 5, 5, 5, 1, 6, 7, 7, 
	7, 7, 7, 7, 1, 8, 1, 9, 
	9, 9, 9, 9, 1, 8, 10, 1, 
	11, 12, 13, 14, 4, 15, 1, 16, 
	16, 16, 17, 16, 18, 19, 16, 1, 
	20, 20, 20, 20, 20, 20, 22, 21, 
	1, 10, 0
};

static const char _VBZ_trans_targs[] = {
	2, 0, 11, 12, 11, 5, 11, 11, 
	11, 9, 11, 3, 4, 6, 13, 14, 
	11, 7, 8, 11, 11, 10, 15
};

static const char _VBZ_trans_actions[] = {
	0, 0, 17, 17, 11, 0, 13, 15, 
	9, 0, 3, 0, 0, 0, 11, 11, 
	1, 0, 0, 7, 5, 0, 7
};

static const int VBZ_start = 1;



void english_morpho_guesser::add_VBZ(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  
	{
	cs = VBZ_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form.c_str() + form.size())) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _VBZ_trans_keys + _VBZ_key_offsets[cs];
	_trans = _VBZ_index_offsets[cs];

	_klen = _VBZ_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _VBZ_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form[form.size() - 1 - (p - form.c_str())]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form[form.size() - 1 - (p - form.c_str())]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _VBZ_indicies[_trans];
	cs = _VBZ_trans_targs[_trans];

	if ( _VBZ_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _VBZ_actions + _VBZ_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (best > 'a') best = 'a', remove = 1, append = nullptr; }
	break;
	case 1:
	{ if (best > 'b') best = 'b', remove = 2, append = nullptr; }
	break;
	case 2:
	{ if (best > 'c') best = 'c', remove = 1, append = nullptr; }
	break;
	case 3:
	{ if (best > 'd') best = 'd', remove = 2, append = nullptr; }
	break;
	case 4:
	{ if (best > 'e') best = 'e', remove = 1, append = nullptr; }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 2, append = nullptr; }
	break;
	case 6:
	{ if (best > 'g') best = 'g', remove = 3, append = "y";     }
	break;
	case 7:
	{ if (best > 'h') best = 'h', remove = 2, append = nullptr; }
	break;
	case 8:
	{ if (best > 'i') best = 'i', remove = 1, append = nullptr; }
	break;
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != ( (form.c_str() + form.size())) )
		goto _resume;
	_test_eof: {}
	_out: {}
	}


  add(VBZ, form.substr(0, form.size() - remove).append(append ? append : ""), lemmas);
}


static const char _JJR_RBR_actions[] = {
	0, 1, 0, 1, 1, 1, 3, 1, 
	4, 1, 5, 2, 1, 4, 2, 2, 
	5, 2, 4, 5
};

static const unsigned char _JJR_RBR_key_offsets[] = {
	0, 0, 1, 2, 26, 26, 32, 37, 
	50, 56, 62, 73, 79, 85, 91, 102, 
	103, 109, 115, 117, 123, 129, 135, 146, 
	152, 163, 169, 175, 181
};

static const char _JJR_RBR_trans_keys[] = {
	114, 101, 98, 99, 100, 101, 102, 103, 
	104, 105, 106, 107, 108, 109, 110, 112, 
	113, 114, 115, 116, 117, 118, 119, 120, 
	121, 122, 97, 98, 101, 105, 111, 117, 
	97, 101, 105, 111, 117, 98, 99, 100, 
	105, 111, 117, 122, 97, 101, 102, 109, 
	112, 120, 97, 100, 101, 105, 111, 117, 
	97, 101, 102, 105, 111, 117, 97, 101, 
	103, 105, 111, 117, 122, 98, 109, 112, 
	120, 97, 101, 104, 105, 111, 117, 97, 
	101, 105, 106, 111, 117, 97, 101, 105, 
	107, 111, 117, 97, 101, 105, 108, 111, 
	117, 122, 98, 109, 112, 120, 101, 97, 
	101, 105, 109, 111, 117, 97, 101, 105, 
	110, 111, 117, 97, 122, 97, 101, 105, 
	111, 112, 117, 97, 101, 105, 111, 113, 
	117, 97, 101, 105, 111, 114, 117, 97, 
	101, 105, 111, 115, 117, 122, 98, 109, 
	112, 120, 97, 101, 105, 111, 116, 117, 
	97, 101, 105, 111, 117, 118, 122, 98, 
	109, 112, 120, 97, 101, 105, 111, 117, 
	119, 97, 101, 105, 111, 117, 120, 97, 
	101, 105, 111, 117, 121, 97, 101, 105, 
	111, 117, 122, 0
};

static const char _JJR_RBR_single_lengths[] = {
	0, 1, 1, 24, 0, 6, 5, 7, 
	6, 6, 7, 6, 6, 6, 7, 1, 
	6, 6, 0, 6, 6, 6, 7, 6, 
	7, 6, 6, 6, 6
};

static const char _JJR_RBR_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 3, 
	0, 0, 2, 0, 0, 0, 2, 0, 
	0, 0, 1, 0, 0, 0, 2, 0, 
	2, 0, 0, 0, 0
};

static const unsigned char _JJR_RBR_index_offsets[] = {
	0, 0, 2, 4, 29, 30, 37, 43, 
	54, 61, 68, 78, 85, 92, 99, 109, 
	111, 118, 125, 127, 134, 141, 148, 158, 
	165, 175, 182, 189, 196
};

static const char _JJR_RBR_indicies[] = {
	0, 1, 2, 1, 4, 5, 6, 7, 
	8, 9, 10, 11, 12, 13, 14, 15, 
	16, 17, 18, 19, 20, 21, 7, 22, 
	23, 24, 25, 26, 3, 1, 27, 28, 
	27, 27, 27, 27, 1, 29, 29, 29, 
	29, 29, 1, 30, 31, 30, 27, 27, 
	27, 30, 27, 30, 30, 1, 27, 28, 
	27, 27, 27, 27, 1, 27, 27, 28, 
	27, 27, 27, 1, 27, 27, 31, 27, 
	27, 27, 30, 30, 30, 1, 27, 27, 
	28, 27, 27, 27, 1, 27, 27, 27, 
	28, 27, 27, 1, 27, 27, 27, 28, 
	27, 27, 1, 27, 27, 27, 32, 27, 
	27, 30, 30, 30, 1, 1, 33, 27, 
	27, 27, 28, 27, 27, 1, 34, 34, 
	34, 28, 34, 34, 1, 29, 1, 34, 
	34, 34, 34, 28, 34, 1, 27, 27, 
	27, 27, 28, 27, 1, 27, 27, 27, 
	27, 28, 27, 1, 27, 27, 27, 27, 
	31, 27, 30, 30, 30, 1, 27, 27, 
	27, 27, 28, 27, 1, 27, 27, 27, 
	27, 27, 31, 30, 30, 30, 1, 34, 
	34, 34, 34, 34, 28, 1, 34, 34, 
	34, 34, 34, 28, 1, 27, 27, 27, 
	27, 27, 28, 1, 27, 27, 27, 27, 
	27, 28, 1, 0
};

static const char _JJR_RBR_trans_targs[] = {
	2, 0, 3, 4, 5, 7, 8, 4, 
	9, 10, 11, 4, 12, 13, 14, 16, 
	17, 19, 20, 21, 22, 23, 24, 25, 
	26, 27, 28, 6, 4, 4, 4, 4, 
	15, 4, 18
};

static const char _JJR_RBR_trans_actions[] = {
	0, 0, 0, 9, 9, 9, 9, 17, 
	9, 9, 9, 14, 9, 9, 9, 9, 
	9, 9, 9, 9, 9, 9, 9, 9, 
	9, 9, 9, 7, 3, 5, 7, 11, 
	11, 1, 7
};

static const int JJR_RBR_start = 1;



void english_morpho_guesser::add_JJR_RBR(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  
	{
	cs = JJR_RBR_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form.c_str() + form.size())) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _JJR_RBR_trans_keys + _JJR_RBR_key_offsets[cs];
	_trans = _JJR_RBR_index_offsets[cs];

	_klen = _JJR_RBR_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _JJR_RBR_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _JJR_RBR_indicies[_trans];
	cs = _JJR_RBR_trans_targs[_trans];

	if ( _JJR_RBR_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _JJR_RBR_actions + _JJR_RBR_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (best > 'a') best = 'a', remove = 2, append = nullptr; }
	break;
	case 1:
	{ if (best > 'b') best = 'b', remove = 3, append = nullptr; }
	break;
	case 2:
	{ if (best > 'c') best = 'c', remove = 3, append = "y";     }
	break;
	case 3:
	{ if (best > 'd') best = 'd', remove = 2, append = nullptr; }
	break;
	case 4:
	{ if (best > 'e') best = 'e', remove = 1, append = nullptr; }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 2, append = nullptr; }
	break;
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != ( (form.c_str() + form.size())) )
		goto _resume;
	_test_eof: {}
	_out: {}
	}


  add(JJR, RBR, form.substr(0, form.size() - remove).append(append ? append : ""), negation_len, lemmas);
}


static const char _JJS_RBS_actions[] = {
	0, 1, 1, 1, 2, 1, 4, 1, 
	5, 2, 0, 5, 2, 1, 4, 2, 
	3, 5
};

static const unsigned char _JJS_RBS_key_offsets[] = {
	0, 0, 1, 2, 3, 25, 25, 25, 
	31, 44, 50, 56, 67, 73, 79, 85, 
	96, 102, 108, 114, 120, 126, 137, 143, 
	154, 160, 166, 172, 178, 178, 183, 183, 
	183, 184
};

static const char _JJS_RBS_trans_keys[] = {
	116, 115, 101, 98, 99, 100, 102, 103, 
	104, 105, 106, 107, 108, 109, 110, 112, 
	113, 114, 115, 116, 118, 119, 120, 121, 
	122, 97, 98, 101, 105, 111, 117, 98, 
	99, 100, 105, 111, 117, 122, 97, 101, 
	102, 109, 112, 120, 97, 100, 101, 105, 
	111, 117, 97, 101, 102, 105, 111, 117, 
	97, 101, 103, 105, 111, 117, 122, 98, 
	109, 112, 120, 97, 101, 104, 105, 111, 
	117, 97, 101, 105, 106, 111, 117, 97, 
	101, 105, 107, 111, 117, 97, 101, 105, 
	108, 111, 117, 122, 98, 109, 112, 120, 
	97, 101, 105, 109, 111, 117, 97, 101, 
	105, 110, 111, 117, 97, 101, 105, 111, 
	112, 117, 97, 101, 105, 111, 113, 117, 
	97, 101, 105, 111, 114, 117, 97, 101, 
	105, 111, 115, 117, 122, 98, 109, 112, 
	120, 97, 101, 105, 111, 116, 117, 97, 
	101, 105, 111, 117, 118, 122, 98, 109, 
	112, 120, 97, 101, 105, 111, 117, 119, 
	97, 101, 105, 111, 117, 120, 97, 101, 
	105, 111, 117, 121, 97, 101, 105, 111, 
	117, 122, 97, 101, 105, 111, 117, 101, 
	97, 122, 0
};

static const char _JJS_RBS_single_lengths[] = {
	0, 1, 1, 1, 22, 0, 0, 6, 
	7, 6, 6, 7, 6, 6, 6, 7, 
	6, 6, 6, 6, 6, 7, 6, 7, 
	6, 6, 6, 6, 0, 5, 0, 0, 
	1, 0
};

static const char _JJS_RBS_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	3, 0, 0, 2, 0, 0, 0, 2, 
	0, 0, 0, 0, 0, 2, 0, 2, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1
};

static const unsigned char _JJS_RBS_index_offsets[] = {
	0, 0, 2, 4, 6, 29, 30, 31, 
	38, 49, 56, 63, 73, 80, 87, 94, 
	104, 111, 118, 125, 132, 139, 149, 156, 
	166, 173, 180, 187, 194, 195, 201, 202, 
	203, 205
};

static const char _JJS_RBS_indicies[] = {
	0, 1, 2, 1, 3, 1, 5, 6, 
	7, 8, 9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 
	23, 24, 25, 26, 4, 27, 28, 29, 
	30, 29, 29, 29, 29, 27, 31, 32, 
	31, 29, 29, 29, 31, 29, 31, 31, 
	27, 29, 30, 29, 29, 29, 29, 27, 
	29, 29, 30, 29, 29, 29, 27, 29, 
	29, 32, 29, 29, 29, 31, 31, 31, 
	27, 29, 29, 30, 29, 29, 29, 27, 
	29, 29, 29, 30, 29, 29, 27, 29, 
	29, 29, 30, 29, 29, 27, 29, 29, 
	29, 33, 29, 29, 31, 31, 31, 27, 
	29, 29, 29, 30, 29, 29, 27, 34, 
	34, 34, 30, 34, 34, 27, 34, 34, 
	34, 34, 30, 34, 27, 29, 29, 29, 
	29, 30, 29, 27, 29, 29, 29, 29, 
	30, 29, 27, 29, 29, 29, 29, 32, 
	29, 31, 31, 31, 27, 29, 29, 29, 
	29, 30, 29, 27, 29, 29, 29, 29, 
	29, 32, 31, 31, 31, 27, 34, 34, 
	34, 34, 34, 30, 27, 34, 34, 34, 
	34, 34, 30, 27, 29, 29, 29, 29, 
	29, 30, 27, 29, 29, 29, 29, 29, 
	30, 27, 1, 35, 35, 35, 35, 35, 
	28, 28, 27, 28, 36, 35, 28, 0
};

static const char _JJS_RBS_trans_targs[] = {
	2, 0, 3, 4, 5, 7, 8, 9, 
	10, 11, 12, 31, 13, 14, 15, 16, 
	17, 18, 19, 20, 21, 22, 23, 24, 
	25, 26, 27, 6, 28, 29, 30, 30, 
	30, 32, 33, 28, 28
};

static const char _JJS_RBS_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 3, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 7, 5, 1, 5, 
	12, 12, 5, 15, 9
};

static const int JJS_RBS_start = 1;



void english_morpho_guesser::add_JJS_RBS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  char best = 'z'; unsigned remove = 0; const char* append = nullptr;
  
	{
	cs = JJS_RBS_start;
	}

	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == ( (form.c_str() + form.size())) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _JJS_RBS_trans_keys + _JJS_RBS_key_offsets[cs];
	_trans = _JJS_RBS_index_offsets[cs];

	_klen = _JJS_RBS_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) < *_mid )
				_upper = _mid - 1;
			else if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _JJS_RBS_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) < _mid[0] )
				_upper = _mid - 2;
			else if ( ( form[form.size() - 1 - (p - form.c_str() - negation_len)]) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _JJS_RBS_indicies[_trans];
	cs = _JJS_RBS_trans_targs[_trans];

	if ( _JJS_RBS_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _JJS_RBS_actions + _JJS_RBS_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
	{ if (best > 'a') best = 'a', remove = 3, append = nullptr; }
	break;
	case 1:
	{ if (best > 'b') best = 'b', remove = 4, append = nullptr; }
	break;
	case 2:
	{ if (best > 'c') best = 'c', remove = 4, append = "y";     }
	break;
	case 3:
	{ if (best > 'd') best = 'd', remove = 3, append = nullptr; }
	break;
	case 4:
	{ if (best > 'e') best = 'e', remove = 2, append = nullptr; }
	break;
	case 5:
	{ if (best > 'f') best = 'f', remove = 3, append = nullptr; }
	break;
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != ( (form.c_str() + form.size())) )
		goto _resume;
	_test_eof: {}
	_out: {}
	}


  add(JJS, RBS, form.substr(0, form.size() - remove).append(append ? append : ""), negation_len, lemmas);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
