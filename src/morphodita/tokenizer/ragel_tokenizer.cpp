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





static const char _ragel_url_email_cond_offsets[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 2, 3, 3, 4, 5, 
	6, 7, 8, 9, 10, 11, 12, 13, 
	14, 15, 16
};

static const char _ragel_url_email_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1
};

static const short _ragel_url_email_cond_keys[] = {
	41u, 41u, 47u, 47u, 47u, 47u, 41u, 41u, 
	47u, 47u, 47u, 47u, 47u, 47u, 47u, 47u, 
	47u, 47u, 47u, 47u, 47u, 47u, 47u, 47u, 
	47u, 47u, 47u, 47u, 47u, 47u, 47u, 47u, 
	47u, 47u, 0
};

static const char _ragel_url_email_cond_spaces[] = {
	1, 0, 0, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0
};

static const short _ragel_url_email_key_offsets[] = {
	0, 0, 15, 29, 41, 54, 63, 71, 
	78, 86, 92, 100, 117, 145, 154, 162, 
	171, 179, 188, 196, 204, 215, 225, 233, 
	241, 252, 262, 270, 278, 289, 299, 315, 
	330, 346, 360, 376, 393, 409, 426, 442, 
	459, 475, 491, 510, 528, 544, 560, 579, 
	597, 613, 629, 648, 666, 682, 698, 714, 
	725, 726, 741, 752, 756, 773, 801, 812, 
	823, 834, 848, 861, 879, 893, 908, 926, 
	944, 962, 983
};

