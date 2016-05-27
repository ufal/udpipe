#!/usr/bin/python
# vim:fileencoding=utf8
from __future__ import unicode_literals

import unittest

class TestUDPipe(unittest.TestCase):
    def test_model(self):
        import ufal.udpipe

        model = ufal.udpipe.Model.load('test/data/test.model')
        self.assertTrue(model)

        tokenizer = model.newTokenizer(model.DEFAULT)
        conlluOutput = ufal.udpipe.OutputFormat.newOutputFormat("conllu")
        sentence = ufal.udpipe.Sentence()
        error = ufal.udpipe.ProcessingError();

        tokenizer.setText("Znamená to, že realitě nepodléhá.");
        self.assertTrue(tokenizer.nextSentence(sentence, error))
        self.assertFalse(error.occurred())

        self.assertTrue(model.tag(sentence, model.DEFAULT))
        self.assertTrue(model.parse(sentence, model.DEFAULT))

        self.assertEqual(conlluOutput.writeSentence(sentence), """\
1	Znamená	znamenat	VERB	VB-S---3P-AA---	Aspect=Imp|Mood=Ind|Negative=Pos|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	0	root	_	_
2	to	ten	PRON	PDNS1----------	Case=Nom|Gender=Neut|Number=Sing|PronType=Dem	1	nsubj	_	SpaceAfter=No
3	,	,	PUNCT	Z:-------------	_	6	punct	_	_
4	že	že	SCONJ	J,-------------	_	6	mark	_	_
5	realitě	realita	NOUN	NNFS3-----A----	Case=Dat|Gender=Fem|Negative=Pos|Number=Sing	6	dobj	_	_
6	nepodléhá	podléhat	VERB	VB-S---3P-NA---	Aspect=Imp|Mood=Ind|Negative=Neg|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	1	ccomp	_	SpaceAfter=No
7	.	.	PUNCT	Z:-------------	_	1	punct	_	_

""")

        self.assertFalse(tokenizer.nextSentence(sentence))

if __name__ == '__main__':
    unittest.main()
