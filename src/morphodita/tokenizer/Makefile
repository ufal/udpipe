# This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
#
# Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

TOKENIZERS=$(patsubst %,tokenizer/%_tokenizer.cpp,czech english generic ragel)

$(TOKENIZERS): %.cpp: %.rl tokenizer/ragel_tokenizer.rl
	ragel $< -T1 -o $@ && sed '1d; /^#line [0-9]/d; /^static const int [^ ]*_en_main = [0-9]*;$$/d' -i $@