static const short _ragel_url_email_trans_keys[] = {
	33u, 48u, 49u, 50u, 95u, 36u, 37u, 39u, 
	46u, 51u, 57u, 65u, 90u, 97u, 122u, 33u, 
	58u, 64u, 95u, 36u, 37u, 39u, 46u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 95u, 36u, 
	37u, 39u, 46u, 48u, 57u, 65u, 90u, 97u, 
	122u, 33u, 64u, 95u, 36u, 37u, 39u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 48u, 49u, 
	50u, 51u, 57u, 65u, 90u, 97u, 122u, 45u, 
	46u, 48u, 57u, 65u, 90u, 97u, 122u, 45u, 
	48u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 48u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 39u, 41u, 61u, 
	95u, 36u, 47u, 48u, 57u, 58u, 59u, 63u, 
	64u, 65u, 90u, 97u, 122u, 33u, 39u, 40u, 
	44u, 46u, 61u, 63u, 95u, 129u, 131u, 135u, 
	151u, 809u, 1065u, 36u, 38u, 42u, 57u, 58u, 
	59u, 64u, 90u, 97u, 122u, 142u, 143u, 155u, 
	159u, 48u, 49u, 50u, 51u, 57u, 65u, 90u, 
	97u, 122u, 45u, 46u, 48u, 57u, 65u, 90u, 
	97u, 122u, 48u, 49u, 50u, 51u, 57u, 65u, 
	90u, 97u, 122u, 45u, 46u, 48u, 57u, 65u, 
	90u, 97u, 122u, 48u, 49u, 50u, 51u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 48u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 48u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 53u, 48u, 
	52u, 54u, 57u, 65u, 90u, 97u, 122u, 45u, 
	46u, 48u, 53u, 54u, 57u, 65u, 90u, 97u, 
	122u, 45u, 46u, 48u, 57u, 65u, 90u, 97u, 
	122u, 45u, 46u, 48u, 57u, 65u, 90u, 97u, 
	122u, 45u, 46u, 53u, 48u, 52u, 54u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 48u, 53u, 
	54u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	53u, 48u, 52u, 54u, 57u, 65u, 90u, 97u, 
	122u, 45u, 46u, 48u, 53u, 54u, 57u, 65u, 
	90u, 97u, 122u, 33u, 45u, 46u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 57u, 65u, 
	90u, 97u, 122u, 33u, 45u, 58u, 64u, 95u, 
	36u, 37u, 39u, 46u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 58u, 64u, 95u, 36u, 37u, 
	39u, 46u, 48u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 58u, 64u, 95u, 36u, 37u, 
	39u, 44u, 48u, 57u, 65u, 90u, 97u, 122u, 
	33u, 48u, 49u, 50u, 58u, 64u, 95u, 36u, 
	37u, 39u, 46u, 51u, 57u, 65u, 90u, 97u, 
	122u, 33u, 45u, 46u, 58u, 64u, 95u, 36u, 
	37u, 39u, 44u, 48u, 57u, 65u, 90u, 97u, 
	122u, 33u, 48u, 49u, 50u, 58u, 64u, 95u, 
	36u, 37u, 39u, 46u, 51u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 48u, 49u, 50u, 58u, 64u, 
	95u, 36u, 37u, 39u, 46u, 51u, 57u, 65u, 
	90u, 97u, 122u, 33u, 45u, 46u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 57u, 65u, 
	90u, 97u, 122u, 33u, 45u, 46u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 57u, 65u, 
	90u, 97u, 122u, 33u, 45u, 46u, 53u, 58u, 
	64u, 95u, 36u, 37u, 39u, 44u, 48u, 52u, 
	54u, 57u, 65u, 90u, 97u, 122u, 33u, 45u, 
	46u, 58u, 64u, 95u, 36u, 37u, 39u, 44u, 
	48u, 53u, 54u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 58u, 64u, 95u, 36u, 37u, 
	39u, 44u, 48u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 58u, 64u, 95u, 36u, 37u, 
	39u, 44u, 48u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 53u, 58u, 64u, 95u, 36u, 
	37u, 39u, 44u, 48u, 52u, 54u, 57u, 65u, 
	90u, 97u, 122u, 33u, 45u, 46u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 53u, 54u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	58u, 64u, 95u, 36u, 37u, 39u, 44u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	58u, 64u, 95u, 36u, 37u, 39u, 44u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	53u, 58u, 64u, 95u, 36u, 37u, 39u, 44u, 
	48u, 52u, 54u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 58u, 64u, 95u, 36u, 37u, 
	39u, 44u, 48u, 53u, 54u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 47u, 95u, 36u, 37u, 39u, 
	57u, 65u, 90u, 97u, 122u, 47u, 33u, 48u, 
	49u, 50u, 95u, 36u, 37u, 39u, 46u, 51u, 
	57u, 65u, 90u, 97u, 122u, 45u, 46u, 58u, 
	303u, 559u, 48u, 57u, 65u, 90u, 97u, 122u, 
	303u, 559u, 48u, 57u, 33u, 39u, 41u, 61u, 
	95u, 36u, 47u, 48u, 57u, 58u, 59u, 63u, 
	64u, 65u, 90u, 97u, 122u, 33u, 39u, 40u, 
	44u, 46u, 61u, 63u, 95u, 129u, 131u, 135u, 
	151u, 809u, 1065u, 36u, 38u, 42u, 57u, 58u, 
	59u, 64u, 90u, 97u, 122u, 142u, 143u, 155u, 
	159u, 45u, 46u, 58u, 303u, 559u, 48u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 58u, 303u, 
	559u, 48u, 57u, 65u, 90u, 97u, 122u, 45u, 
	46u, 58u, 303u, 559u, 48u, 57u, 65u, 90u, 
	97u, 122u, 45u, 46u, 53u, 58u, 303u, 559u, 
	48u, 52u, 54u, 57u, 65u, 90u, 97u, 122u, 
	45u, 46u, 58u, 303u, 559u, 48u, 53u, 54u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	58u, 64u, 95u, 303u, 559u, 36u, 37u, 39u, 
	44u, 48u, 57u, 65u, 90u, 97u, 122u, 33u, 
	95u, 303u, 559u, 36u, 37u, 39u, 46u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 64u, 95u, 
	303u, 559u, 36u, 37u, 39u, 46u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 45u, 46u, 58u, 
	64u, 95u, 303u, 559u, 36u, 37u, 39u, 44u, 
	48u, 57u, 65u, 90u, 97u, 122u, 33u, 45u, 
	46u, 58u, 64u, 95u, 303u, 559u, 36u, 37u, 
	39u, 44u, 48u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 58u, 64u, 95u, 303u, 559u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 53u, 58u, 64u, 
	95u, 303u, 559u, 36u, 37u, 39u, 44u, 48u, 
	52u, 54u, 57u, 65u, 90u, 97u, 122u, 33u, 
	45u, 46u, 58u, 64u, 95u, 303u, 559u, 36u, 
	37u, 39u, 44u, 48u, 53u, 54u, 57u, 65u, 
	90u, 97u, 122u, 0
};

