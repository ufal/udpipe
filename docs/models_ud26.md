## Universal Dependencies 2.6 Models #universal_dependencies_26_models

Universal Dependencies 2.6 Models are distributed under the
[CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The models are based solely on [Universal Dependencies
2.6](https://hdl.handle.net/11234/1-3226) treebanks, and additionally
use [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md).

The models require [UDPipe 2](https://ufal.mff.cuni.cz/udpipe/2).

### Download

The latest version 200831 of the Universal Dependencies 2.6 models
can be downloaded from [LINDAT/CLARIN repository](https://hdl.handle.net/11234/1-4798).

The models are also available in the [REST service](https://lindat.mff.cuni.cz/services/udpipe/).

### Acknowledgements #universal_dependencies_26_models_acknowledgements

This work has been supported by the Ministry of Education, Youth and Sports of
the Czech Republic, Project No. LM2018101 LINDAT/CLARIAH-CZ.

The models were trained on [Universal Dependencies 2.6](https://hdl.handle.net/11234/1-3226) treebanks.

For the UD treebanks which do not contain original plain text version,
raw text is used to train the tokenizer instead. The plain texts
were taken from the [W2C -- Web to Corpus](https://hdl.handle.net/11858/00-097C-0000-0022-6133-9).

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

| Model                   | Mode         |  Words |  Sents |  UPOS |  XPOS |  UFeats | AllTags |  Lemma |  UAS  |  LAS  |  MLAS |  BLEX |
|:------------------------|:-------------|-------:|-------:|------:|------:|--------:|--------:|-------:|------:|------:|------:|------:|
| afrikaans-afribooms-ud-2.6-200830        | Raw text          | 99.82  | 98.25  | 98.55 | 95.42 | 98.27 | 95.33 | 97.52 | 90.34 | 87.93 | 80.33 | 79.91 |
| afrikaans-afribooms-ud-2.6-200830        | Gold tokenization | —      | —      | 98.70 | 95.56 | 98.41 | 95.48 | 97.61 | 90.80 | 88.40 | 80.78 | 80.31 |
| ancient_greek-perseus-ud-2.6-200830      | Raw text          | 99.97  | 98.85  | 93.20 | 86.01 | 91.59 | 85.27 | 86.81 | 79.57 | 73.90 | 54.80 | 55.63 |
| ancient_greek-perseus-ud-2.6-200830      | Gold tokenization | —      | —      | 93.24 | 86.03 | 91.62 | 85.30 | 86.84 | 79.74 | 74.06 | 54.92 | 55.72 |
| ancient_greek-proiel-ud-2.6-200830       | Raw text          | 100.00 | 48.02  | 97.73 | 98.04 | 92.36 | 91.01 | 94.71 | 79.98 | 75.99 | 60.15 | 65.88 |
| ancient_greek-proiel-ud-2.6-200830       | Gold tokenization | —      | —      | 97.91 | 98.18 | 92.59 | 91.30 | 94.76 | 85.95 | 81.90 | 66.72 | 71.84 |
| arabic-padt-ud-2.6-200830                | Raw text          | 94.58  | 82.09  | 91.68 | 88.96 | 89.14 | 88.65 | 90.36 | 78.86 | 74.85 | 66.06 | 68.12 |
| arabic-padt-ud-2.6-200830                | Gold tokenization | —      | —      | 96.87 | 94.20 | 94.36 | 93.82 | 95.23 | 87.60 | 83.14 | 74.51 | 76.09 |
| armenian-armtdp-ud-2.6-200830            | Raw text          | 99.34  | 97.85  | 95.64 | —     | 90.30 | 88.94 | 94.45 | 85.07 | 79.97 | 66.54 | 71.61 |
| armenian-armtdp-ud-2.6-200830            | Gold tokenization | —      | —      | 96.11 | —     | 90.90 | 89.37 | 95.04 | 86.31 | 81.18 | 67.00 | 72.22 |
| basque-bdt-ud-2.6-200830                 | Raw text          | 99.94  | 99.83  | 96.44 | —     | 93.60 | 91.69 | 96.40 | 87.24 | 84.15 | 74.97 | 79.94 |
| basque-bdt-ud-2.6-200830                 | Gold tokenization | —      | —      | 96.48 | —     | 93.64 | 91.72 | 96.43 | 87.30 | 84.21 | 75.00 | 79.96 |
| belarusian-hse-ud-2.6-200830             | Raw text          | 99.84  | 78.70  | 96.14 | 31.78 | 82.07 | 26.98 | 81.48 | 75.81 | 71.05 | 49.78 | 50.93 |
| belarusian-hse-ud-2.6-200830             | Gold tokenization | —      | —      | 96.39 | 31.85 | 82.19 | 27.08 | 81.53 | 80.18 | 75.18 | 52.92 | 53.60 |
| bulgarian-btb-ud-2.6-200830              | Raw text          | 99.91  | 94.17  | 99.15 | 97.19 | 97.95 | 96.84 | 97.97 | 94.35 | 91.61 | 85.92 | 86.43 |
| bulgarian-btb-ud-2.6-200830              | Gold tokenization | —      | —      | 99.27 | 97.30 | 98.05 | 96.95 | 98.07 | 95.17 | 92.41 | 86.62 | 87.17 |
| catalan-ancora-ud-2.6-200830             | Raw text          | 99.98  | 99.43  | 99.05 | 98.99 | 98.63 | 98.14 | 99.31 | 94.53 | 92.86 | 87.63 | 89.24 |
| catalan-ancora-ud-2.6-200830             | Gold tokenization | —      | —      | 99.09 | 99.03 | 98.67 | 98.18 | 99.34 | 94.60 | 92.95 | 87.73 | 89.35 |
| chinese-gsdsimp-ud-2.6-200830            | Raw text          | 90.29  | 99.10  | 87.32 | 87.20 | 89.73 | 86.54 | 90.29 | 72.68 | 70.32 | 63.38 | 66.94 |
| chinese-gsdsimp-ud-2.6-200830            | Gold tokenization | —      | —      | 96.32 | 96.15 | 99.43 | 95.50 | 99.99 | 86.89 | 83.93 | 78.52 | 82.60 |
| chinese-gsd-ud-2.6-200830                | Raw text          | 90.27  | 99.10  | 87.27 | 87.18 | 89.74 | 86.50 | 90.27 | 72.99 | 70.50 | 63.83 | 67.21 |
| chinese-gsd-ud-2.6-200830                | Gold tokenization | —      | —      | 96.30 | 96.16 | 99.42 | 95.45 | 99.99 | 87.30 | 84.22 | 78.63 | 82.84 |
| classical_chinese-kyoto-ud-2.6-200830    | Raw text          | 99.46  | 46.22  | 90.91 | 90.91 | 93.43 | 88.00 | 99.42 | 72.75 | 67.18 | 63.67 | 66.02 |
| classical_chinese-kyoto-ud-2.6-200830    | Gold tokenization | —      | —      | 93.55 | 93.24 | 95.01 | 90.86 | 99.96 | 85.49 | 80.20 | 76.42 | 79.25 |
| coptic-scriptorium-ud-2.6-200830         | Raw text          | 71.91  | 35.97  | 69.61 | 68.00 | 63.06 | 60.16 | 70.51 | 47.75 | 45.89 | 25.42 | 35.81 |
| coptic-scriptorium-ud-2.6-200830         | Gold tokenization | —      | —      | 96.15 | 92.53 | 87.75 | 81.98 | 96.70 | 89.14 | 85.79 | 57.57 | 76.42 |
| croatian-set-ud-2.6-200830               | Raw text          | 99.95  | 94.41  | 98.18 | 95.91 | 96.40 | 95.27 | 97.58 | 92.20 | 88.40 | 80.16 | 83.07 |
| croatian-set-ud-2.6-200830               | Gold tokenization | —      | —      | 98.23 | 96.00 | 96.52 | 95.38 | 97.64 | 92.72 | 88.89 | 80.66 | 83.53 |
| czech-pdt-ud-2.6-200830                  | Raw text          | 99.93  | 93.35  | 99.23 | 97.61 | 97.59 | 97.13 | 99.09 | 93.81 | 92.03 | 87.79 | 89.88 |
| czech-pdt-ud-2.6-200830                  | Gold tokenization | —      | —      | 99.30 | 97.71 | 97.70 | 97.24 | 99.17 | 94.60 | 92.81 | 88.45 | 90.57 |
| czech-cac-ud-2.6-200830                  | Raw text          | 99.98  | 99.68  | 99.52 | 97.33 | 97.05 | 96.64 | 98.93 | 94.31 | 92.48 | 87.56 | 89.76 |
| czech-cac-ud-2.6-200830                  | Gold tokenization | —      | —      | 99.54 | 97.36 | 97.07 | 96.67 | 98.95 | 94.37 | 92.54 | 87.63 | 89.83 |
| czech-fictree-ud-2.6-200830              | Raw text          | 99.99  | 98.95  | 98.68 | 95.80 | 96.79 | 95.38 | 99.20 | 94.83 | 92.66 | 85.35 | 89.58 |
| czech-fictree-ud-2.6-200830              | Gold tokenization | —      | —      | 98.69 | 95.82 | 96.80 | 95.40 | 99.21 | 94.92 | 92.74 | 85.47 | 89.69 |
| czech-cltt-ud-2.6-200830                 | Raw text          | 99.65  | 97.40  | 99.21 | 95.00 | 94.98 | 94.76 | 99.06 | 91.37 | 89.67 | 82.08 | 86.96 |
| czech-cltt-ud-2.6-200830                 | Gold tokenization | —      | —      | 99.49 | 95.19 | 95.16 | 94.95 | 99.30 | 91.91 | 90.21 | 82.25 | 87.31 |
| danish-ddt-ud-2.6-200830                 | Raw text          | 99.81  | 89.78  | 98.01 | —     | 97.52 | 96.72 | 97.31 | 88.56 | 86.46 | 79.62 | 81.12 |
| danish-ddt-ud-2.6-200830                 | Gold tokenization | —      | —      | 98.26 | —     | 97.73 | 96.99 | 97.53 | 89.82 | 87.67 | 80.73 | 82.27 |
| dutch-alpino-ud-2.6-200830               | Raw text          | 99.83  | 88.59  | 97.41 | 95.98 | 97.02 | 95.36 | 97.32 | 92.79 | 90.38 | 81.53 | 83.18 |
| dutch-alpino-ud-2.6-200830               | Gold tokenization | —      | —      | 97.57 | 96.13 | 97.18 | 95.53 | 97.46 | 93.93 | 91.53 | 82.72 | 84.42 |
| dutch-lassysmall-ud-2.6-200830           | Raw text          | 99.83  | 75.40  | 96.58 | 95.42 | 96.41 | 94.73 | 97.21 | 90.36 | 87.66 | 78.84 | 80.17 |
| dutch-lassysmall-ud-2.6-200830           | Gold tokenization | —      | —      | 96.79 | 96.05 | 96.97 | 95.40 | 97.33 | 94.26 | 91.24 | 83.56 | 84.84 |
| english-ewt-ud-2.6-200830                | Raw text          | 98.95  | 86.60  | 96.36 | 96.06 | 96.56 | 94.88 | 97.64 | 89.55 | 87.43 | 80.50 | 83.29 |
| english-ewt-ud-2.6-200830                | Gold tokenization | —      | —      | 97.29 | 97.03 | 97.57 | 95.84 | 98.57 | 92.24 | 90.05 | 83.33 | 86.07 |
| english-gum-ud-2.6-200830                | Raw text          | 99.81  | 83.66  | 96.79 | 96.76 | 97.55 | 95.88 | 97.35 | 90.02 | 87.52 | 79.41 | 80.43 |
| english-gum-ud-2.6-200830                | Gold tokenization | —      | —      | 96.99 | 96.93 | 97.75 | 96.09 | 97.56 | 91.93 | 89.36 | 81.20 | 82.25 |
| english-lines-ud-2.6-200830              | Raw text          | 99.92  | 87.45  | 97.60 | 95.86 | 96.88 | 93.39 | 98.34 | 89.36 | 86.45 | 79.35 | 82.87 |
| english-lines-ud-2.6-200830              | Gold tokenization | —      | —      | 97.67 | 95.90 | 96.92 | 93.41 | 98.41 | 90.26 | 87.36 | 80.24 | 83.79 |
| english-partut-ud-2.6-200830             | Raw text          | 99.72  | 100.00 | 97.37 | 97.08 | 96.29 | 95.38 | 98.23 | 94.12 | 92.09 | 83.04 | 87.20 |
| english-partut-ud-2.6-200830             | Gold tokenization | —      | —      | 97.62 | 97.33 | 96.54 | 95.63 | 98.50 | 94.40 | 92.37 | 83.44 | 87.48 |
| estonian-edt-ud-2.6-200830               | Raw text          | 99.96  | 91.56  | 97.65 | 98.25 | 96.44 | 95.19 | 95.34 | 88.75 | 86.18 | 80.12 | 79.65 |
| estonian-edt-ud-2.6-200830               | Gold tokenization | —      | —      | 97.75 | 98.29 | 96.48 | 95.28 | 95.40 | 89.66 | 87.06 | 80.93 | 80.44 |
| estonian-ewt-ud-2.6-200830               | Raw text          | 98.96  | 70.09  | 95.00 | 96.30 | 93.74 | 91.31 | 93.81 | 81.07 | 77.55 | 69.14 | 70.69 |
| estonian-ewt-ud-2.6-200830               | Gold tokenization | —      | —      | 96.22 | 97.37 | 94.65 | 92.37 | 94.83 | 86.37 | 82.55 | 73.03 | 74.39 |
| finnish-tdt-ud-2.6-200830                | Raw text          | 99.70  | 88.64  | 97.63 | 98.25 | 96.05 | 95.11 | 92.06 | 90.11 | 88.10 | 82.04 | 77.91 |
| finnish-tdt-ud-2.6-200830                | Gold tokenization | —      | —      | 97.97 | 98.56 | 96.37 | 95.48 | 92.38 | 91.69 | 89.63 | 83.30 | 79.18 |
| finnish-ftb-ud-2.6-200830                | Raw text          | 99.91  | 86.84  | 96.52 | 95.08 | 96.72 | 93.82 | 95.73 | 89.93 | 87.32 | 80.13 | 80.74 |
| finnish-ftb-ud-2.6-200830                | Gold tokenization | —      | —      | 96.85 | 95.31 | 96.87 | 94.16 | 95.83 | 91.99 | 89.34 | 82.64 | 83.05 |
| french-gsd-ud-2.6-200830                 | Raw text          | 98.87  | 94.67  | 97.23 | 98.86 | 96.65 | 96.00 | 97.69 | 92.77 | 90.82 | 83.14 | 86.08 |
| french-gsd-ud-2.6-200830                 | Gold tokenization | —      | —      | 98.29 | 99.99 | 97.63 | 96.94 | 98.80 | 94.46 | 92.63 | 84.72 | 87.21 |
| french-sequoia-ud-2.6-200830             | Raw text          | 99.09  | 87.50  | 98.33 | —     | 97.25 | 96.79 | 98.16 | 93.90 | 92.45 | 86.54 | 89.25 |
| french-sequoia-ud-2.6-200830             | Gold tokenization | —      | —      | 99.32 | —     | 98.19 | 97.78 | 99.09 | 95.80 | 94.43 | 88.78 | 90.78 |
| french-partut-ud-2.6-200830              | Raw text          | 99.42  | 100.00 | 97.28 | 96.93 | 94.17 | 93.63 | 95.59 | 94.71 | 92.71 | 80.18 | 83.34 |
| french-partut-ud-2.6-200830              | Gold tokenization | —      | —      | 97.89 | 97.54 | 94.74 | 94.20 | 96.20 | 95.47 | 93.62 | 81.20 | 84.28 |
| french-spoken-ud-2.6-200830              | Raw text          | 99.06  | 21.15  | 96.49 | 96.44 | —     | 93.98 | 97.48 | 79.23 | 74.91 | 64.48 | 66.67 |
| french-spoken-ud-2.6-200830              | Gold tokenization | —      | —      | 97.63 | 97.31 | —     | 95.00 | 98.28 | 87.27 | 82.51 | 74.23 | 75.56 |
| galician-ctg-ud-2.6-200830               | Raw text          | 99.22  | 97.22  | 97.30 | 97.07 | 99.05 | 96.71 | 98.07 | 85.45 | 83.07 | 72.03 | 76.75 |
| galician-ctg-ud-2.6-200830               | Gold tokenization | —      | —      | 98.04 | 97.79 | 99.83 | 97.43 | 98.82 | 87.22 | 84.73 | 74.05 | 78.78 |
| galician-treegal-ud-2.6-200830           | Raw text          | 98.74  | 87.99  | 95.99 | 93.58 | 94.72 | 92.63 | 96.71 | 83.26 | 79.23 | 67.54 | 71.73 |
| galician-treegal-ud-2.6-200830           | Gold tokenization | —      | —      | 97.23 | 94.65 | 95.76 | 93.73 | 97.89 | 86.57 | 82.30 | 71.04 | 75.71 |
| german-hdt-ud-2.6-200830                 | Raw text          | 99.91  | 92.34  | 98.51 | 98.45 | 94.09 | 93.69 | 97.23 | 96.88 | 95.96 | 84.87 | 90.41 |
| german-hdt-ud-2.6-200830                 | Gold tokenization | —      | —      | 98.62 | 98.57 | 94.21 | 93.81 | 97.32 | 97.57 | 96.67 | 85.53 | 91.10 |
| german-gsd-ud-2.6-200830                 | Raw text          | 99.58  | 80.90  | 94.39 | 97.51 | 91.14 | 85.97 | 96.58 | 87.06 | 82.93 | 62.33 | 74.97 |
| german-gsd-ud-2.6-200830                 | Gold tokenization | —      | —      | 94.73 | 97.96 | 91.65 | 86.51 | 96.95 | 89.36 | 85.31 | 64.33 | 77.26 |
| gothic-proiel-ud-2.6-200830              | Raw text          | 100.00 | 31.12  | 96.39 | 96.90 | 90.18 | 88.05 | 94.70 | 74.10 | 68.48 | 55.16 | 62.26 |
| gothic-proiel-ud-2.6-200830              | Gold tokenization | —      | —      | 96.81 | 97.26 | 91.12 | 89.28 | 94.77 | 83.73 | 77.93 | 65.37 | 70.85 |
| greek-gdt-ud-2.6-200830                  | Raw text          | 99.87  | 90.19  | 97.99 | 98.00 | 95.57 | 94.91 | 95.53 | 93.00 | 91.16 | 81.28 | 80.73 |
| greek-gdt-ud-2.6-200830                  | Gold tokenization | —      | —      | 98.14 | 98.14 | 95.69 | 95.02 | 95.61 | 93.82 | 91.95 | 82.03 | 81.53 |
| hebrew-htb-ud-2.6-200830                 | Raw text          | 85.04  | 99.39  | 82.79 | 82.76 | 81.31 | 80.57 | 82.97 | 69.85 | 67.39 | 54.79 | 59.16 |
| hebrew-htb-ud-2.6-200830                 | Gold tokenization | —      | —      | 97.48 | 97.48 | 96.03 | 95.36 | 97.23 | 91.83 | 89.25 | 78.52 | 81.02 |
| hindi-hdtb-ud-2.6-200830                 | Raw text          | 100.00 | 98.90  | 97.64 | 97.29 | 94.18 | 92.32 | 98.78 | 95.32 | 92.37 | 79.24 | 87.69 |
| hindi-hdtb-ud-2.6-200830                 | Gold tokenization | —      | —      | 97.65 | 97.29 | 94.21 | 92.35 | 98.78 | 95.44 | 92.49 | 79.41 | 87.84 |
| hungarian-szeged-ud-2.6-200830           | Raw text          | 99.85  | 95.89  | 96.77 | —     | 94.32 | 93.51 | 94.97 | 87.78 | 84.24 | 74.80 | 77.84 |
| hungarian-szeged-ud-2.6-200830           | Gold tokenization | —      | —      | 96.87 | —     | 94.45 | 93.61 | 95.09 | 88.28 | 84.73 | 75.27 | 78.26 |
| indonesian-gsd-ud-2.6-200830             | Raw text          | 100.00 | 94.13  | 93.89 | 94.28 | 95.55 | 89.00 | 99.61 | 86.07 | 79.97 | 69.25 | 77.74 |
| indonesian-gsd-ud-2.6-200830             | Gold tokenization | —      | —      | 93.90 | 94.26 | 95.52 | 88.98 | 99.61 | 86.32 | 80.18 | 69.51 | 78.00 |
| irish-idt-ud-2.6-200830                  | Raw text          | 99.71  | 97.36  | 94.35 | 94.30 | 73.43 | 70.38 | 93.18 | 84.47 | 77.88 | 40.78 | 65.74 |
| irish-idt-ud-2.6-200830                  | Gold tokenization | —      | —      | 94.59 | 94.60 | 73.65 | 70.63 | 93.41 | 84.98 | 78.30 | 40.94 | 65.87 |
| italian-isdt-ud-2.6-200830               | Raw text          | 99.84  | 98.76  | 98.52 | 98.44 | 98.23 | 97.66 | 98.65 | 94.77 | 93.12 | 86.91 | 87.85 |
| italian-isdt-ud-2.6-200830               | Gold tokenization | —      | —      | 98.68 | 98.60 | 98.38 | 97.81 | 98.81 | 95.07 | 93.44 | 87.20 | 88.19 |
| italian-partut-ud-2.6-200830             | Raw text          | 99.73  | 100.00 | 98.41 | 98.52 | 98.27 | 97.77 | 98.74 | 96.07 | 93.90 | 87.45 | 88.95 |
| italian-partut-ud-2.6-200830             | Gold tokenization | —      | —      | 98.54 | 98.65 | 98.38 | 97.88 | 98.93 | 96.18 | 93.98 | 87.48 | 89.15 |
| italian-postwita-ud-2.6-200830           | Raw text          | 99.47  | 30.49  | 96.53 | 96.28 | 96.43 | 94.89 | 96.76 | 80.97 | 76.94 | 65.79 | 67.44 |
| italian-postwita-ud-2.6-200830           | Gold tokenization | —      | —      | 97.06 | 96.79 | 96.89 | 95.41 | 97.18 | 88.04 | 83.76 | 75.23 | 76.98 |
| italian-twittiro-ud-2.6-200830           | Raw text          | 99.06  | 36.80  | 95.99 | 95.86 | 95.22 | 93.37 | 94.68 | 81.69 | 77.38 | 64.34 | 65.32 |
| italian-twittiro-ud-2.6-200830           | Gold tokenization | —      | —      | 97.01 | 96.77 | 96.14 | 94.42 | 95.50 | 87.84 | 83.43 | 71.64 | 72.68 |
| italian-vit-ud-2.6-200830                | Raw text          | 99.69  | 94.69  | 97.86 | 97.07 | 97.38 | 95.76 | 98.64 | 92.03 | 89.20 | 80.39 | 83.83 |
| italian-vit-ud-2.6-200830                | Gold tokenization | —      | —      | 98.16 | 97.49 | 97.66 | 96.16 | 98.92 | 92.77 | 89.91 | 81.15 | 84.53 |
| japanese-gsd-ud-2.6-200830               | Raw text          | 95.34  | 94.61  | 93.67 | 93.56 | 95.32 | 92.74 | 95.02 | 85.11 | 84.01 | 76.23 | 77.83 |
| japanese-gsd-ud-2.6-200830               | Gold tokenization | —      | —      | 98.03 | 97.71 | 99.99 | 96.83 | 99.61 | 94.73 | 93.41 | 87.64 | 89.28 |
| korean-kaist-ud-2.6-200830               | Raw text          | 99.95  | 100.00 | 95.89 | 87.82 | —     | 87.62 | 94.23 | 89.41 | 87.58 | 82.32 | 80.34 |
| korean-kaist-ud-2.6-200830               | Gold tokenization | —      | —      | 95.94 | 87.85 | —     | 87.66 | 94.27 | 89.51 | 87.67 | 82.42 | 80.42 |
| korean-gsd-ud-2.6-200830                 | Raw text          | 99.87  | 93.93  | 96.61 | 90.19 | 99.69 | 88.03 | 93.51 | 88.68 | 85.04 | 80.93 | 77.36 |
| korean-gsd-ud-2.6-200830                 | Gold tokenization | —      | —      | 96.74 | 90.32 | 99.82 | 88.16 | 93.64 | 89.50 | 85.84 | 81.76 | 78.14 |
| latin-ittb-ud-2.6-200830                 | Raw text          | 99.99  | 92.44  | 98.54 | 96.35 | 96.92 | 95.12 | 98.94 | 90.31 | 88.16 | 82.19 | 85.37 |
| latin-ittb-ud-2.6-200830                 | Gold tokenization | —      | —      | 98.52 | 96.37 | 96.92 | 95.11 | 98.93 | 91.24 | 89.07 | 82.62 | 85.88 |
| latin-llct-ud-2.6-200830                 | Raw text          | 100.00 | 99.49  | 99.60 | 97.13 | 97.11 | 96.63 | 97.68 | 95.48 | 94.35 | 89.31 | 90.44 |
| latin-llct-ud-2.6-200830                 | Gold tokenization | —      | —      | 99.60 | 97.14 | 97.11 | 96.63 | 97.68 | 95.54 | 94.40 | 89.40 | 90.53 |
| latin-proiel-ud-2.6-200830               | Raw text          | 99.87  | 36.81  | 96.67 | 96.81 | 90.71 | 89.59 | 96.16 | 74.44 | 69.97 | 57.51 | 64.96 |
| latin-proiel-ud-2.6-200830               | Gold tokenization | —      | —      | 97.07 | 97.16 | 91.53 | 90.52 | 96.42 | 83.78 | 79.04 | 67.58 | 73.88 |
| latin-perseus-ud-2.6-200830              | Raw text          | 100.00 | 98.46  | 91.65 | 81.18 | 86.33 | 78.75 | 88.05 | 78.09 | 68.97 | 52.82 | 56.03 |
| latin-perseus-ud-2.6-200830              | Gold tokenization | —      | —      | 91.64 | 81.17 | 86.33 | 78.74 | 88.04 | 78.21 | 69.07 | 52.84 | 55.99 |
| latvian-lvtb-ud-2.6-200830               | Raw text          | 99.32  | 98.74  | 96.28 | 89.64 | 93.79 | 88.84 | 95.81 | 88.31 | 85.26 | 75.23 | 79.56 |
| latvian-lvtb-ud-2.6-200830               | Gold tokenization | —      | —      | 96.92 | 90.24 | 94.40 | 89.43 | 96.45 | 89.33 | 86.23 | 76.29 | 80.60 |
| lithuanian-alksnis-ud-2.6-200830         | Raw text          | 99.91  | 87.87  | 95.97 | 90.37 | 91.07 | 89.41 | 93.61 | 82.54 | 78.70 | 67.95 | 71.30 |
| lithuanian-alksnis-ud-2.6-200830         | Gold tokenization | —      | —      | 96.04 | 90.40 | 91.18 | 89.49 | 93.70 | 83.93 | 80.08 | 69.02 | 72.43 |
| lithuanian-hse-ud-2.6-200830             | Raw text          | 97.30  | 97.30  | 89.66 | 89.28 | 81.45 | 77.07 | 87.98 | 70.92 | 62.53 | 44.26 | 53.76 |
| lithuanian-hse-ud-2.6-200830             | Gold tokenization | —      | —      | 91.23 | 91.32 | 83.21 | 78.40 | 90.28 | 73.77 | 64.53 | 45.25 | 54.68 |
| maltese-mudt-ud-2.6-200830               | Raw text          | 99.84  | 86.29  | 95.77 | 95.66 | —     | 95.30 | —     | 84.76 | 79.76 | 68.39 | 72.24 |
| maltese-mudt-ud-2.6-200830               | Gold tokenization | —      | —      | 95.88 | 95.77 | —     | 95.40 | —     | 85.46 | 80.38 | 68.69 | 72.66 |
| marathi-ufal-ud-2.6-200830               | Raw text          | 90.25  | 92.63  | 78.50 | —     | 65.25 | 61.50 | 80.00 | 61.25 | 53.50 | 31.73 | 40.92 |
| marathi-ufal-ud-2.6-200830               | Gold tokenization | —      | —      | 84.22 | —     | 68.69 | 63.83 | 80.10 | 70.39 | 60.92 | 31.95 | 42.32 |
| naija-nsc-ud-2.6-200830                  | Raw text          | 100.00 | 99.56  | 98.14 | —     | 99.16 | 97.77 | 99.27 | 92.46 | 89.81 | 84.18 | 86.20 |
| naija-nsc-ud-2.6-200830                  | Gold tokenization | —      | —      | 98.14 | —     | 99.16 | 97.77 | 99.27 | 92.50 | 89.84 | 84.25 | 86.26 |
| north_sami-giella-ud-2.6-200830          | Raw text          | 99.87  | 98.79  | 92.35 | 93.57 | 89.40 | 85.61 | 86.85 | 76.66 | 71.84 | 60.71 | 58.95 |
| north_sami-giella-ud-2.6-200830          | Gold tokenization | —      | —      | 92.47 | 93.70 | 89.56 | 85.75 | 86.96 | 76.97 | 72.16 | 60.95 | 59.19 |
| norwegian-bokmaal-ud-2.6-200830          | Raw text          | 99.83  | 95.63  | 98.37 | —     | 97.52 | 96.86 | 98.55 | 93.74 | 92.26 | 87.03 | 88.76 |
| norwegian-bokmaal-ud-2.6-200830          | Gold tokenization | —      | —      | 98.57 | —     | 97.71 | 97.05 | 98.75 | 94.48 | 93.00 | 87.67 | 89.43 |
| norwegian-nynorsk-ud-2.6-200830          | Raw text          | 99.91  | 94.11  | 98.36 | —     | 97.38 | 96.67 | 98.37 | 93.86 | 92.11 | 86.07 | 88.11 |
| norwegian-nynorsk-ud-2.6-200830          | Gold tokenization | —      | —      | 98.50 | —     | 97.51 | 96.80 | 98.50 | 94.66 | 92.93 | 86.95 | 89.01 |
| norwegian-nynorsklia-ud-2.6-200830       | Raw text          | 99.91  | 99.53  | 96.45 | —     | 95.71 | 93.62 | 98.05 | 80.90 | 76.53 | 65.74 | 69.55 |
| norwegian-nynorsklia-ud-2.6-200830       | Gold tokenization | —      | —      | 96.55 | —     | 95.79 | 93.72 | 98.14 | 81.15 | 76.76 | 65.94 | 69.80 |
| old_church_slavonic-proiel-ud-2.6-200830 | Raw text          | 100.00 | 41.43  | 96.58 | 96.83 | 90.44 | 89.17 | 93.19 | 77.42 | 73.57 | 63.51 | 68.53 |
| old_church_slavonic-proiel-ud-2.6-200830 | Gold tokenization | —      | —      | 96.89 | 97.09 | 91.22 | 89.97 | 93.20 | 87.95 | 83.81 | 73.92 | 77.26 |
| old_french-srcmf-ud-2.6-200830           | Raw text          | 99.93  | 100.00 | 96.40 | 96.27 | 97.80 | 95.58 | —     | 92.28 | 87.74 | 81.08 | 84.17 |
| old_french-srcmf-ud-2.6-200830           | Gold tokenization | —      | —      | 96.47 | 96.33 | 97.86 | 95.64 | —     | 92.36 | 87.81 | 81.17 | 84.26 |
| old_russian-torot-ud-2.6-200830          | Raw text          | 100.00 | 29.60  | 94.33 | 94.39 | 87.51 | 85.16 | 85.82 | 70.66 | 65.18 | 51.26 | 53.18 |
| old_russian-torot-ud-2.6-200830          | Gold tokenization | —      | —      | 94.93 | 94.99 | 88.44 | 86.35 | 85.77 | 83.15 | 77.17 | 63.78 | 62.66 |
| old_russian-rnc-ud-2.6-200830            | Raw text          | 98.15  | 85.46  | 91.80 | 87.74 | 75.83 | 66.63 | 74.94 | 63.08 | 57.53 | 33.85 | 35.04 |
| old_russian-rnc-ud-2.6-200830            | Gold tokenization | —      | —      | 93.34 | 89.43 | 77.09 | 67.76 | 76.13 | 66.86 | 60.73 | 36.05 | 37.07 |
| persian-seraji-ud-2.6-200830             | Raw text          | 99.65  | 98.75  | 97.69 | 97.66 | 97.75 | 97.29 | 96.67 | 91.09 | 88.15 | 83.43 | 82.26 |
| persian-seraji-ud-2.6-200830             | Gold tokenization | —      | —      | 97.98 | 97.97 | 98.07 | 97.60 | 96.94 | 91.74 | 88.76 | 84.00 | 82.82 |
| polish-pdb-ud-2.6-200830                 | Raw text          | 99.85  | 97.33  | 98.88 | 95.73 | 95.84 | 95.03 | 98.05 | 94.02 | 92.01 | 84.93 | 88.08 |
| polish-pdb-ud-2.6-200830                 | Gold tokenization | —      | —      | 99.04 | 95.88 | 95.99 | 95.18 | 98.19 | 94.51 | 92.50 | 85.36 | 88.52 |
| polish-lfg-ud-2.6-200830                 | Raw text          | 99.85  | 99.65  | 98.92 | 95.99 | 96.51 | 95.06 | 98.27 | 96.89 | 95.52 | 89.73 | 92.45 |
| polish-lfg-ud-2.6-200830                 | Gold tokenization | —      | —      | 99.09 | 96.18 | 96.70 | 95.25 | 98.41 | 97.29 | 95.91 | 90.12 | 92.77 |
| portuguese-gsd-ud-2.6-200830             | Raw text          | 99.84  | 97.50  | 98.53 | 98.52 | 99.71 | 98.43 | 99.33 | 94.57 | 93.47 | 88.69 | 90.02 |
| portuguese-gsd-ud-2.6-200830             | Gold tokenization | —      | —      | 98.69 | 98.69 | 99.87 | 98.59 | 99.49 | 94.94 | 93.82 | 89.11 | 90.36 |
| portuguese-bosque-ud-2.6-200830          | Raw text          | 99.55  | 90.64  | 97.19 | —     | 96.17 | 94.79 | 97.98 | 92.32 | 89.72 | 79.29 | 84.22 |
| portuguese-bosque-ud-2.6-200830          | Gold tokenization | —      | —      | 97.60 | —     | 96.49 | 95.11 | 98.42 | 93.53 | 90.80 | 80.42 | 85.51 |
| romanian-rrt-ud-2.6-200830               | Raw text          | 99.69  | 95.28  | 97.79 | 97.18 | 97.32 | 96.81 | 98.20 | 91.83 | 87.56 | 80.00 | 82.17 |
| romanian-rrt-ud-2.6-200830               | Gold tokenization | —      | —      | 98.08 | 97.44 | 97.60 | 97.08 | 98.49 | 92.74 | 88.38 | 80.82 | 82.88 |
| romanian-nonstandard-ud-2.6-200830       | Raw text          | 98.35  | 96.73  | 95.61 | 91.38 | 90.03 | 88.67 | 94.23 | 88.89 | 84.47 | 67.59 | 75.81 |
| romanian-nonstandard-ud-2.6-200830       | Gold tokenization | —      | —      | 97.21 | 92.90 | 91.53 | 90.13 | 95.74 | 91.00 | 86.49 | 69.53 | 77.28 |
| russian-syntagrus-ud-2.6-200830          | Raw text          | 99.60  | 98.80  | 98.86 | —     | 97.60 | 97.38 | 98.33 | 94.22 | 92.97 | 89.27 | 90.35 |
| russian-syntagrus-ud-2.6-200830          | Gold tokenization | —      | —      | 99.27 | —     | 97.98 | 97.76 | 98.68 | 94.99 | 93.72 | 89.90 | 90.95 |
| russian-gsd-ud-2.6-200830                | Raw text          | 99.50  | 96.22  | 98.03 | 97.51 | 94.76 | 93.60 | 96.89 | 91.66 | 88.38 | 80.67 | 84.18 |
| russian-gsd-ud-2.6-200830                | Gold tokenization | —      | —      | 98.49 | 97.98 | 95.17 | 93.97 | 97.27 | 92.77 | 89.43 | 81.44 | 85.05 |
| russian-taiga-ud-2.6-200830              | Raw text          | 97.16  | 82.69  | 94.13 | 95.72 | 90.01 | 87.50 | 93.05 | 81.17 | 76.99 | 65.28 | 69.94 |
| russian-taiga-ud-2.6-200830              | Gold tokenization | —      | —      | 96.47 | 98.56 | 92.72 | 89.87 | 95.68 | 85.57 | 80.81 | 68.93 | 73.90 |
| sanskrit-vedic-ud-2.6-200830             | Raw text          | 100.00 | 27.18  | 89.50 | —     | 81.72 | 77.12 | 87.11 | 60.79 | 49.75 | 41.65 | 44.67 |
| sanskrit-vedic-ud-2.6-200830             | Gold tokenization | —      | —      | 90.01 | —     | 83.11 | 78.58 | 87.24 | 73.34 | 61.55 | 51.87 | 54.91 |
| scottish_gaelic-arcosg-ud-2.6-200830     | Raw text          | 99.58  | 55.57  | 93.63 | 87.07 | 89.78 | 85.43 | 95.41 | 77.66 | 71.86 | 55.15 | 60.51 |
| scottish_gaelic-arcosg-ud-2.6-200830     | Gold tokenization | —      | —      | 94.26 | 87.84 | 90.23 | 86.30 | 95.85 | 83.77 | 77.61 | 62.05 | 68.26 |
| serbian-set-ud-2.6-200830                | Raw text          | 99.99  | 93.00  | 98.98 | 95.75 | 95.92 | 95.35 | 97.82 | 93.66 | 91.18 | 83.18 | 86.80 |
| serbian-set-ud-2.6-200830                | Gold tokenization | —      | —      | 99.01 | 95.78 | 95.94 | 95.39 | 97.83 | 94.33 | 91.82 | 83.84 | 87.45 |
| slovak-snk-ud-2.6-200830                 | Raw text          | 100.00 | 85.28  | 97.19 | 87.79 | 92.66 | 86.71 | 96.52 | 91.71 | 89.60 | 78.75 | 84.54 |
| slovak-snk-ud-2.6-200830                 | Gold tokenization | —      | —      | 97.30 | 88.06 | 92.84 | 86.98 | 96.60 | 93.68 | 91.57 | 80.55 | 86.59 |
| slovenian-ssj-ud-2.6-200830              | Raw text          | 97.99  | 67.98  | 96.93 | 94.35 | 94.56 | 93.95 | 96.59 | 88.09 | 86.65 | 80.90 | 83.58 |
| slovenian-ssj-ud-2.6-200830              | Gold tokenization | —      | —      | 98.86 | 96.44 | 96.69 | 96.01 | 98.54 | 94.41 | 92.96 | 86.68 | 89.30 |
| slovenian-sst-ud-2.6-200830              | Raw text          | 99.85  | 23.14  | 94.70 | 92.70 | 92.52 | 89.74 | 97.14 | 64.23 | 59.57 | 49.25 | 52.95 |
| slovenian-sst-ud-2.6-200830              | Gold tokenization | —      | —      | 95.71 | 93.11 | 92.94 | 90.90 | 97.46 | 77.81 | 72.24 | 62.71 | 67.18 |
| spanish-ancora-ud-2.6-200830             | Raw text          | 99.95  | 98.32  | 99.09 | 99.02 | 98.87 | 98.33 | 99.36 | 93.62 | 91.78 | 86.82 | 88.06 |
| spanish-ancora-ud-2.6-200830             | Gold tokenization | —      | —      | 99.14 | 99.06 | 98.91 | 98.37 | 99.40 | 93.83 | 91.97 | 87.01 | 88.24 |
| spanish-gsd-ud-2.6-200830                | Raw text          | 99.76  | 94.54  | 97.17 | —     | 97.05 | 95.32 | 98.80 | 92.00 | 89.70 | 79.23 | 84.49 |
| spanish-gsd-ud-2.6-200830                | Gold tokenization | —      | —      | 97.40 | —     | 97.27 | 95.54 | 99.02 | 92.73 | 90.38 | 79.93 | 85.13 |
| swedish-talbanken-ud-2.6-200830          | Raw text          | 99.89  | 96.13  | 98.41 | 97.26 | 97.33 | 96.46 | 98.19 | 91.99 | 89.68 | 83.63 | 85.82 |
| swedish-talbanken-ud-2.6-200830          | Gold tokenization | —      | —      | 98.51 | 97.38 | 97.44 | 96.57 | 98.30 | 92.46 | 90.14 | 84.12 | 86.33 |
| swedish-lines-ud-2.6-200830              | Raw text          | 99.96  | 87.20  | 97.71 | 95.47 | 90.89 | 88.10 | 97.76 | 89.14 | 85.80 | 71.44 | 81.67 |
| swedish-lines-ud-2.6-200830              | Gold tokenization | —      | —      | 97.75 | 95.48 | 90.91 | 88.09 | 97.79 | 89.91 | 86.52 | 72.13 | 82.48 |
| tamil-ttb-ud-2.6-200830                  | Raw text          | 94.51  | 97.52  | 88.39 | 82.92 | 85.30 | 82.11 | 89.15 | 70.28 | 64.91 | 54.93 | 58.46 |
| tamil-ttb-ud-2.6-200830                  | Gold tokenization | —      | —      | 93.36 | 87.28 | 90.10 | 86.22 | 93.97 | 78.03 | 71.79 | 61.09 | 64.80 |
| telugu-mtg-ud-2.6-200830                 | Raw text          | 99.58  | 96.62  | 93.63 | 93.63 | 98.48 | 93.63 | —     | 90.17 | 83.52 | 76.00 | 79.62 |
| telugu-mtg-ud-2.6-200830                 | Gold tokenization | —      | —      | 94.04 | 94.04 | 98.89 | 94.04 | —     | 91.12 | 84.47 | 76.84 | 80.46 |
| turkish-imst-ud-2.6-200830               | Raw text          | 98.30  | 96.97  | 94.48 | 93.69 | 92.06 | 89.95 | 94.41 | 72.63 | 66.80 | 58.31 | 61.57 |
| turkish-imst-ud-2.6-200830               | Gold tokenization | —      | —      | 96.10 | 95.32 | 93.66 | 91.50 | 96.00 | 76.10 | 69.93 | 60.33 | 63.83 |
| ukrainian-iu-ud-2.6-200830               | Raw text          | 99.81  | 96.61  | 97.89 | 94.22 | 94.18 | 93.13 | 97.39 | 90.59 | 88.24 | 78.76 | 83.19 |
| ukrainian-iu-ud-2.6-200830               | Gold tokenization | —      | —      | 98.10 | 94.42 | 94.34 | 93.30 | 97.56 | 91.11 | 88.75 | 79.11 | 83.59 |
| urdu-udtb-ud-2.6-200830                  | Raw text          | 100.00 | 98.31  | 94.10 | 92.27 | 82.89 | 78.41 | 97.38 | 88.27 | 82.63 | 56.79 | 74.77 |
| urdu-udtb-ud-2.6-200830                  | Gold tokenization | —      | —      | 94.08 | 92.26 | 82.92 | 78.43 | 97.39 | 88.37 | 82.74 | 56.90 | 74.92 |
| uyghur-udt-ud-2.6-200830                 | Raw text          | 99.54  | 81.81  | 89.24 | 91.70 | 88.47 | 80.04 | 94.76 | 76.58 | 64.72 | 46.67 | 55.08 |
| uyghur-udt-ud-2.6-200830                 | Gold tokenization | —      | —      | 89.67 | 92.21 | 88.92 | 80.47 | 95.27 | 78.39 | 66.27 | 47.53 | 56.23 |
| vietnamese-vtb-ud-2.6-200830             | Raw text          | 85.37  | 93.46  | 78.19 | 76.69 | 85.11 | 76.53 | 85.15 | 52.80 | 47.90 | 41.56 | 44.31 |
| vietnamese-vtb-ud-2.6-200830             | Gold tokenization | —      | —      | 90.56 | 88.69 | 99.72 | 88.47 | 99.58 | 72.63 | 65.26 | 58.85 | 62.42 |
| welsh-ccg-ud-2.6-200830                  | Raw text          | 99.42  | 96.28  | 94.02 | 92.96 | 89.04 | 86.39 | 92.88 | 85.79 | 79.16 | 60.98 | 66.81 |
| welsh-ccg-ud-2.6-200830                  | Gold tokenization | —      | —      | 94.54 | 93.51 | 89.52 | 86.83 | 93.46 | 87.04 | 80.35 | 62.14 | 68.05 |
| wolof-wtb-ud-2.6-200830                  | Raw text          | 99.23  | 91.95  | 94.25 | 94.12 | 93.37 | 91.19 | 95.22 | 83.79 | 78.59 | 66.50 | 70.09 |
| wolof-wtb-ud-2.6-200830                  | Gold tokenization | —      | —      | 95.19 | 95.03 | 94.22 | 92.10 | 95.97 | 85.98 | 80.75 | 68.61 | 72.08 |
