use Test::More tests => 7;
use utf8;

use Ufal::UDPipe;

ok(my $model = Ufal::UDPipe::Model::load('t/data/test.model'));

my $tokenizer = $model->newTokenizer($Ufal::UDPipe::Model::DEFAULT);
my $conllu_output = Ufal::UDPipe::OutputFormat::newOutputFormat("conllu");
my $sentence = Ufal::UDPipe::Sentence->new();
my $error = Ufal::UDPipe::ProcessingError->new();

$tokenizer->setText("Znamená to, že realitě nepodléhá. ");
ok($tokenizer->nextSentence($sentence, $error));
ok(!$error->occurred());

ok($model->tag($sentence, $Ufal::UDPipe::Model::DEFAULT));
ok($model->parse($sentence, $Ufal::UDPipe::Model::DEFAULT));

is($conllu_output->writeSentence($sentence), <<EOF);
# newdoc
# newpar
# sent_id = 1
# text = Znamená to, že realitě nepodléhá.
1	Znamená	znamenat	VERB	VB-S---3P-AA---	Aspect=Imp|Mood=Ind|Negative=Pos|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	0	root	_	_
2	to	ten	PRON	PDNS1----------	Case=Nom|Gender=Neut|Number=Sing|PronType=Dem	1	nsubj	_	SpaceAfter=No
3	,	,	PUNCT	Z:-------------	_	6	punct	_	_
4	že	že	SCONJ	J,-------------	_	6	mark	_	_
5	realitě	realita	NOUN	NNFS3-----A----	Case=Dat|Gender=Fem|Negative=Pos|Number=Sing	6	dobj	_	_
6	nepodléhá	podléhat	VERB	VB-S---3P-NA---	Aspect=Imp|Mood=Ind|Negative=Neg|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin|Voice=Act	1	ccomp	_	SpaceAfter=No
7	.	.	PUNCT	Z:-------------	_	1	punct	_	_

EOF

ok(!$tokenizer->nextSentence($sentence));