static const char _ragel_url_email_single_lengths[] = {
	0, 5, 4, 2, 3, 3, 2, 1, 
	2, 0, 2, 5, 14, 3, 2, 3, 
	2, 3, 2, 2, 3, 2, 2, 2, 
	3, 2, 2, 2, 3, 2, 6, 5, 
	6, 4, 6, 7, 6, 7, 6, 7, 
	6, 6, 7, 6, 6, 6, 7, 6, 
	6, 6, 7, 6, 6, 6, 6, 3, 
	1, 5, 5, 2, 5, 14, 5, 5, 
	5, 6, 5, 8, 4, 5, 8, 8, 
	8, 9, 8
};

static const char _ragel_url_email_range_lengths[] = {
	0, 5, 5, 5, 5, 3, 3, 3, 
	3, 3, 3, 6, 7, 3, 3, 3, 
	3, 3, 3, 3, 4, 4, 3, 3, 
	4, 4, 3, 3, 4, 4, 5, 5, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	5, 5, 6, 6, 5, 5, 6, 6, 
	5, 5, 6, 6, 5, 5, 5, 4, 
	0, 5, 3, 1, 6, 7, 3, 3, 
	3, 4, 4, 5, 5, 5, 5, 5, 
	5, 6, 6
};

static const short _ragel_url_email_index_offsets[] = {
	0, 0, 11, 21, 29, 38, 45, 51, 
	56, 62, 66, 72, 84, 106, 113, 119, 
	126, 132, 139, 145, 151, 159, 166, 172, 
	178, 186, 193, 199, 205, 213, 220, 232, 
	243, 255, 265, 277, 290, 302, 315, 327, 
	340, 352, 364, 378, 391, 403, 415, 429, 
	442, 454, 466, 480, 493, 505, 517, 529, 
	537, 539, 550, 559, 563, 575, 597, 606, 
	615, 624, 635, 645, 659, 669, 680, 694, 
	708, 722, 738
};

