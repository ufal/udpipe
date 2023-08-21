## Universal Dependencies 2.10 Models #universal_dependencies_210_models

Universal Dependencies 2.10 Models are distributed under the
[CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The models are based solely on [Universal Dependencies
2.10](https://hdl.handle.net/11234/1-4758) treebanks, and additionally
use [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md)
and [RobeCzech](https://huggingface.co/ufal/robeczech-base).

The models require [UDPipe 2](https://ufal.mff.cuni.cz/udpipe/2).

### Download

The latest version 220711 of the Universal Dependencies 2.10 models
can be downloaded from [LINDAT/CLARIN repository](https://hdl.handle.net/11234/1-4804).

The models are also available in the [REST service](https://lindat.mff.cuni.cz/services/udpipe/).

### Acknowledgements #universal_dependencies_210_models_acknowledgements

This work has been supported by the Ministry of Education, Youth and Sports of
the Czech Republic, Project No. LM2018101 LINDAT/CLARIAH-CZ.

The models were trained on [Universal Dependencies 2.10](https://hdl.handle.net/11234/1-4758) treebanks.

For the UD treebanks which do not contain original plain text version,
raw text is used to train the tokenizer instead. The plain texts
were taken from the [W2C -- Web to Corpus](https://hdl.handle.net/11858/00-097C-0000-0022-6133-9).

Finally, [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md)
and [RobeCzech](https://huggingface.co/ufal/robeczech-base) are used to provide
contextualized word embeddings.

### Publications

- Milan Straka, Jakub Náplava, Jana Straková, David Samuel (2020): [RobeCzech: Czech RoBERTa, a monolingual contextualized language representation model](https://doi.org/10.1007/978-3-030-83527-9_17). In: Text, Speech, and Dialogue. TSD 2021. Lecture Notes in Computer Science, vol 12848. Springer, Cham.
- Milan Straka, Jana Straková, Jan Hajič (2019): [Evaluating Contextualized Embeddings on 54 Languages in POS Tagging, Lemmatization and Dependency Parsing](https://arxiv.org/abs/1908.07448). In: ArXiv.org Computing Research Repository, ISSN 2331-8422, 1904.02099
- Milan Straka (2018): [UDPipe 2.0 Prototype at CoNLL 2018 UD Shared Task](https://www.aclweb.org/anthology/K18-2020/). In: Proceedings of CoNLL 2018: The SIGNLL Conference on Computational Natural Language Learning, pp. 197-207, Association for Computational Linguistics, Stroudsburg, PA, USA, ISBN 978-1-948087-72-8

### Model Description

The Universal Dependencies 2.10 models contain 123 models of 69 languages, each
consisting of a tokenizer, tagger, lemmatizer and dependency parser, all trained
using the UD data. We used the original train-dev-test split, but for treebanks
with only train and no dev data we used last 10% of the train data as dev data.
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

| Model                                     | Mode              | Words  | Sents  | UPOS  | XPOS  | UFeats | AllTags | Lemma  | UAS   | LAS   | MLAS  | BLEX  |
|:------------------------------------------|:------------------|-------:|-------:|------:|------:|-------:|--------:|-------:|------:|------:|------:|------:|
| afrikaans-afribooms-ud-2.10-220711        | Raw text          | 99.78  | 98.59  | 98.58 | 95.46 | 98.13  | 95.33   | 97.43  | 90.10 | 87.23 | 78.64 | 78.59 |
| afrikaans-afribooms-ud-2.10-220711        | Gold tokenization | —      | —      | 98.77 | 95.62 | 98.31  | 95.50   | 97.53  | 90.72 | 87.80 | 79.23 | 78.99 |
| ancient_greek-perseus-ud-2.10-220711      | Raw text          | 99.97  | 98.85  | 92.83 | 85.55 | 91.45  | 84.87   | 86.68  | 80.13 | 74.36 | 54.62 | 55.72 |
| ancient_greek-perseus-ud-2.10-220711      | Gold tokenization | —      | —      | 92.88 | 85.60 | 91.47  | 84.90   | 86.70  | 80.32 | 74.53 | 54.73 | 55.87 |
| ancient_greek-proiel-ud-2.10-220711       | Raw text          | 100.00 | 48.02  | 97.77 | 98.05 | 92.35  | 91.05   | 94.71  | 79.82 | 76.06 | 60.08 | 65.75 |
| ancient_greek-proiel-ud-2.10-220711       | Gold tokenization | —      | —      | 97.87 | 98.14 | 92.49  | 91.26   | 94.73  | 86.05 | 82.14 | 67.03 | 71.90 |
| ancient_hebrew-ptnk-ud-2.10-220711        | Raw text          | 68.76  | 98.06  | 56.80 | 56.94 | 55.13  | 50.80   | 49.88  | 38.73 | 34.67 | 18.47 | 17.69 |
| ancient_hebrew-ptnk-ud-2.10-220711        | Gold tokenization | —      | —      | 68.03 | 67.97 | 66.97  | 56.15   | 53.35  | 63.31 | 51.61 | 28.08 | 24.34 |
| arabic-padt-ud-2.10-220711                | Raw text          | 94.58  | 82.09  | 91.72 | 89.01 | 89.14  | 88.69   | 90.41  | 78.63 | 74.54 | 65.84 | 67.88 |
| arabic-padt-ud-2.10-220711                | Gold tokenization | —      | —      | 97.02 | 94.38 | 94.53  | 94.08   | 95.31  | 88.11 | 83.49 | 74.57 | 76.13 |
| armenian-armtdp-ud-2.10-220711            | Raw text          | 99.28  | 95.70  | 96.07 | —     | 91.39  | 90.28   | 95.04  | 86.84 | 82.22 | 69.53 | 74.39 |
| armenian-armtdp-ud-2.10-220711            | Gold tokenization | —      | —      | 96.63 | —     | 92.03  | 90.77   | 95.70  | 88.50 | 83.81 | 70.18 | 75.42 |
| armenian-bsut-ud-2.10-220711              | Raw text          | 99.79  | 98.73  | 97.31 | —     | 92.01  | 91.24   | 96.62  | 90.02 | 85.75 | 71.20 | 78.86 |
| armenian-bsut-ud-2.10-220711              | Gold tokenization | —      | —      | 97.53 | —     | 92.24  | 91.48   | 96.82  | 90.56 | 86.29 | 71.73 | 79.32 |
| basque-bdt-ud-2.10-220711                 | Raw text          | 99.94  | 99.83  | 96.25 | —     | 92.69  | 90.69   | 96.36  | 87.40 | 84.28 | 73.94 | 79.81 |
| basque-bdt-ud-2.10-220711                 | Gold tokenization | —      | —      | 96.30 | —     | 92.73  | 90.72   | 96.39  | 87.48 | 84.36 | 73.99 | 79.86 |
| belarusian-hse-ud-2.10-220711             | Raw text          | 99.47  | 83.97  | 98.30 | 96.26 | 94.38  | 92.37   | 93.35  | 86.84 | 84.85 | 76.00 | 76.01 |
| belarusian-hse-ud-2.10-220711             | Gold tokenization | —      | —      | 98.81 | 96.74 | 94.94  | 92.87   | 93.86  | 89.55 | 87.38 | 78.23 | 78.11 |
| bulgarian-btb-ud-2.10-220711              | Raw text          | 99.91  | 94.17  | 99.19 | 97.20 | 97.97  | 96.85   | 97.99  | 94.41 | 91.67 | 85.89 | 86.31 |
| bulgarian-btb-ud-2.10-220711              | Gold tokenization | —      | —      | 99.29 | 97.30 | 98.07  | 96.96   | 98.09  | 95.24 | 92.44 | 86.52 | 87.03 |
| catalan-ancora-ud-2.10-220711             | Raw text          | 99.95  | 99.08  | 99.07 | 97.21 | 98.70  | 96.96   | 99.40  | 94.86 | 93.14 | 87.45 | 88.92 |
| catalan-ancora-ud-2.10-220711             | Gold tokenization | —      | —      | 99.14 | 97.32 | 98.78  | 97.07   | 99.46  | 95.02 | 93.30 | 87.69 | 89.13 |
| chinese-gsdsimp-ud-2.10-220711            | Raw text          | 90.29  | 99.10  | 87.21 | 87.16 | 89.74  | 86.42   | 90.29  | 73.11 | 70.62 | 63.58 | 67.09 |
| chinese-gsdsimp-ud-2.10-220711            | Gold tokenization | —      | —      | 96.14 | 96.04 | 99.45  | 95.30   | 99.99  | 87.28 | 84.07 | 78.56 | 82.64 |
| chinese-gsd-ud-2.10-220711                | Raw text          | 90.27  | 99.10  | 87.15 | 87.05 | 89.71  | 86.36   | 90.27  | 72.85 | 70.29 | 63.41 | 66.89 |
| chinese-gsd-ud-2.10-220711                | Gold tokenization | —      | —      | 96.21 | 96.08 | 99.40  | 95.34   | 99.99  | 87.15 | 83.96 | 78.41 | 82.59 |
| classical_chinese-kyoto-ud-2.10-220711    | Raw text          | 97.26  | 40.71  | 87.40 | 86.48 | 89.97  | 83.27   | 96.78  | 67.56 | 62.17 | 58.02 | 60.60 |
| classical_chinese-kyoto-ud-2.10-220711    | Gold tokenization | —      | —      | 92.30 | 90.87 | 93.94  | 88.19   | 99.47  | 83.16 | 77.63 | 73.15 | 76.42 |
| coptic-scriptorium-ud-2.10-220711         | Raw text          | 74.49  | 33.87  | 72.43 | 72.34 | 72.53  | 71.54   | 72.91  | 51.25 | 49.43 | 36.55 | 39.14 |
| coptic-scriptorium-ud-2.10-220711         | Gold tokenization | —      | —      | 96.94 | 96.78 | 97.49  | 95.50   | 97.02  | 90.48 | 87.70 | 76.04 | 79.57 |
| croatian-set-ud-2.10-220711               | Raw text          | 99.93  | 94.79  | 98.48 | 95.72 | 96.23  | 95.49   | 97.60  | 92.17 | 89.27 | 81.53 | 84.23 |
| croatian-set-ud-2.10-220711               | Gold tokenization | —      | —      | 98.54 | 95.80 | 96.30  | 95.56   | 97.68  | 92.67 | 89.75 | 81.92 | 84.69 |
| czech-pdt-ud-2.10-220711                  | Raw text          | 99.94  | 93.74  | 99.37 | 98.40 | 98.33  | 98.02   | 99.21  | 94.90 | 93.50 | 90.28 | 91.88 |
| czech-pdt-ud-2.10-220711                  | Gold tokenization | —      | —      | 99.45 | 98.47 | 98.40  | 98.09   | 99.28  | 95.63 | 94.23 | 90.88 | 92.50 |
| czech-cac-ud-2.10-220711                  | Raw text          | 99.99  | 99.68  | 99.72 | 98.57 | 98.37  | 98.12   | 99.18  | 96.12 | 94.76 | 91.09 | 92.67 |
| czech-cac-ud-2.10-220711                  | Gold tokenization | —      | —      | 99.73 | 98.58 | 98.38  | 98.13   | 99.19  | 96.12 | 94.76 | 91.11 | 92.69 |
| czech-cltt-ud-2.10-220711                 | Raw text          | 99.71  | 97.79  | 99.22 | 95.32 | 95.23  | 95.03   | 99.18  | 90.77 | 89.24 | 81.35 | 86.22 |
| czech-cltt-ud-2.10-220711                 | Gold tokenization | —      | —      | 99.47 | 95.47 | 95.40  | 95.18   | 99.47  | 91.20 | 89.68 | 81.67 | 86.73 |
| czech-fictree-ud-2.10-220711              | Raw text          | 99.99  | 98.95  | 99.17 | 97.06 | 97.83  | 96.86   | 99.35  | 96.38 | 94.91 | 89.61 | 92.81 |
| czech-fictree-ud-2.10-220711              | Gold tokenization | —      | —      | 99.18 | 97.08 | 97.84  | 96.88   | 99.36  | 96.46 | 94.97 | 89.71 | 92.91 |
| danish-ddt-ud-2.10-220711                 | Raw text          | 99.81  | 89.78  | 97.95 | —     | 97.29  | 96.54   | 97.26  | 88.27 | 86.25 | 79.22 | 80.96 |
| danish-ddt-ud-2.10-220711                 | Gold tokenization | —      | —      | 98.16 | —     | 97.53  | 96.79   | 97.45  | 89.46 | 87.42 | 80.42 | 82.17 |
| dutch-alpino-ud-2.10-220711               | Raw text          | 99.83  | 88.98  | 97.86 | 96.79 | 97.80  | 96.29   | 95.11  | 92.95 | 90.58 | 83.15 | 79.88 |
| dutch-alpino-ud-2.10-220711               | Gold tokenization | —      | —      | 97.97 | 96.87 | 97.91  | 96.41   | 95.26  | 94.00 | 91.63 | 84.17 | 80.83 |
| dutch-lassysmall-ud-2.10-220711           | Raw text          | 99.80  | 74.93  | 96.98 | 95.62 | 96.63  | 94.88   | 95.70  | 90.61 | 87.94 | 79.67 | 78.25 |
| dutch-lassysmall-ud-2.10-220711           | Gold tokenization | —      | —      | 97.25 | 96.43 | 97.36  | 95.83   | 95.97  | 94.51 | 91.66 | 84.48 | 83.08 |
| english-ewt-ud-2.10-220711                | Raw text          | 98.95  | 87.02  | 96.39 | 96.13 | 96.53  | 94.80   | 97.13  | 90.07 | 88.10 | 81.47 | 83.42 |
| english-ewt-ud-2.10-220711                | Gold tokenization | —      | —      | 97.35 | 97.06 | 97.52  | 95.71   | 98.07  | 92.62 | 90.56 | 84.02 | 85.98 |
| english-atis-ud-2.10-220711               | Raw text          | 100.00 | 81.96  | 98.97 | —     | 98.54  | 98.13   | 99.94  | 94.39 | 92.92 | 87.85 | 90.39 |
| english-atis-ud-2.10-220711               | Gold tokenization | —      | —      | 98.97 | —     | 98.56  | 98.15   | 99.94  | 95.88 | 94.26 | 89.80 | 92.40 |
| english-gum-ud-2.10-220711                | Raw text          | 99.64  | 95.36  | 97.95 | 97.91 | 97.88  | 96.91   | 98.77  | 92.30 | 90.35 | 84.63 | 86.31 |
| english-gum-ud-2.10-220711                | Gold tokenization | —      | —      | 98.27 | 98.26 | 98.22  | 97.24   | 99.09  | 93.17 | 91.19 | 85.42 | 87.04 |
| english-lines-ud-2.10-220711              | Raw text          | 99.92  | 87.45  | 97.71 | 96.77 | 97.02  | 94.41   | 98.40  | 91.17 | 88.22 | 80.27 | 83.45 |
| english-lines-ud-2.10-220711              | Gold tokenization | —      | —      | 97.79 | 96.84 | 97.07  | 94.48   | 98.47  | 92.10 | 89.17 | 81.05 | 84.36 |
| english-partut-ud-2.10-220711             | Raw text          | 99.72  | 100.00 | 97.23 | 97.11 | 96.35  | 95.26   | 98.14  | 94.24 | 92.21 | 83.35 | 87.34 |
| english-partut-ud-2.10-220711             | Gold tokenization | —      | —      | 97.48 | 97.36 | 96.60  | 95.51   | 98.42  | 94.48 | 92.46 | 83.74 | 87.62 |
| estonian-edt-ud-2.10-220711               | Raw text          | 99.95  | 92.03  | 97.68 | 98.31 | 96.28  | 95.07   | 95.36  | 88.81 | 86.16 | 79.92 | 79.56 |
| estonian-edt-ud-2.10-220711               | Gold tokenization | —      | —      | 97.81 | 98.36 | 96.36  | 95.19   | 95.43  | 89.71 | 87.03 | 80.77 | 80.37 |
| estonian-ewt-ud-2.10-220711               | Raw text          | 98.82  | 75.26  | 95.41 | 96.29 | 94.06  | 91.92   | 93.86  | 82.62 | 79.30 | 71.40 | 72.35 |
| estonian-ewt-ud-2.10-220711               | Gold tokenization | —      | —      | 96.65 | 97.43 | 95.15  | 93.10   | 94.97  | 86.76 | 83.25 | 74.79 | 75.57 |
| faroese-farpahc-ud-2.10-220711            | Raw text          | 99.74  | 92.77  | 97.44 | 93.04 | 94.43  | 92.50   | 99.74  | 85.76 | 82.13 | 68.07 | 75.34 |
| faroese-farpahc-ud-2.10-220711            | Gold tokenization | —      | —      | 97.64 | 93.28 | 94.68  | 92.72   | 100.00 | 86.82 | 83.10 | 69.17 | 76.51 |
| finnish-tdt-ud-2.10-220711                | Raw text          | 99.70  | 90.82  | 97.58 | 98.18 | 95.99  | 95.10   | 92.14  | 90.20 | 88.18 | 82.19 | 78.16 |
| finnish-tdt-ud-2.10-220711                | Gold tokenization | —      | —      | 97.92 | 98.49 | 96.29  | 95.43   | 92.46  | 91.51 | 89.46 | 83.20 | 79.17 |
| finnish-ftb-ud-2.10-220711                | Raw text          | 99.91  | 86.84  | 96.69 | 95.14 | 96.83  | 94.02   | 95.57  | 89.80 | 87.18 | 80.04 | 80.49 |
| finnish-ftb-ud-2.10-220711                | Gold tokenization | —      | —      | 97.00 | 95.36 | 96.92  | 94.32   | 95.67  | 91.91 | 89.23 | 82.55 | 82.84 |
| french-gsd-ud-2.10-220711                 | Raw text          | 98.78  | 94.69  | 97.26 | —     | 97.35  | 96.63   | 97.55  | 92.76 | 90.82 | 84.55 | 86.32 |
| french-gsd-ud-2.10-220711                 | Gold tokenization | —      | —      | 98.44 | —     | 98.47  | 97.71   | 98.75  | 94.55 | 92.71 | 86.34 | 87.59 |
| french-parisstories-ud-2.10-220711        | Raw text          | 99.49  | 87.87  | 96.24 | —     | 94.41  | 92.17   | 97.55  | 79.95 | 74.84 | 61.23 | 68.35 |
| french-parisstories-ud-2.10-220711        | Gold tokenization | —      | —      | 96.81 | —     | 94.90  | 92.68   | 97.98  | 81.67 | 76.50 | 62.46 | 69.32 |
| french-partut-ud-2.10-220711              | Raw text          | 99.48  | 100.00 | 97.26 | 96.76 | 94.72  | 93.96   | 97.33  | 94.72 | 92.81 | 81.09 | 86.22 |
| french-partut-ud-2.10-220711              | Gold tokenization | —      | —      | 97.89 | 97.35 | 95.27  | 94.51   | 97.89  | 95.62 | 93.85 | 82.18 | 87.24 |
| french-rhapsodie-ud-2.10-220711           | Raw text          | 99.22  | 99.47  | 97.20 | 97.45 | 96.12  | 93.30   | 98.26  | 88.71 | 84.99 | 75.15 | 79.88 |
| french-rhapsodie-ud-2.10-220711           | Gold tokenization | —      | —      | 98.00 | 98.13 | 96.89  | 93.97   | 98.99  | 89.89 | 86.08 | 75.91 | 80.35 |
| french-sequoia-ud-2.10-220711             | Raw text          | 99.15  | 84.02  | 98.32 | —     | 97.15  | 96.68   | 98.33  | 93.60 | 92.22 | 86.08 | 89.00 |
| french-sequoia-ud-2.10-220711             | Gold tokenization | —      | —      | 99.24 | —     | 97.95  | 97.54   | 99.13  | 95.43 | 94.11 | 88.00 | 90.34 |
| galician-ctg-ud-2.10-220711               | Raw text          | 99.22  | 97.22  | 97.28 | 97.05 | 99.06  | 96.70   | 98.04  | 85.59 | 83.20 | 72.11 | 76.94 |
| galician-ctg-ud-2.10-220711               | Gold tokenization | —      | —      | 98.01 | 97.78 | 99.84  | 97.41   | 98.79  | 87.31 | 84.80 | 74.04 | 78.88 |
| galician-treegal-ud-2.10-220711           | Raw text          | 98.74  | 87.99  | 96.00 | 93.69 | 94.85  | 92.82   | 96.67  | 83.44 | 79.36 | 67.82 | 71.68 |
| galician-treegal-ud-2.10-220711           | Gold tokenization | —      | —      | 97.19 | 94.83 | 95.94  | 93.91   | 97.86  | 86.75 | 82.40 | 71.30 | 75.54 |
| german-hdt-ud-2.10-220711                 | Raw text          | 99.90  | 92.34  | 98.51 | 98.45 | 94.08  | 93.70   | 97.16  | 96.94 | 96.04 | 84.79 | 90.40 |
| german-hdt-ud-2.10-220711                 | Gold tokenization | —      | —      | 98.62 | 98.58 | 94.22  | 93.83   | 97.26  | 97.63 | 96.75 | 85.46 | 91.08 |
| german-gsd-ud-2.10-220711                 | Raw text          | 99.81  | 81.12  | 95.78 | 97.68 | 90.23  | 87.27   | 96.75  | 87.32 | 83.12 | 63.79 | 75.00 |
| german-gsd-ud-2.10-220711                 | Gold tokenization | —      | —      | 95.94 | 97.87 | 90.60  | 87.60   | 96.96  | 89.28 | 85.04 | 65.33 | 76.75 |
| gothic-proiel-ud-2.10-220711              | Raw text          | 100.00 | 31.12  | 96.48 | 96.98 | 90.08  | 88.19   | 94.62  | 74.17 | 68.40 | 55.39 | 62.02 |
| gothic-proiel-ud-2.10-220711              | Gold tokenization | —      | —      | 96.97 | 97.42 | 90.90  | 89.33   | 94.71  | 84.11 | 78.29 | 65.73 | 71.10 |
| greek-gdt-ud-2.10-220711                  | Raw text          | 99.87  | 90.19  | 98.09 | 98.10 | 95.60  | 95.01   | 95.61  | 93.05 | 91.24 | 81.58 | 81.04 |
| greek-gdt-ud-2.10-220711                  | Gold tokenization | —      | —      | 98.23 | 98.24 | 95.79  | 95.20   | 95.70  | 93.85 | 92.04 | 82.28 | 81.75 |
| hebrew-htb-ud-2.10-220711                 | Raw text          | 85.05  | 99.39  | 82.78 | 82.80 | 81.23  | 80.53   | 82.91  | 70.63 | 68.13 | 55.31 | 59.44 |
| hebrew-htb-ud-2.10-220711                 | Gold tokenization | —      | —      | 97.44 | 97.42 | 95.73  | 95.05   | 97.34  | 92.71 | 90.24 | 78.77 | 81.80 |
| hebrew-iahltwiki-ud-2.10-220711           | Raw text          | 88.54  | 97.16  | 85.97 | 86.00 | 80.55  | 79.47   | 87.15  | 76.16 | 74.19 | 56.91 | 66.92 |
| hebrew-iahltwiki-ud-2.10-220711           | Gold tokenization | —      | —      | 97.09 | 97.10 | 91.59  | 90.41   | 98.24  | 93.88 | 91.45 | 74.27 | 85.44 |
| hindi-hdtb-ud-2.10-220711                 | Raw text          | 100.00 | 98.90  | 97.57 | 97.12 | 94.16  | 92.23   | 98.92  | 95.30 | 92.32 | 79.20 | 87.66 |
| hindi-hdtb-ud-2.10-220711                 | Gold tokenization | —      | —      | 97.58 | 97.14 | 94.18  | 92.26   | 98.92  | 95.42 | 92.44 | 79.35 | 87.81 |
| hungarian-szeged-ud-2.10-220711           | Raw text          | 99.85  | 95.89  | 96.68 | —     | 94.22  | 93.53   | 94.92  | 88.81 | 85.09 | 75.22 | 78.19 |
| hungarian-szeged-ud-2.10-220711           | Gold tokenization | —      | —      | 96.79 | —     | 94.36  | 93.64   | 95.04  | 89.31 | 85.54 | 75.51 | 78.47 |
| icelandic-icepahc-ud-2.10-220711          | Raw text          | 99.82  | 92.15  | 96.90 | 93.24 | 91.32  | 86.42   | 95.99  | 87.21 | 83.36 | 65.98 | 74.25 |
| icelandic-icepahc-ud-2.10-220711          | Gold tokenization | —      | —      | 97.08 | 93.45 | 91.47  | 86.61   | 96.15  | 87.78 | 83.87 | 66.49 | 74.84 |
| icelandic-modern-ud-2.10-220711           | Raw text          | 99.92  | 99.22  | 99.07 | 98.14 | 98.38  | 97.88   | 98.91  | 94.41 | 93.17 | 89.31 | 90.07 |
| icelandic-modern-ud-2.10-220711           | Gold tokenization | —      | —      | 99.14 | 98.21 | 98.45  | 97.95   | 98.98  | 94.50 | 93.26 | 89.41 | 90.16 |
| indonesian-gsd-ud-2.10-220711             | Raw text          | 99.48  | 92.90  | 94.23 | 93.81 | 95.53  | 88.78   | 98.13  | 87.65 | 81.59 | 72.35 | 77.02 |
| indonesian-gsd-ud-2.10-220711             | Gold tokenization | —      | —      | 94.66 | 94.26 | 95.99  | 89.17   | 98.53  | 88.57 | 82.42 | 73.21 | 77.88 |
| indonesian-csui-ud-2.10-220711            | Raw text          | 99.45  | 91.01  | 96.05 | 96.14 | 96.85  | 95.43   | 98.23  | 86.38 | 82.10 | 76.54 | 78.80 |
| indonesian-csui-ud-2.10-220711            | Gold tokenization | —      | —      | 96.56 | 96.72 | 97.37  | 95.99   | 98.87  | 87.77 | 83.28 | 77.62 | 79.92 |
| irish-idt-ud-2.10-220711                  | Raw text          | 99.72  | 97.25  | 95.63 | 94.76 | 90.33  | 87.14   | 95.30  | 86.74 | 81.10 | 64.20 | 71.52 |
| irish-idt-ud-2.10-220711                  | Gold tokenization | —      | —      | 95.89 | 95.07 | 90.60  | 87.46   | 95.54  | 87.28 | 81.64 | 64.56 | 71.85 |
| italian-isdt-ud-2.10-220711               | Raw text          | 99.84  | 98.76  | 98.57 | 98.50 | 98.25  | 97.67   | 98.79  | 94.66 | 93.01 | 86.61 | 88.00 |
| italian-isdt-ud-2.10-220711               | Gold tokenization | —      | —      | 98.72 | 98.65 | 98.41  | 97.83   | 98.95  | 94.96 | 93.34 | 86.97 | 88.40 |
| italian-markit-ud-2.10-220711             | Raw text          | 99.59  | 98.24  | 96.76 | 97.00 | 93.80  | 92.08   | 88.18  | 88.36 | 84.51 | 69.95 | 77.77 |
| italian-markit-ud-2.10-220711             | Gold tokenization | —      | —      | 97.15 | 97.40 | 94.10  | 92.35   | 88.54  | 89.13 | 85.26 | 70.51 | 78.46 |
| italian-partut-ud-2.10-220711             | Raw text          | 99.73  | 100.00 | 98.43 | 98.43 | 98.35  | 97.61   | 98.68  | 96.21 | 94.18 | 87.87 | 89.09 |
| italian-partut-ud-2.10-220711             | Gold tokenization | —      | —      | 98.54 | 98.57 | 98.49  | 97.69   | 98.93  | 96.26 | 94.15 | 87.68 | 89.07 |
| italian-postwita-ud-2.10-220711           | Raw text          | 99.40  | 28.11  | 96.43 | 96.18 | 96.30  | 94.79   | 96.72  | 80.61 | 76.89 | 65.29 | 66.90 |
| italian-postwita-ud-2.10-220711           | Gold tokenization | —      | —      | 97.04 | 96.82 | 96.80  | 95.29   | 97.31  | 88.34 | 84.19 | 75.32 | 77.32 |
| italian-twittiro-ud-2.10-220711           | Raw text          | 99.14  | 39.36  | 95.92 | 95.92 | 95.07  | 93.46   | 94.50  | 82.23 | 77.79 | 64.50 | 65.42 |
| italian-twittiro-ud-2.10-220711           | Gold tokenization | —      | —      | 96.91 | 96.61 | 96.00  | 94.15   | 95.16  | 88.07 | 83.53 | 71.89 | 72.69 |
| italian-vit-ud-2.10-220711                | Raw text          | 99.76  | 96.73  | 98.14 | 97.39 | 97.64  | 96.21   | 98.89  | 92.08 | 89.16 | 80.93 | 83.70 |
| italian-vit-ud-2.10-220711                | Gold tokenization | —      | —      | 98.36 | 97.71 | 97.85  | 96.53   | 99.10  | 92.88 | 89.97 | 81.91 | 84.63 |
| japanese-gsd-ud-2.10-220711               | Raw text          | 96.17  | 100.00 | 94.93 | 94.18 | 96.16  | 93.81   | 95.05  | 87.68 | 86.85 | 80.43 | 80.78 |
| japanese-gsd-ud-2.10-220711               | Gold tokenization | —      | —      | 98.55 | 97.50 | 99.99  | 97.13   | 98.47  | 94.73 | 93.75 | 88.50 | 88.34 |
| japanese-gsdluw-ud-2.10-220711            | Raw text          | 95.18  | 99.72  | 93.81 | 93.54 | 95.18  | 93.46   | 93.66  | 86.22 | 85.54 | 76.27 | 76.58 |
| japanese-gsdluw-ud-2.10-220711            | Gold tokenization | —      | —      | 98.36 | 98.05 | 100.00 | 97.93   | 97.89  | 95.23 | 94.18 | 86.38 | 85.19 |
| korean-kaist-ud-2.10-220711               | Raw text          | 100.00 | 100.00 | 95.88 | 87.74 | —      | 87.56   | 94.17  | 89.33 | 87.47 | 82.15 | 80.14 |
| korean-kaist-ud-2.10-220711               | Gold tokenization | —      | —      | 95.88 | 87.74 | —      | 87.56   | 94.17  | 89.33 | 87.47 | 82.15 | 80.14 |
| korean-gsd-ud-2.10-220711                 | Raw text          | 99.87  | 93.93  | 96.57 | 90.27 | 99.67  | 88.02   | 93.57  | 88.54 | 84.91 | 80.73 | 77.23 |
| korean-gsd-ud-2.10-220711                 | Gold tokenization | —      | —      | 96.73 | 90.43 | 99.80  | 88.20   | 93.69  | 89.27 | 85.61 | 81.45 | 77.93 |
| latin-ittb-ud-2.10-220711                 | Raw text          | 99.99  | 91.21  | 98.91 | 96.58 | 96.75  | 95.19   | 99.18  | 90.53 | 88.53 | 82.07 | 86.07 |
| latin-ittb-ud-2.10-220711                 | Gold tokenization | —      | —      | 98.92 | 96.57 | 96.78  | 95.20   | 99.18  | 91.50 | 89.51 | 82.63 | 86.59 |
| latin-llct-ud-2.10-220711                 | Raw text          | 100.00 | 99.49  | 99.68 | 97.14 | 97.26  | 96.89   | 97.78  | 95.55 | 94.56 | 89.80 | 90.95 |
| latin-llct-ud-2.10-220711                 | Gold tokenization | —      | —      | 99.68 | 97.15 | 97.27  | 96.90   | 97.78  | 95.55 | 94.57 | 89.81 | 90.97 |
| latin-perseus-ud-2.10-220711              | Raw text          | 100.00 | 98.46  | 91.83 | 80.66 | 86.12  | 78.56   | 88.13  | 77.98 | 68.59 | 52.30 | 55.51 |
| latin-perseus-ud-2.10-220711              | Gold tokenization | —      | —      | 91.85 | 80.66 | 86.12  | 78.55   | 88.16  | 78.14 | 68.71 | 52.39 | 55.58 |
| latin-proiel-ud-2.10-220711               | Raw text          | 99.87  | 36.81  | 96.69 | 96.87 | 90.56  | 89.54   | 96.21  | 74.07 | 69.56 | 56.74 | 63.93 |
| latin-proiel-ud-2.10-220711               | Gold tokenization | —      | —      | 97.12 | 97.32 | 91.19  | 90.27   | 96.44  | 83.20 | 78.50 | 66.34 | 73.00 |
| latin-udante-ud-2.10-220711               | Raw text          | 99.61  | 98.81  | 90.58 | 75.59 | 81.31  | 71.62   | 87.25  | 75.26 | 67.81 | 43.95 | 50.36 |
| latin-udante-ud-2.10-220711               | Gold tokenization | —      | —      | 90.82 | 75.70 | 81.53  | 71.70   | 87.44  | 75.50 | 67.97 | 44.08 | 50.51 |
| latvian-lvtb-ud-2.10-220711               | Raw text          | 99.31  | 97.83  | 96.51 | 89.83 | 93.86  | 89.08   | 95.92  | 88.75 | 85.79 | 76.04 | 80.25 |
| latvian-lvtb-ud-2.10-220711               | Gold tokenization | —      | —      | 97.14 | 90.43 | 94.50  | 89.67   | 96.55  | 89.84 | 86.82 | 77.09 | 81.31 |
| lithuanian-alksnis-ud-2.10-220711         | Raw text          | 99.91  | 87.87  | 95.94 | 90.44 | 91.03  | 89.52   | 93.60  | 82.45 | 78.64 | 67.97 | 71.37 |
| lithuanian-alksnis-ud-2.10-220711         | Gold tokenization | —      | —      | 96.04 | 90.52 | 91.16  | 89.63   | 93.69  | 83.70 | 79.88 | 68.98 | 72.36 |
| lithuanian-hse-ud-2.10-220711             | Raw text          | 97.30  | 97.30  | 89.28 | 90.21 | 83.13  | 78.38   | 88.16  | 70.27 | 61.79 | 45.67 | 54.04 |
| lithuanian-hse-ud-2.10-220711             | Gold tokenization | —      | —      | 91.23 | 92.36 | 85.19  | 80.09   | 90.57  | 73.96 | 64.53 | 47.54 | 56.10 |
| maltese-mudt-ud-2.10-220711               | Raw text          | 99.84  | 86.29  | 95.80 | 95.79 | —      | 95.35   | —      | 84.96 | 80.07 | 68.98 | 72.86 |
| maltese-mudt-ud-2.10-220711               | Gold tokenization | —      | —      | 95.95 | 95.92 | —      | 95.48   | —      | 85.65 | 80.70 | 69.40 | 73.33 |
| marathi-ufal-ud-2.10-220711               | Raw text          | 90.25  | 92.63  | 76.50 | —     | 65.25  | 60.75   | 80.75  | 60.75 | 50.75 | 28.39 | 38.00 |
| marathi-ufal-ud-2.10-220711               | Gold tokenization | —      | —      | 82.52 | —     | 67.96  | 62.86   | 80.83  | 68.93 | 58.50 | 29.46 | 38.17 |
| naija-nsc-ud-2.10-220711                  | Raw text          | 99.94  | 100.00 | 98.03 | —     | 98.94  | 97.53   | 99.32  | 93.65 | 90.99 | 88.13 | 89.60 |
| naija-nsc-ud-2.10-220711                  | Gold tokenization | —      | —      | 98.08 | —     | 99.00  | 97.58   | 99.38  | 93.75 | 91.08 | 88.21 | 89.68 |
| north_sami-giella-ud-2.10-220711          | Raw text          | 99.87  | 98.79  | 91.77 | 93.54 | 89.30  | 85.36   | 87.01  | 75.16 | 70.43 | 59.76 | 58.27 |
| north_sami-giella-ud-2.10-220711          | Gold tokenization | —      | —      | 91.91 | 93.67 | 89.45  | 85.52   | 87.13  | 75.47 | 70.76 | 60.05 | 58.56 |
| norwegian-bokmaal-ud-2.10-220711          | Raw text          | 99.77  | 96.05  | 98.35 | —     | 97.43  | 96.82   | 98.57  | 93.62 | 92.16 | 86.91 | 88.74 |
| norwegian-bokmaal-ud-2.10-220711          | Gold tokenization | —      | —      | 98.61 | —     | 97.68  | 97.07   | 98.82  | 94.40 | 92.91 | 87.59 | 89.43 |
| norwegian-nynorsk-ud-2.10-220711          | Raw text          | 99.93  | 94.17  | 98.24 | —     | 97.34  | 96.55   | 98.40  | 93.89 | 92.18 | 86.03 | 88.36 |
| norwegian-nynorsk-ud-2.10-220711          | Gold tokenization | —      | —      | 98.41 | —     | 97.50  | 96.73   | 98.53  | 94.63 | 92.93 | 86.93 | 89.20 |
| norwegian-nynorsklia-ud-2.10-220711       | Raw text          | 99.91  | 99.53  | 96.61 | —     | 95.71  | 93.75   | 98.05  | 81.18 | 76.61 | 66.01 | 69.68 |
| norwegian-nynorsklia-ud-2.10-220711       | Gold tokenization | —      | —      | 96.72 | —     | 95.80  | 93.85   | 98.14  | 81.42 | 76.84 | 66.23 | 69.90 |
| old_church_slavonic-proiel-ud-2.10-220711 | Raw text          | 100.00 | 41.43  | 96.72 | 96.90 | 90.37  | 89.19   | 93.13  | 77.71 | 73.92 | 63.82 | 68.87 |
| old_church_slavonic-proiel-ud-2.10-220711 | Gold tokenization | —      | —      | 97.08 | 97.28 | 91.06  | 89.93   | 93.14  | 88.30 | 84.18 | 74.01 | 77.39 |
| old_french-srcmf-ud-2.10-220711           | Raw text          | 99.70  | 100.00 | 96.68 | 96.50 | 97.70  | 95.72   | 99.65  | 91.17 | 87.38 | 80.76 | 84.40 |
| old_french-srcmf-ud-2.10-220711           | Gold tokenization | —      | —      | 96.99 | 96.82 | 98.01  | 96.03   | 99.95  | 91.58 | 87.82 | 81.20 | 84.85 |
| old_russian-torot-ud-2.10-220711          | Raw text          | 100.00 | 29.60  | 94.39 | 94.70 | 87.56  | 85.23   | 85.92  | 71.00 | 65.32 | 51.64 | 53.64 |
| old_russian-torot-ud-2.10-220711          | Gold tokenization | —      | —      | 95.06 | 95.29 | 88.50  | 86.60   | 85.96  | 83.30 | 77.24 | 64.09 | 62.94 |
| old_russian-rnc-ud-2.10-220711            | Raw text          | 97.48  | 84.03  | 90.94 | 86.55 | 76.51  | 67.15   | 75.31  | 61.28 | 55.93 | 33.24 | 34.04 |
| old_russian-rnc-ud-2.10-220711            | Gold tokenization | —      | —      | 93.29 | 88.93 | 78.48  | 68.86   | 76.77  | 67.13 | 61.08 | 37.15 | 37.24 |
| old_east_slavic-birchbark-ud-2.10-220711  | Raw text          | 99.98  | 16.73  | 89.24 | 99.35 | 76.11  | 72.43   | 65.88  | 63.41 | 56.50 | 32.53 | 27.14 |
| old_east_slavic-birchbark-ud-2.10-220711  | Gold tokenization | —      | —      | 89.37 | 99.37 | 76.54  | 72.82   | 66.05  | 76.31 | 69.00 | 41.63 | 33.60 |
| persian-perdt-ud-2.10-220711              | Raw text          | 99.66  | 99.83  | 97.48 | 97.36 | 97.61  | 95.60   | 98.88  | 93.63 | 91.42 | 86.18 | 88.66 |
| persian-perdt-ud-2.10-220711              | Gold tokenization | —      | —      | 97.78 | 97.65 | 97.90  | 95.89   | 99.19  | 94.18 | 91.95 | 86.72 | 89.23 |
| persian-seraji-ud-2.10-220711             | Raw text          | 99.65  | 98.75  | 97.91 | 97.94 | 97.95  | 97.48   | 96.52  | 91.68 | 88.84 | 84.21 | 82.83 |
| persian-seraji-ud-2.10-220711             | Gold tokenization | —      | —      | 98.24 | 98.28 | 98.28  | 97.78   | 96.80  | 92.36 | 89.48 | 84.82 | 83.40 |
| polish-pdb-ud-2.10-220711                 | Raw text          | 99.85  | 97.33  | 98.89 | 95.89 | 96.11  | 95.26   | 98.10  | 94.22 | 92.19 | 85.44 | 88.36 |
| polish-pdb-ud-2.10-220711                 | Gold tokenization | —      | —      | 99.05 | 96.03 | 96.24  | 95.40   | 98.24  | 94.72 | 92.69 | 85.83 | 88.78 |
| polish-lfg-ud-2.10-220711                 | Raw text          | 99.85  | 99.65  | 99.00 | 96.08 | 96.57  | 95.16   | 98.24  | 96.86 | 95.51 | 89.80 | 92.34 |
| polish-lfg-ud-2.10-220711                 | Gold tokenization | —      | —      | 99.17 | 96.25 | 96.74  | 95.33   | 98.38  | 97.25 | 95.89 | 90.19 | 92.66 |
| pomak-philotis-ud-2.10-220711             | Raw text          | 99.98  | 94.49  | 98.86 | —     | 95.62  | 95.30   | 96.67  | 88.24 | 83.26 | 71.19 | 74.14 |
| pomak-philotis-ud-2.10-220711             | Gold tokenization | —      | —      | 98.90 | —     | 95.65  | 95.33   | 96.69  | 88.68 | 83.75 | 71.48 | 74.42 |
| portuguese-gsd-ud-2.10-220711             | Raw text          | 99.87  | 97.28  | 98.51 | 98.51 | 99.74  | 98.41   | 99.27  | 94.50 | 93.41 | 88.76 | 89.96 |
| portuguese-gsd-ud-2.10-220711             | Gold tokenization | —      | —      | 98.65 | 98.64 | 99.89  | 98.55   | 99.40  | 94.90 | 93.81 | 89.23 | 90.36 |
| portuguese-bosque-ud-2.10-220711          | Raw text          | 99.68  | 89.89  | 97.87 | —     | 96.95  | 96.00   | 98.35  | 92.35 | 90.07 | 81.38 | 84.69 |
| portuguese-bosque-ud-2.10-220711          | Gold tokenization | —      | —      | 98.22 | —     | 97.23  | 96.28   | 98.66  | 93.50 | 91.16 | 82.47 | 85.87 |
| romanian-nonstandard-ud-2.10-220711       | Raw text          | 98.83  | 96.77  | 96.18 | 91.87 | 90.53  | 89.18   | 94.90  | 88.85 | 84.82 | 68.21 | 76.36 |
| romanian-nonstandard-ud-2.10-220711       | Gold tokenization | —      | —      | 97.30 | 92.86 | 91.49  | 90.10   | 95.99  | 90.57 | 86.50 | 69.69 | 77.68 |
| romanian-rrt-ud-2.10-220711               | Raw text          | 99.71  | 95.16  | 97.90 | 97.21 | 97.40  | 96.98   | 97.96  | 91.97 | 88.44 | 81.66 | 83.13 |
| romanian-rrt-ud-2.10-220711               | Gold tokenization | —      | —      | 98.19 | 97.45 | 97.65  | 97.22   | 98.22  | 92.72 | 89.13 | 82.15 | 83.70 |
| romanian-simonero-ud-2.10-220711          | Raw text          | 99.84  | 100.00 | 98.45 | 97.97 | 97.56  | 97.25   | 98.91  | 94.08 | 92.13 | 85.52 | 88.32 |
| romanian-simonero-ud-2.10-220711          | Gold tokenization | —      | —      | 98.61 | 98.12 | 97.70  | 97.40   | 99.07  | 94.42 | 92.45 | 85.81 | 88.62 |
| russian-syntagrus-ud-2.10-220711          | Raw text          | 99.67  | 98.31  | 98.46 | —     | 93.96  | 93.71   | 98.18  | 93.84 | 91.70 | 82.72 | 88.90 |
| russian-syntagrus-ud-2.10-220711          | Gold tokenization | —      | —      | 98.79 | —     | 94.28  | 94.03   | 98.46  | 94.56 | 92.39 | 83.28 | 89.44 |
| russian-gsd-ud-2.10-220711                | Raw text          | 99.50  | 96.49  | 98.11 | 97.55 | 94.71  | 93.61   | 97.01  | 91.44 | 88.55 | 81.04 | 84.62 |
| russian-gsd-ud-2.10-220711                | Gold tokenization | —      | —      | 98.58 | 97.98 | 95.17  | 94.01   | 97.43  | 92.67 | 89.69 | 82.00 | 85.65 |
| russian-taiga-ud-2.10-220711              | Raw text          | 98.12  | 86.33  | 95.65 | —     | 93.13  | 92.06   | 94.73  | 83.08 | 79.57 | 70.60 | 73.88 |
| russian-taiga-ud-2.10-220711              | Gold tokenization | —      | —      | 97.34 | —     | 94.90  | 93.72   | 96.37  | 85.64 | 81.92 | 72.82 | 76.10 |
| sanskrit-vedic-ud-2.10-220711             | Raw text          | 100.00 | 27.18  | 89.16 | —     | 81.61  | 76.76   | 87.05  | 60.92 | 50.04 | 41.66 | 44.99 |
| sanskrit-vedic-ud-2.10-220711             | Gold tokenization | —      | —      | 89.97 | —     | 83.02  | 78.34   | 87.34  | 73.74 | 62.01 | 52.00 | 55.41 |
| scottish_gaelic-arcosg-ud-2.10-220711     | Raw text          | 97.47  | 60.89  | 93.78 | 89.29 | 90.91  | 88.21   | 95.08  | 81.24 | 75.60 | 62.73 | 69.22 |
| scottish_gaelic-arcosg-ud-2.10-220711     | Gold tokenization | —      | —      | 96.62 | 92.24 | 94.02  | 91.39   | 97.59  | 87.33 | 81.65 | 69.25 | 75.23 |
| serbian-set-ud-2.10-220711                | Raw text          | 99.99  | 93.00  | 99.09 | 96.00 | 96.21  | 95.75   | 97.76  | 93.63 | 91.20 | 83.76 | 87.00 |
| serbian-set-ud-2.10-220711                | Gold tokenization | —      | —      | 99.13 | 96.01 | 96.20  | 95.75   | 97.78  | 94.26 | 91.80 | 84.32 | 87.60 |
| slovak-snk-ud-2.10-220711                 | Raw text          | 100.00 | 81.69  | 97.65 | 90.35 | 93.50  | 89.56   | 96.46  | 91.39 | 89.65 | 80.43 | 84.44 |
| slovak-snk-ud-2.10-220711                 | Gold tokenization | —      | —      | 97.88 | 90.55 | 93.69  | 89.80   | 96.50  | 93.91 | 92.08 | 82.89 | 86.95 |
| slovenian-ssj-ud-2.10-220711              | Raw text          | 99.94  | 98.95  | 98.97 | 96.97 | 97.15  | 96.63   | 98.58  | 93.99 | 92.60 | 86.83 | 88.91 |
| slovenian-ssj-ud-2.10-220711              | Gold tokenization | —      | —      | 99.03 | 97.02 | 97.23  | 96.69   | 98.63  | 94.15 | 92.76 | 86.99 | 89.02 |
| slovenian-sst-ud-2.10-220711              | Raw text          | 99.85  | 23.14  | 94.82 | 92.71 | 92.43  | 89.84   | 97.38  | 65.69 | 60.84 | 50.88 | 54.78 |
| slovenian-sst-ud-2.10-220711              | Gold tokenization | —      | —      | 95.62 | 93.09 | 92.84  | 90.89   | 97.56  | 78.39 | 73.07 | 63.39 | 68.33 |
| spanish-ancora-ud-2.10-220711             | Raw text          | 99.95  | 98.78  | 99.06 | 96.02 | 98.74  | 95.59   | 99.37  | 93.70 | 91.79 | 86.41 | 87.88 |
| spanish-ancora-ud-2.10-220711             | Gold tokenization | —      | —      | 99.11 | 96.07 | 98.79  | 95.63   | 99.42  | 93.88 | 91.97 | 86.59 | 88.04 |
| spanish-gsd-ud-2.10-220711                | Raw text          | 99.75  | 95.62  | 97.15 | —     | 96.94  | 95.27   | 98.72  | 91.87 | 89.57 | 78.63 | 84.25 |
| spanish-gsd-ud-2.10-220711                | Gold tokenization | —      | —      | 97.39 | —     | 97.19  | 95.53   | 98.97  | 92.66 | 90.32 | 79.43 | 85.04 |
| swedish-talbanken-ud-2.10-220711          | Raw text          | 99.84  | 96.53  | 98.44 | 97.33 | 97.32  | 96.51   | 98.15  | 92.23 | 89.85 | 83.92 | 85.97 |
| swedish-talbanken-ud-2.10-220711          | Gold tokenization | —      | —      | 98.61 | 97.52 | 97.51  | 96.72   | 98.32  | 92.68 | 90.30 | 84.48 | 86.54 |
| swedish-lines-ud-2.10-220711              | Raw text          | 99.96  | 88.00  | 97.66 | 95.51 | 90.84  | 88.14   | 97.72  | 90.60 | 87.38 | 71.82 | 82.17 |
| swedish-lines-ud-2.10-220711              | Gold tokenization | —      | —      | 97.73 | 95.52 | 90.87  | 88.15   | 97.76  | 91.44 | 88.19 | 72.50 | 82.95 |
| tamil-ttb-ud-2.10-220711                  | Raw text          | 94.26  | 97.52  | 84.29 | 83.18 | 84.64  | 78.22   | 89.45  | 70.43 | 61.88 | 50.61 | 55.39 |
| tamil-ttb-ud-2.10-220711                  | Gold tokenization | —      | —      | 89.29 | 87.78 | 89.99  | 82.70   | 94.42  | 78.13 | 68.78 | 56.87 | 61.48 |
| telugu-mtg-ud-2.10-220711                 | Raw text          | 99.58  | 96.62  | 93.63 | 93.63 | 98.61  | 93.49   | —      | 90.72 | 84.63 | 77.14 | 81.14 |
| telugu-mtg-ud-2.10-220711                 | Gold tokenization | —      | —      | 94.04 | 94.04 | 99.03  | 93.90   | —      | 91.68 | 85.58 | 77.98 | 81.98 |
| turkish-boun-ud-2.10-220711               | Raw text          | 98.83  | 86.93  | 91.56 | 92.51 | 91.72  | 86.56   | 93.23  | 78.48 | 72.40 | 59.77 | 65.11 |
| turkish-boun-ud-2.10-220711               | Gold tokenization | —      | —      | 92.53 | 93.47 | 92.67  | 87.31   | 94.26  | 81.07 | 74.73 | 61.33 | 66.92 |
| turkish-atis-ud-2.10-220711               | Raw text          | 100.00 | 80.20  | 98.96 | —     | 98.46  | 98.25   | 99.15  | 89.22 | 87.49 | 85.12 | 86.08 |
| turkish-atis-ud-2.10-220711               | Gold tokenization | —      | —      | 99.02 | —     | 98.52  | 98.32   | 99.13  | 91.11 | 89.30 | 86.98 | 87.93 |
| turkish-framenet-ud-2.10-220711           | Raw text          | 100.00 | 100.00 | 96.86 | —     | 94.89  | 94.21   | 96.66  | 93.39 | 84.25 | 73.98 | 77.64 |
| turkish-framenet-ud-2.10-220711           | Gold tokenization | —      | —      | 96.86 | —     | 94.89  | 94.21   | 96.66  | 93.39 | 84.25 | 73.98 | 77.64 |
| turkish-imst-ud-2.10-220711               | Raw text          | 98.30  | 96.97  | 94.38 | 93.98 | 90.92  | 88.60   | 94.54  | 74.73 | 69.04 | 58.25 | 63.10 |
| turkish-imst-ud-2.10-220711               | Gold tokenization | —      | —      | 95.94 | 95.49 | 92.40  | 89.97   | 96.13  | 78.07 | 72.09 | 60.26 | 65.33 |
| turkish-kenet-ud-2.10-220711              | Raw text          | 100.00 | 98.12  | 93.71 | —     | 92.05  | 90.86   | 93.33  | 83.91 | 71.18 | 61.81 | 64.77 |
| turkish-kenet-ud-2.10-220711              | Gold tokenization | —      | —      | 93.72 | —     | 92.06  | 90.87   | 93.33  | 84.07 | 71.29 | 61.92 | 64.89 |
| turkish-penn-ud-2.10-220711               | Raw text          | 99.34  | 80.59  | 95.60 | —     | 94.41  | 93.33   | 94.36  | 84.22 | 71.67 | 62.21 | 64.53 |
| turkish-penn-ud-2.10-220711               | Gold tokenization | —      | —      | 96.30 | —     | 95.11  | 94.02   | 95.01  | 86.76 | 73.91 | 63.63 | 66.02 |
| turkish-tourism-ud-2.10-220711            | Raw text          | 99.96  | 99.86  | 98.80 | —     | 95.08  | 94.67   | 98.36  | 97.20 | 91.52 | 81.98 | 87.38 |
| turkish-tourism-ud-2.10-220711            | Gold tokenization | —      | —      | 98.85 | —     | 95.12  | 94.73   | 98.40  | 97.25 | 91.58 | 82.04 | 87.45 |
| turkish_german-sagt-ud-2.10-220711        | Raw text          | 98.91  | 99.44  | 90.21 | —     | 80.32  | 75.60   | 90.82  | 71.14 | 60.98 | 41.12 | 51.00 |
| turkish_german-sagt-ud-2.10-220711        | Gold tokenization | —      | —      | 91.09 | —     | 80.89  | 76.08   | 91.52  | 72.69 | 62.06 | 41.64 | 51.71 |
| ukrainian-iu-ud-2.10-220711               | Raw text          | 99.81  | 96.61  | 97.90 | 94.35 | 94.18  | 93.12   | 97.34  | 90.61 | 88.27 | 78.92 | 83.01 |
| ukrainian-iu-ud-2.10-220711               | Gold tokenization | —      | —      | 98.08 | 94.54 | 94.34  | 93.29   | 97.53  | 91.12 | 88.72 | 79.21 | 83.36 |
| urdu-udtb-ud-2.10-220711                  | Raw text          | 100.00 | 98.31  | 93.91 | 92.15 | 82.83  | 78.40   | 97.41  | 88.15 | 82.49 | 56.62 | 74.68 |
| urdu-udtb-ud-2.10-220711                  | Gold tokenization | —      | —      | 93.93 | 92.17 | 82.86  | 78.43   | 97.41  | 88.23 | 82.58 | 56.67 | 74.77 |
| uyghur-udt-ud-2.10-220711                 | Raw text          | 99.54  | 81.81  | 89.33 | 91.75 | 88.12  | 79.98   | 94.67  | 76.66 | 64.87 | 46.84 | 55.29 |
| uyghur-udt-ud-2.10-220711                 | Gold tokenization | —      | —      | 89.71 | 92.30 | 88.59  | 80.50   | 95.14  | 78.38 | 66.49 | 47.83 | 56.56 |
| vietnamese-vtb-ud-2.10-220711             | Raw text          | 85.37  | 93.46  | 78.21 | 76.76 | 85.12  | 76.57   | 85.16  | 52.68 | 47.84 | 41.55 | 44.29 |
| vietnamese-vtb-ud-2.10-220711             | Gold tokenization | —      | —      | 90.36 | 88.55 | 99.72  | 88.32   | 99.59  | 72.88 | 65.41 | 58.76 | 62.51 |
| welsh-ccg-ud-2.10-220711                  | Raw text          | 99.42  | 97.37  | 95.33 | 94.40 | 89.82  | 87.61   | 93.93  | 86.61 | 80.67 | 63.31 | 69.02 |
| welsh-ccg-ud-2.10-220711                  | Gold tokenization | —      | —      | 95.84 | 94.87 | 90.31  | 88.07   | 94.44  | 87.85 | 81.83 | 64.36 | 70.21 |
| western_armenian-armtdp-ud-2.10-220711    | Raw text          | 99.89  | 98.68  | 96.82 | —     | 92.51  | 91.83   | 97.14  | 89.39 | 84.66 | 69.84 | 76.01 |
| western_armenian-armtdp-ud-2.10-220711    | Gold tokenization | —      | —      | 96.90 | —     | 92.60  | 91.93   | 97.22  | 89.64 | 84.89 | 70.07 | 76.23 |
| wolof-wtb-ud-2.10-220711                  | Raw text          | 99.23  | 91.95  | 94.20 | 94.15 | 93.50  | 91.41   | 95.20  | 84.15 | 78.69 | 66.75 | 70.23 |
| wolof-wtb-ud-2.10-220711                  | Gold tokenization | —      | —      | 95.17 | 95.07 | 94.32  | 92.31   | 95.96  | 86.27 | 80.75 | 68.70 | 72.06 |
