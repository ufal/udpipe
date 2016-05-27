#!/bin/sh

../../src/udpipe --train --tokenizer=epochs=2 --tagger=iterations=2 --parser=embedding_feats=5\;embedding_upostag=5\;embedding_form=5\;embedding_deprel=5\;hidden_layer=5\;iterations=20\;learning_rate=0.5\;learning_rate_final=0.05 test.model <<EOF
1	Znamená	znamenat	VERB	VB-S---3P-AA---	Aspect=Imp|Mood=Ind|Negative=Pos|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	0	root	_	_
2	to	ten	PRON	PDNS1----------	Case=Nom|Gender=Neut|Number=Sing|PronType=Dem	1	nsubj	_	SpaceAfter=No
3	,	,	PUNCT	Z:-------------	_	6	punct	_	_
4	že	že	SCONJ	J,-------------	_	6	mark	_	_
5	realitě	realita	NOUN	NNFS3-----A----	Case=Dat|Gender=Fem|Negative=Pos|Number=Sing	6	dobj	_	_
6	nepodléhá	podléhat	VERB	VB-S---3P-NA---	Aspect=Imp|Mood=Ind|Negative=Neg|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	1	ccomp	_	SpaceAfter=No
7	.	.	PUNCT	Z:-------------	_	1	punct	_	_

1	Znamená	znamenat	VERB	VB-S---3P-AA---	Aspect=Imp|Mood=Ind|Negative=Pos|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	0	root	_	_
2	to	ten	PRON	PDNS1----------	Case=Nom|Gender=Neut|Number=Sing|PronType=Dem	1	nsubj	_	SpaceAfter=No
3	,	,	PUNCT	Z:-------------	_	6	punct	_	_
4	že	že	SCONJ	J,-------------	_	6	mark	_	_
5	realitě	realita	NOUN	NNFS3-----A----	Case=Dat|Gender=Fem|Negative=Pos|Number=Sing	6	dobj	_	_
6	nepodléhá	podléhat	VERB	VB-S---3P-NA---	Aspect=Imp|Mood=Ind|Negative=Neg|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	1	ccomp	_	SpaceAfter=No
7	.	.	PUNCT	Z:-------------	_	1	punct	_	_

EOF