static const char _ragel_url_email_indicies[] = {
	0, 2, 3, 4, 0, 0, 0, 5, 
	6, 6, 1, 0, 7, 8, 0, 0, 
	0, 0, 0, 0, 1, 9, 9, 9, 
	9, 9, 9, 9, 1, 9, 8, 9, 
	9, 9, 9, 9, 9, 1, 10, 11, 
	12, 13, 14, 14, 1, 15, 16, 14, 
	14, 14, 1, 15, 14, 14, 14, 1, 
	15, 17, 14, 14, 14, 1, 14, 18, 
	18, 1, 15, 17, 14, 19, 19, 1, 
	20, 21, 21, 20, 20, 20, 21, 20, 
	20, 21, 21, 1, 22, 22, 24, 22, 
	22, 23, 22, 23, 23, 23, 23, 23, 
	25, 26, 23, 23, 22, 23, 23, 23, 
	23, 1, 27, 28, 29, 30, 18, 18, 
	1, 15, 31, 14, 14, 14, 1, 32, 
	33, 34, 35, 18, 18, 1, 15, 36, 
	14, 14, 14, 1, 37, 38, 39, 40, 
	18, 18, 1, 15, 36, 35, 14, 14, 
	1, 15, 36, 32, 14, 14, 1, 15, 
	36, 41, 35, 32, 14, 14, 1, 15, 
	36, 32, 14, 14, 14, 1, 15, 31, 
	30, 14, 14, 1, 15, 31, 27, 14, 
	14, 1, 15, 31, 42, 30, 27, 14, 
	14, 1, 15, 31, 27, 14, 14, 14, 
	1, 15, 16, 13, 14, 14, 1, 15, 
	16, 10, 14, 14, 1, 15, 16, 43, 
	13, 10, 14, 14, 1, 15, 16, 10, 
	14, 14, 14, 1, 0, 44, 45, 7, 
	8, 0, 0, 0, 46, 46, 46, 1, 
	0, 44, 7, 8, 0, 0, 0, 46, 
	46, 46, 1, 0, 44, 47, 7, 8, 
	0, 0, 0, 46, 46, 46, 1, 0, 
	7, 8, 0, 0, 0, 46, 48, 48, 
	1, 0, 44, 47, 7, 8, 0, 0, 
	0, 46, 49, 49, 1, 0, 50, 51, 
	52, 7, 8, 0, 0, 0, 53, 48, 
	48, 1, 0, 44, 54, 7, 8, 0, 
	0, 0, 46, 46, 46, 1, 0, 55, 
	56, 57, 7, 8, 0, 0, 0, 58, 
	48, 48, 1, 0, 44, 59, 7, 8, 
	0, 0, 0, 46, 46, 46, 1, 0, 
	60, 61, 62, 7, 8, 0, 0, 0, 
	63, 48, 48, 1, 0, 44, 59, 7, 
	8, 0, 0, 0, 58, 46, 46, 1, 
	0, 44, 59, 7, 8, 0, 0, 0, 
	55, 46, 46, 1, 0, 44, 59, 64, 
	7, 8, 0, 0, 0, 58, 55, 46, 
	46, 1, 0, 44, 59, 7, 8, 0, 
	0, 0, 55, 46, 46, 46, 1, 0, 
	44, 54, 7, 8, 0, 0, 0, 53, 
	46, 46, 1, 0, 44, 54, 7, 8, 
	0, 0, 0, 50, 46, 46, 1, 0, 
	44, 54, 65, 7, 8, 0, 0, 0, 
	53, 50, 46, 46, 1, 0, 44, 54, 
	7, 8, 0, 0, 0, 50, 46, 46, 
	46, 1, 0, 44, 45, 7, 8, 0, 
	0, 0, 5, 46, 46, 1, 0, 44, 
	45, 7, 8, 0, 0, 0, 2, 46, 
	46, 1, 0, 44, 45, 66, 7, 8, 
	0, 0, 0, 5, 2, 46, 46, 1, 
	0, 44, 45, 7, 8, 0, 0, 0, 
	2, 46, 46, 46, 1, 0, 44, 47, 
	7, 8, 0, 0, 0, 46, 67, 67, 
	1, 0, 44, 47, 7, 8, 0, 0, 
	0, 46, 68, 68, 1, 0, 44, 47, 
	69, 8, 0, 0, 0, 46, 68, 68, 
	1, 9, 70, 9, 9, 9, 9, 9, 
	1, 71, 1, 0, 2, 3, 4, 0, 
	0, 0, 5, 46, 46, 1, 15, 17, 
	72, 21, 23, 14, 19, 19, 1, 21, 
	23, 72, 1, 20, 21, 21, 20, 20, 
	20, 21, 20, 20, 21, 21, 1, 22, 
	22, 24, 22, 22, 23, 22, 23, 23, 
	23, 23, 23, 25, 26, 23, 23, 22, 
	23, 23, 23, 23, 1, 15, 17, 72, 
	21, 23, 14, 14, 14, 1, 15, 17, 
	72, 21, 23, 40, 14, 14, 1, 15, 
	17, 72, 21, 23, 37, 14, 14, 1, 
	15, 17, 73, 72, 21, 23, 40, 37, 
	14, 14, 1, 15, 17, 72, 21, 23, 
	37, 14, 14, 14, 1, 0, 44, 47, 
	74, 8, 0, 21, 23, 0, 0, 46, 
	49, 49, 1, 9, 9, 21, 23, 9, 
	9, 75, 9, 9, 1, 9, 8, 9, 
	21, 23, 9, 9, 75, 9, 9, 1, 
	0, 44, 47, 74, 8, 0, 21, 23, 
	0, 0, 46, 46, 46, 1, 0, 44, 
	47, 74, 8, 0, 21, 23, 0, 0, 
	63, 46, 46, 1, 0, 44, 47, 74, 
	8, 0, 21, 23, 0, 0, 60, 46, 
	46, 1, 0, 44, 47, 76, 74, 8, 
	0, 21, 23, 0, 0, 63, 60, 46, 
	46, 1, 0, 44, 47, 74, 8, 0, 
	21, 23, 0, 0, 60, 46, 46, 46, 
	1, 0
};

