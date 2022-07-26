## EvaLatin 2020 Models #evalatin20_models

EvaLatin 2020 Models are distributed under the
[CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The models are based solely on [EvaLatin 2020](https://github.com/CIRCSE/LT4HALA)
treebanks, and additionally use [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md).

The models require [UDPipe 2](https://ufal.mff.cuni.cz/udpipe/2).

### Download

The latest version 200831 of the EvaLatin 2020 models
can be downloaded from [LINDAT/CLARIN repository](https://hdl.handle.net/11234/1-4803).

The models are also available in the [REST service](https://lindat.mff.cuni.cz/services/udpipe/).

### Acknowledgements #evalatin20_models_acknowledgements

This work was supported by the grant no. GX20-16819X of the Grant Agency of the
Czech Republic, and has been using language resources stored and distributed by
the LINDAT/CLARIAH-CZ project of the Ministry of Education, Youth and Sports of
the Czech Republic (project LM2018101).

The models were trained on [EvaLatin 2020](https://github.com/CIRCSE/LT4HALA) treebanks.

Finally, [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md)
is used to provide contextualized word embeddings.

### Publications

- Milan Straka, Jana Straková (2020): [UDPipe at EvaLatin 2020: Contextualized Embeddings
and Treebank Embeddings](https://arxiv.org/abs/2006.03687). In: ArXiv.org Computing Research Repository, ISSN 2331-8422, 2006.03687

### Model Performance

| Model | Dataset           |  UPOS |  Lemma |
|:------|:------------------|------:|-------:|
| latin-evalatin20-200830 | test classical    | 96.73 |  96.39 |
| latin-evalatin20-200830 | test cross-genre  | 90.47 |  86.89 |
| latin-evalatin20-200830 | test cross-time   | 87.58 |  90.59 |
