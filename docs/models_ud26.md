## Universal Dependencies 2.6 Models #universal_dependencies_26_models

Universal Dependencies 2.6 Models are distributed under the
[CC BY-NC-SA](http://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The models are based solely on [Universal Dependencies
2.6](http://hdl.handle.net/11234/1-3226) treebanks, and additionally
use [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md).

### Download

The latest version 200831 of the Universal Dependencies 2.6 models is currently
available only in the [REST service](http://lindat.mff.cuni.cz/services/udpipe/),
but the models will become downloadable soon.

### Acknowledgements #universal_dependencies_26_models_acknowledgements

This work has been supported by the Ministry of Education, Youth and Sports of
the Czech Republic, Project No. LM2018101 LINDAT/CLARIAH-CZ.

The models were trained on [Universal Dependencies 2.6 http://hdl.handle.net/11234/1-3226] treebanks.

For the UD treebanks which do not contain original plain text version,
raw text is used to train the tokenizer instead. The plain texts
were taken from the [W2C -- Web to Corpus http://hdl.handle.net/11858/00-097C-0000-0022-6133-9].

Finally, [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md)
is used to provide contextualized word embeddings.

### Publications

- Milan Straka, Jana Straková, Jan Hajič (2019): [Evaluating Contextualized Embeddings on 54 Languages in POS Tagging, Lemmatization and Dependency Parsing](https://arxiv.org/abs/1908.07448). In: ArXiv.org Computing Research Repository, ISSN 2331-8422, 1904.02099
- Milan Straka (2018): [UDPipe 2.0 Prototype at CoNLL 2018 UD Shared Task](https://www.aclweb.org/anthology/K18-2020/). In: Proceedings of CoNLL 2018: The SIGNLL Conference on Computational Natural Language Learning, pp. 197-207, Association for Computational Linguistics, Stroudsburg, PA, USA, ISBN 978-1-948087-72-8

### Model Description

The Universal Dependencies 2.6 models contain 99 models of 63 languages, each consisting of
a tokenizer, tagger, lemmatizer and dependency parser, all trained using
the UD data. We used the original train-dev-test split, but for treebanks with
only train and no dev data we used last 10% of the train data as dev data.
We produce models only for treebanks with at least 1000 training words.

The tokenizer is trained using the `SpaceAfter=No` features. If the features
are not present in the data, they can be filled in using raw text in the
language in question.

The tagger, lemmatizer and parser are trained using gold UD data.

### Model Performance

We present the tokenizer, tagger, lemmatizer and parser performance, measured on
the testing portion of the data, evaluated both on the raw text and using the
gold tokenization. The results are F1 scores measured by the `conll18_ud_eval.py`
script.