static const char _ragel_url_email_trans_targs[] = {
	2, 0, 30, 48, 50, 49, 52, 3, 
	5, 4, 6, 26, 28, 27, 8, 7, 
	13, 9, 10, 58, 11, 60, 12, 61, 
	61, 12, 61, 14, 22, 24, 23, 15, 
	16, 18, 20, 19, 17, 62, 63, 65, 
	64, 21, 25, 29, 31, 35, 32, 33, 
	34, 67, 36, 44, 46, 45, 37, 38, 
	40, 42, 41, 39, 70, 71, 73, 72, 
	43, 47, 51, 53, 54, 55, 56, 57, 
	59, 66, 68, 69, 74
};

static const char _ragel_url_email_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 0, 1, 0, 1, 
	2, 3, 4, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 1, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 1, 1, 1, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 1, 1, 1
};

static const int ragel_url_email_start = 1;





vector<uint8_t> ragel_tokenizer::ragel_map;
atomic_flag ragel_tokenizer::ragel_map_flag = ATOMIC_FLAG_INIT;

ragel_tokenizer::ragel_tokenizer(unsigned url_email_tokenizer) : unicode_tokenizer(url_email_tokenizer) {
  initialize_ragel_map();
}

void ragel_tokenizer::initialize_ragel_map() {
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
  
	{
	cs = ragel_url_email_start;
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
	_widec = ( ragel_char(chars[current]));
	_klen = _ragel_url_email_cond_lengths[cs];
	_keys = _ragel_url_email_cond_keys + (_ragel_url_email_cond_offsets[cs]*2);
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
				switch ( _ragel_url_email_cond_spaces[_ragel_url_email_cond_offsets[cs] + ((_mid - _keys)>>1)] ) {
	case 0: {
		_widec = (short)(256u + (( ragel_char(chars[current])) - 0u));
		if ( 
 version >= 2  ) _widec += 256;
		break;
	}
	case 1: {
		_widec = (short)(768u + (( ragel_char(chars[current])) - 0u));
		if ( 
parens ) _widec += 256;
		break;
	}
				}
				break;
			}
		}
	}

	_keys = _ragel_url_email_trans_keys + _ragel_url_email_key_offsets[cs];
	_trans = _ragel_url_email_index_offsets[cs];

	_klen = _ragel_url_email_single_lengths[cs];
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

	_klen = _ragel_url_email_range_lengths[cs];
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
	_trans = _ragel_url_email_indicies[_trans];
	cs = _ragel_url_email_trans_targs[_trans];

	if ( _ragel_url_email_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _ragel_url_email_trans_actions[_trans] ) {
	case 3:
	{parens-=!!parens;}
	break;
	case 1:
	{ end = current + 1; }
	break;
	case 2:
	{parens++;}
	{ end = current + 1; }
	break;
	case 4:
	{parens-=!!parens;}
	{ end = current + 1; }
	break;
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++( current) != ( (chars.size() - 1)) )
		goto _resume;
	_test_eof: {}
	_out: {}
	}



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
