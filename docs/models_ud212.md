## Universal Dependencies 2.12 Models #universal_dependencies_212_models

Universal Dependencies 2.12 Models are distributed under the
[CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The models are based solely on [Universal Dependencies
2.12](http://hdl.handle.net/11234/1-5150) treebanks, and additionally
use [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md)
and [RobeCzech](https://huggingface.co/ufal/robeczech-base).

The models require [UDPipe 2](https://ufal.mff.cuni.cz/udpipe/2).

### Download

The latest version 230717 of the Universal Dependencies 2.12 models
can be downloaded from [LINDAT/CLARIN repository](https://hdl.handle.net/11234/1-5200).

The models are also available in the [REST service](https://lindat.mff.cuni.cz/services/udpipe/).

### Acknowledgements #universal_dependencies_212_models_acknowledgements

This work has been supported by the Ministry of Education, Youth and Sports of
the Czech Republic, Project No. LM2018101 LINDAT/CLARIAH-CZ.

The models were trained on [Universal Dependencies 2.12](https://hdl.handle.net/11234/1-5150) treebanks.

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

The Universal Dependencies 2.12 models contain 131 models of 72 languages, each
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

| Model                                         | Mode              | Words  | Sents  | UPOS  | XPOS  | UFeats | AllTags | Lemma  | UAS   | LAS   | MLAS  | BLEX  |
|:----------------------------------------------|:------------------|-------:|-------:|------:|------:|-------:|--------:|-------:|------:|------:|------:|------:|
| afrikaans-afribooms-ud-2.12-230717            | Raw text          | 99.94  | 99.65  | 98.65 | 95.80 | 98.35  | 95.62   | 98.29  | 90.41 | 87.46 | 78.92 | 79.72 |
| afrikaans-afribooms-ud-2.12-230717            | Gold tokenization | —      | —      | 98.71 | 95.85 | 98.41  | 95.67   | 98.33  | 90.58 | 87.63 | 79.08 | 79.85 |
| ancient_greek-proiel-ud-2.12-230717           | Raw text          | 99.98  | 49.19  | 97.63 | 97.93 | 92.13  | 90.72   | 94.76  | 81.74 | 77.92 | 61.96 | 66.22 |
| ancient_greek-proiel-ud-2.12-230717           | Gold tokenization | —      | —      | 97.87 | 98.14 | 92.58  | 91.30   | 94.82  | 86.59 | 82.72 | 68.31 | 72.13 |
| ancient_greek-perseus-ud-2.12-230717          | Raw text          | 99.97  | 98.85  | 92.98 | 85.76 | 91.42  | 84.89   | 86.72  | 80.13 | 74.55 | 54.46 | 55.76 |
| ancient_greek-perseus-ud-2.12-230717          | Gold tokenization | —      | —      | 93.02 | 85.78 | 91.44  | 84.91   | 86.74  | 80.30 | 74.71 | 54.56 | 55.87 |
| ancient_hebrew-ptnk-ud-2.12-230717            | Raw text          | 71.14  | 98.06  | 69.03 | 69.17 | 67.09  | 66.13   | 67.47  | 48.21 | 46.75 | 31.14 | 32.42 |
| ancient_hebrew-ptnk-ud-2.12-230717            | Gold tokenization | —      | —      | 96.70 | 96.86 | 90.36  | 89.22   | 96.60  | 91.29 | 87.61 | 68.26 | 76.48 |
| arabic-padt-ud-2.12-230717                    | Raw text          | 94.58  | 82.09  | 91.67 | 89.00 | 89.12  | 88.65   | 90.34  | 78.87 | 74.83 | 66.04 | 68.16 |
| arabic-padt-ud-2.12-230717                    | Gold tokenization | —      | —      | 96.96 | 94.34 | 94.50  | 94.01   | 95.20  | 88.24 | 83.65 | 74.73 | 76.32 |
| armenian-armtdp-ud-2.12-230717                | Raw text          | 99.28  | 95.70  | 96.29 | —     | 91.22  | 90.21   | 94.93  | 86.55 | 81.73 | 68.65 | 73.67 |
| armenian-armtdp-ud-2.12-230717                | Gold tokenization | —      | —      | 96.86 | —     | 91.86  | 90.71   | 95.54  | 88.21 | 83.30 | 69.30 | 74.60 |
| armenian-bsut-ud-2.12-230717                  | Raw text          | 99.79  | 98.73  | 97.38 | —     | 91.96  | 91.20   | 96.79  | 90.06 | 85.83 | 71.10 | 79.05 |
| armenian-bsut-ud-2.12-230717                  | Gold tokenization | —      | —      | 97.60 | —     | 92.16  | 91.41   | 96.99  | 90.62 | 86.38 | 71.65 | 79.57 |
| basque-bdt-ud-2.12-230717                     | Raw text          | 99.94  | 99.83  | 96.33 | —     | 93.32  | 91.44   | 96.38  | 87.81 | 84.80 | 74.93 | 79.49 |
| basque-bdt-ud-2.12-230717                     | Gold tokenization | —      | —      | 96.39 | —     | 93.37  | 91.48   | 96.41  | 87.88 | 84.86 | 74.97 | 79.52 |
| belarusian-hse-ud-2.12-230717                 | Raw text          | 99.37  | 86.58  | 98.21 | 97.59 | 94.52  | 93.57   | 93.25  | 87.32 | 85.41 | 76.98 | 76.66 |
| belarusian-hse-ud-2.12-230717                 | Gold tokenization | —      | —      | 98.81 | 98.16 | 95.10  | 94.13   | 93.82  | 89.79 | 87.60 | 78.81 | 78.42 |
| bulgarian-btb-ud-2.12-230717                  | Raw text          | 99.91  | 94.17  | 99.20 | 97.25 | 97.96  | 96.87   | 98.01  | 94.41 | 91.71 | 85.92 | 86.61 |
| bulgarian-btb-ud-2.12-230717                  | Gold tokenization | —      | —      | 99.33 | 97.37 | 98.09  | 96.99   | 98.09  | 95.22 | 92.48 | 86.61 | 87.35 |
| catalan-ancora-ud-2.12-230717                 | Raw text          | 99.95  | 99.08  | 99.11 | 97.25 | 98.72  | 96.98   | 99.40  | 94.92 | 93.43 | 88.06 | 89.51 |
| catalan-ancora-ud-2.12-230717                 | Gold tokenization | —      | —      | 99.17 | 97.36 | 98.79  | 97.09   | 99.46  | 95.08 | 93.59 | 88.27 | 89.70 |
| chinese-gsdsimp-ud-2.12-230717                | Raw text          | 90.29  | 99.10  | 87.23 | 87.15 | 89.70  | 86.47   | 90.23  | 72.74 | 70.28 | 63.38 | 66.93 |
| chinese-gsdsimp-ud-2.12-230717                | Gold tokenization | —      | —      | 96.10 | 96.00 | 99.43  | 95.33   | 99.93  | 87.08 | 83.97 | 78.23 | 82.55 |
| chinese-gsd-ud-2.12-230717                    | Raw text          | 90.27  | 99.10  | 87.18 | 87.09 | 89.68  | 86.42   | 90.20  | 72.57 | 70.14 | 63.02 | 66.73 |
| chinese-gsd-ud-2.12-230717                    | Gold tokenization | —      | —      | 96.16 | 96.05 | 99.41  | 95.38   | 99.92  | 87.18 | 84.07 | 78.07 | 82.55 |
| classical_chinese-kyoto-ud-2.12-230717        | Raw text          | 97.94  | 46.37  | 89.68 | 88.94 | 91.59  | 86.06   | 97.51  | 71.39 | 66.13 | 62.36 | 64.70 |
| classical_chinese-kyoto-ud-2.12-230717        | Gold tokenization | —      | —      | 93.43 | 92.14 | 94.63  | 89.89   | 99.52  | 84.71 | 79.40 | 75.55 | 78.24 |
| coptic-scriptorium-ud-2.12-230717             | Raw text          | 75.26  | 33.63  | 73.02 | 72.92 | 73.15  | 72.08   | 73.90  | 51.82 | 50.05 | 37.34 | 39.84 |
| coptic-scriptorium-ud-2.12-230717             | Gold tokenization | —      | —      | 97.05 | 96.91 | 97.81  | 95.90   | 97.41  | 90.46 | 87.70 | 76.41 | 79.79 |
| croatian-set-ud-2.12-230717                   | Raw text          | 99.93  | 94.79  | 98.49 | 95.82 | 96.31  | 95.53   | 97.78  | 92.41 | 89.57 | 81.89 | 84.77 |
| croatian-set-ud-2.12-230717                   | Gold tokenization | —      | —      | 98.57 | 95.90 | 96.39  | 95.62   | 97.83  | 92.91 | 90.07 | 82.34 | 85.21 |
| czech-pdt-ud-2.12-230717                      | Raw text          | 99.93  | 93.37  | 99.30 | 98.46 | 98.78  | 98.25   | 99.38  | 95.01 | 93.64 | 90.75 | 92.30 |
| czech-pdt-ud-2.12-230717                      | Gold tokenization | —      | —      | 99.38 | 98.55 | 98.87  | 98.34   | 99.46  | 95.81 | 94.43 | 91.41 | 92.97 |
| czech-cac-ud-2.12-230717                      | Raw text          | 99.99  | 99.68  | 99.69 | 98.25 | 98.07  | 97.74   | 99.30  | 96.20 | 94.95 | 90.84 | 93.11 |
| czech-cac-ud-2.12-230717                      | Gold tokenization | —      | —      | 99.70 | 98.26 | 98.08  | 97.74   | 99.31  | 96.20 | 94.95 | 90.86 | 93.13 |
| czech-cltt-ud-2.12-230717                     | Raw text          | 99.32  | 96.92  | 98.95 | 93.72 | 93.99  | 93.65   | 98.48  | 92.47 | 90.77 | 82.87 | 88.60 |
| czech-cltt-ud-2.12-230717                     | Gold tokenization | —      | —      | 99.40 | 94.18 | 94.49  | 94.07   | 99.03  | 93.17 | 91.20 | 82.92 | 88.86 |
| czech-fictree-ud-2.12-230717                  | Raw text          | 99.99  | 98.95  | 99.14 | 97.07 | 97.93  | 96.88   | 99.29  | 96.17 | 94.67 | 89.47 | 92.40 |
| czech-fictree-ud-2.12-230717                  | Gold tokenization | —      | —      | 99.16 | 97.08 | 97.95  | 96.89   | 99.30  | 96.24 | 94.73 | 89.57 | 92.50 |
| danish-ddt-ud-2.12-230717                     | Raw text          | 99.82  | 89.80  | 97.98 | —     | 97.36  | 96.60   | 97.38  | 88.60 | 86.46 | 79.10 | 81.13 |
| danish-ddt-ud-2.12-230717                     | Gold tokenization | —      | —      | 98.17 | —     | 97.59  | 96.83   | 97.52  | 89.82 | 87.64 | 80.18 | 82.15 |
| dutch-alpino-ud-2.12-230717                   | Raw text          | 99.75  | 89.10  | 97.77 | 96.66 | 97.68  | 96.28   | 94.91  | 92.90 | 90.52 | 83.71 | 80.13 |
| dutch-alpino-ud-2.12-230717                   | Gold tokenization | —      | —      | 98.02 | 96.82 | 97.89  | 96.45   | 95.16  | 94.31 | 91.90 | 85.01 | 81.32 |
| dutch-lassysmall-ud-2.12-230717               | Raw text          | 99.77  | 74.71  | 97.33 | 96.15 | 96.86  | 95.50   | 96.18  | 90.99 | 88.49 | 80.77 | 79.51 |
| dutch-lassysmall-ud-2.12-230717               | Gold tokenization | —      | —      | 97.67 | 96.84 | 97.49  | 96.29   | 96.51  | 94.63 | 91.91 | 85.25 | 84.07 |
| english-ewt-ud-2.12-230717                    | Raw text          | 99.09  | 87.82  | 96.69 | 96.32 | 96.70  | 95.01   | 97.26  | 90.71 | 88.81 | 82.26 | 84.29 |
| english-ewt-ud-2.12-230717                    | Gold tokenization | —      | —      | 97.50 | 97.17 | 97.56  | 95.84   | 98.08  | 93.03 | 91.04 | 84.70 | 86.64 |
| english-atis-ud-2.12-230717                   | Raw text          | 99.98  | 80.49  | 99.04 | —     | 98.59  | 98.14   | 99.63  | 94.45 | 93.02 | 87.97 | 90.18 |
| english-atis-ud-2.12-230717                   | Gold tokenization | —      | —      | 99.07 | —     | 98.65  | 98.21   | 99.64  | 96.17 | 94.59 | 90.22 | 92.42 |
| english-eslspok-ud-2.12-230717                | Raw text          | 100.00 | 92.21  | 98.59 | 98.54 | —      | 98.10   | —      | 95.37 | 93.87 | 91.26 | 93.01 |
| english-eslspok-ud-2.12-230717                | Gold tokenization | —      | —      | 98.68 | 98.68 | —      | 98.19   | —      | 96.20 | 94.79 | 91.98 | 93.74 |
| english-gum-ud-2.12-230717                    | Raw text          | 99.63  | 95.72  | 98.00 | 98.04 | 97.91  | 97.02   | 98.84  | 92.87 | 90.96 | 85.35 | 87.10 |
| english-gum-ud-2.12-230717                    | Gold tokenization | —      | —      | 98.34 | 98.39 | 98.27  | 97.36   | 99.15  | 93.72 | 91.79 | 86.11 | 87.79 |
| english-lines-ud-2.12-230717                  | Raw text          | 99.92  | 87.45  | 97.68 | 96.86 | 97.03  | 94.48   | 98.33  | 91.32 | 88.55 | 80.61 | 83.70 |
| english-lines-ud-2.12-230717                  | Gold tokenization | —      | —      | 97.78 | 96.95 | 97.13  | 94.58   | 98.39  | 92.18 | 89.38 | 81.36 | 84.51 |
| english-partut-ud-2.12-230717                 | Raw text          | 99.72  | 100.00 | 97.43 | 97.29 | 96.44  | 95.41   | 98.17  | 94.24 | 92.33 | 83.28 | 87.38 |
| english-partut-ud-2.12-230717                 | Gold tokenization | —      | —      | 97.68 | 97.54 | 96.68  | 95.69   | 98.44  | 94.42 | 92.52 | 83.71 | 87.64 |
| erzya-jr-ud-2.12-230717                       | Raw text          | 99.18  | 94.15  | 87.94 | 87.38 | 78.90  | 73.49   | 84.89  | 72.92 | 63.24 | 41.32 | 48.34 |
| erzya-jr-ud-2.12-230717                       | Gold tokenization | —      | —      | 88.66 | 88.07 | 79.54  | 74.02   | 85.52  | 74.08 | 64.24 | 41.90 | 48.91 |
| estonian-edt-ud-2.12-230717                   | Raw text          | 99.94  | 92.23  | 97.67 | 98.21 | 96.42  | 95.26   | 95.45  | 88.56 | 85.96 | 80.03 | 79.59 |
| estonian-edt-ud-2.12-230717                   | Gold tokenization | —      | —      | 97.79 | 98.26 | 96.50  | 95.38   | 95.52  | 89.43 | 86.81 | 80.81 | 80.32 |
| estonian-ewt-ud-2.12-230717                   | Raw text          | 98.63  | 78.03  | 94.88 | 96.14 | 94.10  | 91.81   | 93.75  | 83.23 | 80.01 | 72.15 | 73.37 |
| estonian-ewt-ud-2.12-230717                   | Gold tokenization | —      | —      | 96.22 | 97.46 | 95.35  | 93.07   | 95.00  | 87.27 | 83.71 | 74.95 | 76.20 |
| faroese-farpahc-ud-2.12-230717                | Raw text          | 99.74  | 92.77  | 97.45 | 93.00 | 94.24  | 92.32   | 99.74  | 86.01 | 82.29 | 68.12 | 75.32 |
| faroese-farpahc-ud-2.12-230717                | Gold tokenization | —      | —      | 97.67 | 93.17 | 94.49  | 92.53   | 100.00 | 86.96 | 83.20 | 69.20 | 76.51 |
| finnish-tdt-ud-2.12-230717                    | Raw text          | 99.70  | 90.82  | 97.67 | 98.31 | 96.08  | 95.23   | 92.11  | 90.42 | 88.48 | 82.43 | 78.28 |
| finnish-tdt-ud-2.12-230717                    | Gold tokenization | —      | —      | 98.00 | 98.59 | 96.39  | 95.54   | 92.39  | 91.72 | 89.75 | 83.44 | 79.26 |
| finnish-ftb-ud-2.12-230717                    | Raw text          | 99.91  | 86.84  | 96.70 | 95.08 | 96.78  | 94.01   | 95.76  | 90.17 | 87.35 | 80.13 | 80.78 |
| finnish-ftb-ud-2.12-230717                    | Gold tokenization | —      | —      | 97.08 | 95.29 | 96.87  | 94.35   | 95.90  | 92.40 | 89.53 | 82.71 | 83.28 |
| french-gsd-ud-2.12-230717                     | Raw text          | 98.84  | 94.93  | 97.33 | —     | 97.25  | 96.57   | 97.72  | 93.15 | 91.20 | 84.70 | 86.92 |
| french-gsd-ud-2.12-230717                     | Gold tokenization | —      | —      | 98.48 | —     | 98.32  | 97.65   | 98.85  | 94.94 | 93.11 | 86.58 | 88.24 |
| french-parisstories-ud-2.12-230717            | Raw text          | 99.73  | 93.08  | 97.20 | —     | 93.07  | 91.20   | 98.02  | 79.94 | 76.66 | 62.32 | 71.73 |
| french-parisstories-ud-2.12-230717            | Gold tokenization | —      | —      | 97.48 | —     | 93.30  | 91.45   | 98.26  | 81.17 | 77.85 | 63.20 | 72.63 |
| french-partut-ud-2.12-230717                  | Raw text          | 99.42  | 98.64  | 97.43 | 96.97 | 95.28  | 94.51   | 97.89  | 94.44 | 92.83 | 82.58 | 87.48 |
| french-partut-ud-2.12-230717                  | Gold tokenization | —      | —      | 98.12 | 97.62 | 95.89  | 95.12   | 98.50  | 95.35 | 93.89 | 83.73 | 88.49 |
| french-rhapsodie-ud-2.12-230717               | Raw text          | 99.16  | 99.82  | 97.31 | 97.37 | 96.16  | 93.38   | 98.19  | 88.06 | 84.92 | 75.51 | 80.31 |
| french-rhapsodie-ud-2.12-230717               | Gold tokenization | —      | —      | 98.19 | 98.11 | 97.02  | 94.15   | 99.00  | 89.33 | 86.14 | 76.49 | 80.94 |
| french-sequoia-ud-2.12-230717                 | Raw text          | 99.15  | 89.53  | 98.40 | —     | 97.19  | 96.84   | 98.30  | 94.06 | 92.75 | 86.39 | 89.39 |
| french-sequoia-ud-2.12-230717                 | Gold tokenization | —      | —      | 99.25 | —     | 98.01  | 97.63   | 99.14  | 95.63 | 94.37 | 88.23 | 90.53 |
| galician-treegal-ud-2.12-230717               | Raw text          | 98.74  | 87.99  | 95.93 | 93.63 | 94.83  | 92.83   | 96.76  | 83.46 | 79.60 | 67.95 | 71.94 |
| galician-treegal-ud-2.12-230717               | Gold tokenization | —      | —      | 97.19 | 94.72 | 95.89  | 93.88   | 97.90  | 86.99 | 82.78 | 71.43 | 75.90 |
| galician-ctg-ud-2.12-230717                   | Raw text          | 99.22  | 97.22  | 97.24 | 97.07 | 99.05  | 96.65   | 98.12  | 85.14 | 82.72 | 71.07 | 75.57 |
| galician-ctg-ud-2.12-230717                   | Gold tokenization | —      | —      | 97.97 | 97.79 | 99.83  | 97.35   | 98.86  | 86.86 | 84.31 | 73.03 | 77.52 |
| german-gsd-ud-2.12-230717                     | Raw text          | 99.76  | 82.68  | 96.16 | 97.53 | 90.78  | 88.15   | 96.91  | 87.04 | 83.20 | 65.39 | 75.45 |
| german-gsd-ud-2.12-230717                     | Gold tokenization | —      | —      | 96.47 | 97.80 | 91.21  | 88.63   | 97.18  | 88.79 | 84.99 | 66.97 | 77.23 |
| german-hdt-ud-2.12-230717                     | Raw text          | 99.90  | 92.39  | 98.55 | 98.46 | 94.21  | 93.81   | 97.69  | 96.90 | 96.00 | 84.87 | 90.50 |
| german-hdt-ud-2.12-230717                     | Gold tokenization | —      | —      | 98.66 | 98.59 | 94.34  | 93.95   | 97.79  | 97.60 | 96.71 | 85.54 | 91.20 |
| gothic-proiel-ud-2.12-230717                  | Raw text          | 100.00 | 31.12  | 96.17 | 96.70 | 89.88  | 87.80   | 94.71  | 78.72 | 72.74 | 58.81 | 63.43 |
| gothic-proiel-ud-2.12-230717                  | Gold tokenization | —      | —      | 96.85 | 97.22 | 91.01  | 89.27   | 94.78  | 86.66 | 80.81 | 68.63 | 72.61 |
| greek-gdt-ud-2.12-230717                      | Raw text          | 99.87  | 90.19  | 98.19 | 98.21 | 95.72  | 95.10   | 96.09  | 92.99 | 91.18 | 81.67 | 81.75 |
| greek-gdt-ud-2.12-230717                      | Gold tokenization | —      | —      | 98.32 | 98.34 | 95.83  | 95.20   | 96.17  | 93.80 | 91.91 | 82.25 | 82.38 |
| greek-gud-ud-2.12-230717                      | Raw text          | 99.92  | 94.98  | 97.01 | 96.26 | 94.24  | 90.55   | 95.76  | 92.94 | 90.06 | 75.94 | 80.42 |
| greek-gud-ud-2.12-230717                      | Gold tokenization | —      | —      | 97.11 | 96.32 | 94.32  | 90.65   | 95.83  | 93.59 | 90.68 | 76.44 | 80.91 |
| hebrew-htb-ud-2.12-230717                     | Raw text          | 85.10  | 99.69  | 82.96 | 82.95 | 81.30  | 80.69   | 83.02  | 70.71 | 68.21 | 55.84 | 60.10 |
| hebrew-htb-ud-2.12-230717                     | Gold tokenization | —      | —      | 97.64 | 97.62 | 95.79  | 95.27   | 97.36  | 92.45 | 89.94 | 79.28 | 82.12 |
| hebrew-iahltwiki-ud-2.12-230717               | Raw text          | 88.54  | 97.16  | 85.97 | 85.97 | 81.45  | 80.46   | 87.15  | 76.11 | 74.26 | 57.99 | 67.30 |
| hebrew-iahltwiki-ud-2.12-230717               | Gold tokenization | —      | —      | 97.09 | 97.09 | 92.18  | 91.10   | 98.29  | 93.66 | 91.26 | 75.14 | 85.53 |
| hindi-hdtb-ud-2.12-230717                     | Raw text          | 100.00 | 98.72  | 97.74 | 97.35 | 94.23  | 92.39   | 98.93  | 95.31 | 92.43 | 79.64 | 87.78 |
| hindi-hdtb-ud-2.12-230717                     | Gold tokenization | —      | —      | 97.74 | 97.34 | 94.25  | 92.40   | 98.94  | 95.43 | 92.55 | 79.77 | 87.94 |
| hungarian-szeged-ud-2.12-230717               | Raw text          | 99.85  | 95.89  | 96.68 | —     | 94.18  | 93.47   | 94.89  | 88.56 | 84.89 | 74.96 | 78.33 |
| hungarian-szeged-ud-2.12-230717               | Gold tokenization | —      | —      | 96.77 | —     | 94.31  | 93.57   | 95.01  | 88.99 | 85.30 | 75.22 | 78.65 |
| icelandic-modern-ud-2.12-230717               | Raw text          | 99.37  | 94.59  | 97.58 | 95.34 | 88.49  | 85.62   | 96.87  | 86.05 | 83.30 | 64.64 | 75.54 |
| icelandic-modern-ud-2.12-230717               | Gold tokenization | —      | —      | 98.15 | 95.92 | 88.93  | 86.07   | 97.45  | 87.03 | 84.17 | 65.30 | 76.47 |
| icelandic-gc-ud-2.12-230717                   | Raw text          | 99.72  | 94.64  | 94.72 | 82.28 | 85.01  | 79.83   | 91.64  | 83.22 | 78.78 | 58.56 | 68.85 |
| icelandic-gc-ud-2.12-230717                   | Gold tokenization | —      | —      | 95.06 | 82.71 | 85.52  | 80.34   | 91.81  | 84.14 | 79.66 | 59.21 | 69.49 |
| icelandic-icepahc-ud-2.12-230717              | Raw text          | 99.80  | 92.67  | 96.90 | 93.31 | 92.01  | 87.13   | 96.24  | 87.30 | 83.46 | 66.91 | 74.57 |
| icelandic-icepahc-ud-2.12-230717              | Gold tokenization | —      | —      | 97.08 | 93.55 | 92.18  | 87.35   | 96.39  | 87.85 | 83.95 | 67.40 | 75.14 |
| indonesian-gsd-ud-2.12-230717                 | Raw text          | 99.49  | 92.35  | 94.35 | 94.03 | 95.77  | 89.10   | 98.12  | 87.62 | 81.71 | 72.43 | 76.99 |
| indonesian-gsd-ud-2.12-230717                 | Gold tokenization | —      | —      | 94.79 | 94.41 | 96.17  | 89.42   | 98.52  | 88.41 | 82.43 | 73.18 | 77.77 |
| indonesian-csui-ud-2.12-230717                | Raw text          | 99.45  | 91.01  | 95.88 | 96.07 | 96.66  | 95.33   | 98.11  | 85.95 | 81.63 | 76.18 | 78.21 |
| indonesian-csui-ud-2.12-230717                | Gold tokenization | —      | —      | 96.34 | 96.58 | 97.15  | 95.78   | 98.74  | 87.26 | 82.71 | 77.14 | 79.20 |
| irish-idt-ud-2.12-230717                      | Raw text          | 99.88  | 97.58  | 96.04 | 94.90 | 90.84  | 87.69   | 95.85  | 86.48 | 80.95 | 64.53 | 71.46 |
| irish-idt-ud-2.12-230717                      | Gold tokenization | —      | —      | 96.13 | 95.08 | 90.97  | 87.85   | 95.97  | 86.75 | 81.19 | 64.56 | 71.53 |
| irish-twittirish-ud-2.12-230717               | Raw text          | 98.50  | 46.62  | 90.58 | —     | —      | 90.58   | 88.41  | 78.58 | 72.34 | 58.38 | 56.97 |
| irish-twittirish-ud-2.12-230717               | Gold tokenization | —      | —      | 91.80 | —     | —      | 91.80   | 89.57  | 85.75 | 79.31 | 66.72 | 64.16 |
| italian-isdt-ud-2.12-230717                   | Raw text          | 99.74  | 99.07  | 98.44 | 98.38 | 98.14  | 97.64   | 98.68  | 94.73 | 93.05 | 86.79 | 88.06 |
| italian-isdt-ud-2.12-230717                   | Gold tokenization | —      | —      | 98.71 | 98.64 | 98.39  | 97.89   | 98.95  | 95.14 | 93.47 | 87.19 | 88.54 |
| italian-markit-ud-2.12-230717                 | Raw text          | 99.62  | 98.24  | 96.96 | 97.13 | 94.12  | 92.60   | 88.18  | 88.60 | 84.72 | 70.64 | 77.87 |
| italian-markit-ud-2.12-230717                 | Gold tokenization | —      | —      | 97.35 | 97.52 | 94.39  | 92.88   | 88.50  | 89.39 | 85.51 | 71.25 | 78.65 |
| italian-parlamint-ud-2.12-230717              | Raw text          | 99.42  | 94.12  | 98.59 | 97.96 | 97.95  | 97.05   | 98.63  | 91.93 | 89.97 | 84.20 | 86.02 |
| italian-parlamint-ud-2.12-230717              | Gold tokenization | —      | —      | 99.17 | 98.52 | 98.50  | 97.58   | 99.16  | 93.41 | 91.44 | 85.80 | 87.60 |
| italian-partut-ud-2.12-230717                 | Raw text          | 99.73  | 100.00 | 98.41 | 98.41 | 98.19  | 97.64   | 98.57  | 96.15 | 94.15 | 87.84 | 88.90 |
| italian-partut-ud-2.12-230717                 | Gold tokenization | —      | —      | 98.52 | 98.52 | 98.27  | 97.72   | 98.82  | 96.18 | 94.09 | 87.68 | 88.80 |
| italian-postwita-ud-2.12-230717               | Raw text          | 99.36  | 49.53  | 96.58 | 96.33 | 96.33  | 94.80   | 96.62  | 82.80 | 79.03 | 68.81 | 70.55 |
| italian-postwita-ud-2.12-230717               | Gold tokenization | —      | —      | 97.17 | 96.95 | 96.90  | 95.44   | 97.25  | 87.96 | 83.79 | 75.19 | 76.95 |
| italian-twittiro-ud-2.12-230717               | Raw text          | 98.94  | 46.67  | 95.91 | 95.74 | 94.94  | 93.42   | 94.57  | 83.09 | 78.65 | 66.30 | 66.97 |
| italian-twittiro-ud-2.12-230717               | Gold tokenization | —      | —      | 96.84 | 96.57 | 95.87  | 94.15   | 95.43  | 88.34 | 83.43 | 71.82 | 72.62 |
| italian-vit-ud-2.12-230717                    | Raw text          | 99.74  | 94.87  | 98.11 | 97.29 | 97.65  | 96.15   | 98.87  | 92.22 | 89.43 | 81.28 | 84.07 |
| italian-vit-ud-2.12-230717                    | Gold tokenization | —      | —      | 98.35 | 97.65 | 97.87  | 96.51   | 99.10  | 92.94 | 90.12 | 81.99 | 84.78 |
| japanese-gsdluw-ud-2.12-230717                | Raw text          | 95.18  | 99.72  | 93.82 | 93.50 | 95.18  | 93.44   | 93.56  | 86.27 | 85.58 | 76.26 | 76.41 |
| japanese-gsdluw-ud-2.12-230717                | Gold tokenization | —      | —      | 98.36 | 98.01 | 100.00 | 97.90   | 97.78  | 95.16 | 94.19 | 86.28 | 84.89 |
| japanese-gsd-ud-2.12-230717                   | Raw text          | 96.17  | 100.00 | 94.97 | 94.18 | 96.16  | 93.85   | 95.03  | 87.91 | 87.07 | 80.80 | 80.98 |
| japanese-gsd-ud-2.12-230717                   | Gold tokenization | —      | —      | 98.59 | 97.52 | 99.99  | 97.20   | 98.47  | 94.93 | 93.94 | 88.80 | 88.47 |
| korean-kaist-ud-2.12-230717                   | Raw text          | 100.00 | 100.00 | 96.19 | 87.78 | —      | 87.58   | 94.18  | 88.85 | 86.92 | 82.77 | 80.35 |
| korean-kaist-ud-2.12-230717                   | Gold tokenization | —      | —      | 96.19 | 87.78 | —      | 87.58   | 94.18  | 88.85 | 86.92 | 82.77 | 80.35 |
| korean-gsd-ud-2.12-230717                     | Raw text          | 99.87  | 93.93  | 96.54 | 90.07 | 99.67  | 87.94   | 93.62  | 87.88 | 83.98 | 80.68 | 76.82 |
| korean-gsd-ud-2.12-230717                     | Gold tokenization | —      | —      | 96.72 | 90.24 | 99.79  | 88.12   | 93.74  | 88.69 | 84.76 | 81.49 | 77.58 |
| latin-ittb-ud-2.12-230717                     | Raw text          | 99.99  | 91.21  | 99.01 | 96.65 | 97.07  | 95.62   | 99.16  | 90.25 | 88.31 | 82.53 | 85.95 |
| latin-ittb-ud-2.12-230717                     | Gold tokenization | —      | —      | 99.03 | 96.66 | 97.12  | 95.64   | 99.18  | 91.28 | 89.35 | 83.17 | 86.55 |
| latin-llct-ud-2.12-230717                     | Raw text          | 99.99  | 99.49  | 99.75 | 97.14 | 97.15  | 96.87   | 97.76  | 95.37 | 94.37 | 89.06 | 90.39 |
| latin-llct-ud-2.12-230717                     | Gold tokenization | —      | —      | 99.75 | 97.14 | 97.16  | 96.87   | 97.77  | 95.39 | 94.39 | 89.07 | 90.41 |
| latin-perseus-ud-2.12-230717                  | Raw text          | 99.95  | 98.99  | 92.88 | 81.11 | 84.60  | 77.45   | 88.86  | 78.92 | 71.78 | 53.50 | 59.27 |
| latin-perseus-ud-2.12-230717                  | Gold tokenization | —      | —      | 92.95 | 81.14 | 84.65  | 77.49   | 88.89  | 79.08 | 71.91 | 53.59 | 59.31 |
| latin-proiel-ud-2.12-230717                   | Raw text          | 99.85  | 37.40  | 96.60 | 96.69 | 90.66  | 89.40   | 96.19  | 76.66 | 72.46 | 59.50 | 64.96 |
| latin-proiel-ud-2.12-230717                   | Gold tokenization | —      | —      | 97.02 | 97.12 | 91.43  | 90.31   | 96.42  | 83.88 | 79.55 | 67.88 | 73.35 |
| latin-udante-ud-2.12-230717                   | Raw text          | 99.61  | 98.81  | 90.94 | 75.50 | 84.14  | 72.23   | 86.97  | 75.88 | 68.63 | 47.63 | 51.46 |
| latin-udante-ud-2.12-230717                   | Gold tokenization | —      | —      | 91.18 | 75.56 | 84.38  | 72.29   | 87.20  | 75.95 | 68.65 | 47.73 | 51.44 |
| latvian-lvtb-ud-2.12-230717                   | Raw text          | 99.29  | 97.80  | 96.79 | 90.92 | 94.75  | 90.16   | 96.57  | 88.79 | 85.85 | 77.56 | 81.04 |
| latvian-lvtb-ud-2.12-230717                   | Gold tokenization | —      | —      | 97.44 | 91.56 | 95.44  | 90.77   | 97.21  | 89.95 | 86.94 | 78.74 | 82.16 |
| lithuanian-alksnis-ud-2.12-230717             | Raw text          | 99.91  | 87.87  | 95.95 | 90.31 | 91.09  | 89.50   | 93.45  | 82.74 | 78.94 | 68.03 | 71.20 |
| lithuanian-alksnis-ud-2.12-230717             | Gold tokenization | —      | —      | 96.08 | 90.44 | 91.27  | 89.64   | 93.56  | 83.94 | 80.08 | 68.93 | 72.06 |
| lithuanian-hse-ud-2.12-230717                 | Raw text          | 97.30  | 97.30  | 89.93 | 90.03 | 81.92  | 79.03   | 88.16  | 71.85 | 62.63 | 44.48 | 53.82 |
| lithuanian-hse-ud-2.12-230717                 | Gold tokenization | —      | —      | 91.32 | 91.42 | 83.40  | 80.09   | 90.75  | 75.00 | 65.00 | 45.87 | 55.84 |
| maghrebi_arabic_french-arabizi-ud-2.12-230717 | Raw text          | 91.65  | 7.00   | 78.81 | 71.59 | 82.65  | 69.81   | 50.63  | 57.90 | 49.93 | 36.22 | 24.59 |
| maghrebi_arabic_french-arabizi-ud-2.12-230717 | Gold tokenization | —      | —      | 86.69 | 78.89 | 90.82  | 77.88   | 54.66  | 76.32 | 65.86 | 47.72 | 31.57 |
| maltese-mudt-ud-2.12-230717                   | Raw text          | 99.84  | 86.29  | 95.73 | 95.79 | —      | 95.31   | —      | 85.08 | 80.25 | 68.88 | 73.05 |
| maltese-mudt-ud-2.12-230717                   | Gold tokenization | —      | —      | 95.87 | 95.92 | —      | 95.46   | —      | 85.70 | 80.81 | 69.28 | 73.43 |
| manx-cadhan-ud-2.12-230717                    | Raw text          | 97.36  | 98.25  | 94.18 | —     | 95.78  | 93.37   | 93.43  | 87.42 | 84.02 | 77.75 | 78.03 |
| manx-cadhan-ud-2.12-230717                    | Gold tokenization | —      | —      | 96.77 | —     | 98.39  | 95.93   | 95.98  | 92.47 | 89.13 | 83.16 | 82.07 |
| marathi-ufal-ud-2.12-230717                   | Raw text          | 94.16  | 92.63  | 82.73 | —     | 74.21  | 70.80   | 84.91  | 68.13 | 60.83 | 39.75 | 47.62 |
| marathi-ufal-ud-2.12-230717                   | Gold tokenization | —      | —      | 87.14 | —     | 76.94  | 73.06   | 87.86  | 73.79 | 65.29 | 41.82 | 50.10 |
| naija-nsc-ud-2.12-230717                      | Raw text          | 99.95  | 100.00 | 98.04 | —     | 98.92  | 97.53   | 99.33  | 93.02 | 90.46 | 87.48 | 88.98 |
| naija-nsc-ud-2.12-230717                      | Gold tokenization | —      | —      | 98.08 | —     | 98.96  | 97.56   | 99.39  | 93.10 | 90.53 | 87.55 | 89.03 |
| north_sami-giella-ud-2.12-230717              | Raw text          | 99.87  | 98.79  | 91.63 | 93.51 | 89.29  | 85.24   | 86.91  | 75.78 | 70.85 | 60.16 | 58.42 |
| north_sami-giella-ud-2.12-230717              | Gold tokenization | —      | —      | 91.78 | 93.65 | 89.42  | 85.38   | 87.01  | 76.01 | 71.08 | 60.38 | 58.60 |
| norwegian-bokmaal-ud-2.12-230717              | Raw text          | 99.82  | 97.27  | 98.38 | 98.94 | 97.47  | 96.81   | 98.58  | 93.88 | 92.64 | 86.95 | 88.78 |
| norwegian-bokmaal-ud-2.12-230717              | Gold tokenization | —      | —      | 98.58 | 99.14 | 97.65  | 97.01   | 98.78  | 94.54 | 93.28 | 87.54 | 89.41 |
| norwegian-nynorsk-ud-2.12-230717              | Raw text          | 99.93  | 94.54  | 98.43 | 99.16 | 97.35  | 96.55   | 98.46  | 93.79 | 92.37 | 85.84 | 88.08 |
| norwegian-nynorsk-ud-2.12-230717              | Gold tokenization | —      | —      | 98.62 | 99.27 | 97.51  | 96.75   | 98.60  | 94.58 | 93.19 | 86.82 | 89.08 |
| old_church_slavonic-proiel-ud-2.12-230717     | Raw text          | 100.00 | 40.05  | 96.11 | 96.39 | 89.72  | 88.01   | 90.18  | 78.06 | 73.64 | 60.96 | 61.97 |
| old_church_slavonic-proiel-ud-2.12-230717     | Gold tokenization | —      | —      | 96.66 | 96.98 | 90.32  | 88.99   | 90.19  | 85.05 | 80.53 | 68.59 | 69.15 |
| old_east_slavic-torot-ud-2.12-230717          | Raw text          | 100.00 | 34.53  | 95.40 | 95.48 | 89.92  | 87.64   | 88.09  | 76.94 | 72.12 | 58.23 | 57.89 |
| old_east_slavic-torot-ud-2.12-230717          | Gold tokenization | —      | —      | 95.89 | 95.94 | 90.67  | 88.72   | 88.14  | 85.27 | 80.38 | 67.99 | 65.94 |
| old_east_slavic-birchbark-ud-2.12-230717      | Raw text          | 99.99  | 16.66  | 88.50 | 99.37 | 76.09  | 72.03   | 65.58  | 64.39 | 57.66 | 32.84 | 27.49 |
| old_east_slavic-birchbark-ud-2.12-230717      | Gold tokenization | —      | —      | 89.13 | 99.38 | 76.70  | 72.84   | 65.59  | 76.43 | 69.19 | 41.15 | 32.90 |
| old_east_slavic-rnc-ud-2.12-230717            | Raw text          | 97.64  | 60.48  | 92.65 | 86.46 | 78.10  | 68.76   | 77.22  | 64.58 | 60.03 | 36.90 | 37.52 |
| old_east_slavic-rnc-ud-2.12-230717            | Gold tokenization | —      | —      | 93.99 | 88.74 | 79.24  | 69.78   | 78.28  | 70.79 | 65.17 | 39.63 | 40.04 |
| old_french-srcmf-ud-2.12-230717               | Raw text          | 99.70  | 100.00 | 96.74 | 96.58 | 97.76  | 95.78   | 99.66  | 90.92 | 87.17 | 80.65 | 84.10 |
| old_french-srcmf-ud-2.12-230717               | Gold tokenization | —      | —      | 97.06 | 96.91 | 98.07  | 96.08   | 99.96  | 91.34 | 87.62 | 81.11 | 84.56 |
| persian-perdt-ud-2.12-230717                  | Raw text          | 99.66  | 99.83  | 97.51 | 97.39 | 97.64  | 95.63   | 98.91  | 93.56 | 91.32 | 86.30 | 88.68 |
| persian-perdt-ud-2.12-230717                  | Gold tokenization | —      | —      | 97.81 | 97.68 | 97.94  | 95.92   | 99.23  | 94.08 | 91.84 | 86.86 | 89.27 |
| persian-seraji-ud-2.12-230717                 | Raw text          | 99.65  | 98.75  | 97.89 | 97.91 | 97.90  | 97.44   | 98.26  | 91.78 | 88.92 | 84.47 | 84.56 |
| persian-seraji-ud-2.12-230717                 | Gold tokenization | —      | —      | 98.22 | 98.23 | 98.22  | 97.74   | 98.54  | 92.47 | 89.62 | 85.12 | 85.22 |
| polish-pdb-ud-2.12-230717                     | Raw text          | 99.85  | 97.33  | 98.86 | 95.76 | 95.89  | 95.08   | 98.08  | 94.21 | 92.16 | 85.14 | 88.25 |
| polish-pdb-ud-2.12-230717                     | Gold tokenization | —      | —      | 99.02 | 95.92 | 96.04  | 95.22   | 98.21  | 94.72 | 92.67 | 85.55 | 88.68 |
| polish-lfg-ud-2.12-230717                     | Raw text          | 99.85  | 99.65  | 98.97 | 96.08 | 96.49  | 95.15   | 98.24  | 96.78 | 95.40 | 89.61 | 92.26 |
| polish-lfg-ud-2.12-230717                     | Gold tokenization | —      | —      | 99.13 | 96.26 | 96.67  | 95.32   | 98.38  | 97.19 | 95.81 | 90.02 | 92.60 |
| pomak-philotis-ud-2.12-230717                 | Raw text          | 99.98  | 94.49  | 98.80 | —     | 95.54  | 95.26   | 96.71  | 88.17 | 83.04 | 70.60 | 73.82 |
| pomak-philotis-ud-2.12-230717                 | Gold tokenization | —      | —      | 98.82 | —     | 95.54  | 95.26   | 96.73  | 88.63 | 83.54 | 71.09 | 74.23 |
| portuguese-bosque-ud-2.12-230717              | Raw text          | 99.68  | 89.73  | 97.88 | —     | 96.86  | 95.87   | 98.27  | 92.13 | 89.83 | 80.75 | 84.19 |
| portuguese-bosque-ud-2.12-230717              | Gold tokenization | —      | —      | 98.16 | —     | 97.11  | 96.11   | 98.57  | 93.28 | 90.94 | 81.78 | 85.38 |
| portuguese-cintil-ud-2.12-230717              | Raw text          | 99.41  | 78.66  | 97.42 | 96.01 | 95.29  | 93.21   | 97.66  | 85.13 | 81.88 | 71.77 | 75.51 |
| portuguese-cintil-ud-2.12-230717              | Gold tokenization | —      | —      | 98.00 | 96.61 | 95.91  | 93.80   | 98.23  | 87.54 | 84.20 | 74.06 | 77.83 |
| portuguese-petrogold-ud-2.12-230717           | Raw text          | 99.59  | 93.11  | 98.75 | —     | 98.70  | 98.20   | 99.09  | 94.69 | 93.57 | 88.77 | 90.11 |
| portuguese-petrogold-ud-2.12-230717           | Gold tokenization | —      | —      | 99.05 | —     | 99.00  | 98.46   | 99.51  | 95.62 | 94.42 | 89.68 | 91.18 |
| romanian-rrt-ud-2.12-230717                   | Raw text          | 99.70  | 95.50  | 97.88 | 97.14 | 97.39  | 96.91   | 98.00  | 91.92 | 88.46 | 81.93 | 83.37 |
| romanian-rrt-ud-2.12-230717                   | Gold tokenization | —      | —      | 98.15 | 97.40 | 97.65  | 97.16   | 98.25  | 92.77 | 89.25 | 82.56 | 83.97 |
| romanian-nonstandard-ud-2.12-230717           | Raw text          | 98.83  | 96.77  | 96.12 | 91.86 | 90.52  | 89.16   | 94.82  | 88.67 | 84.67 | 68.20 | 76.17 |
| romanian-nonstandard-ud-2.12-230717           | Gold tokenization | —      | —      | 97.25 | 92.87 | 91.51  | 90.13   | 95.88  | 90.46 | 86.41 | 69.79 | 77.51 |
| romanian-simonero-ud-2.12-230717              | Raw text          | 99.84  | 100.00 | 98.41 | 97.97 | 97.51  | 97.20   | 98.89  | 93.95 | 92.03 | 85.30 | 88.19 |
| romanian-simonero-ud-2.12-230717              | Gold tokenization | —      | —      | 98.56 | 98.12 | 97.66  | 97.34   | 99.04  | 94.29 | 92.35 | 85.57 | 88.47 |
| russian-syntagrus-ud-2.12-230717              | Raw text          | 99.67  | 98.31  | 98.50 | —     | 94.02  | 93.76   | 98.19  | 93.80 | 91.66 | 82.80 | 88.87 |
| russian-syntagrus-ud-2.12-230717              | Gold tokenization | —      | —      | 98.83 | —     | 94.33  | 94.07   | 98.48  | 94.51 | 92.33 | 83.33 | 89.40 |
| russian-gsd-ud-2.12-230717                    | Raw text          | 99.50  | 96.49  | 98.05 | 97.56 | 94.57  | 93.51   | 96.87  | 91.64 | 88.70 | 80.95 | 84.53 |
| russian-gsd-ud-2.12-230717                    | Gold tokenization | —      | —      | 98.50 | 97.97 | 95.01  | 93.90   | 97.25  | 92.83 | 89.81 | 81.85 | 85.46 |
| russian-taiga-ud-2.12-230717                  | Raw text          | 98.07  | 86.01  | 95.59 | —     | 93.02  | 92.05   | 94.62  | 82.89 | 79.47 | 70.47 | 73.61 |
| russian-taiga-ud-2.12-230717                  | Gold tokenization | —      | —      | 97.25 | —     | 94.80  | 93.67   | 96.34  | 85.53 | 81.91 | 72.70 | 76.02 |
| sanskrit-vedic-ud-2.12-230717                 | Raw text          | 100.00 | 27.18  | 89.20 | —     | 81.19  | 76.40   | 87.11  | 61.05 | 50.02 | 41.42 | 44.77 |
| sanskrit-vedic-ud-2.12-230717                 | Gold tokenization | —      | —      | 90.07 | —     | 82.70  | 78.17   | 87.40  | 73.69 | 61.55 | 51.56 | 54.91 |
| scottish_gaelic-arcosg-ud-2.12-230717         | Raw text          | 97.43  | 61.26  | 93.66 | 89.50 | 91.07  | 88.34   | 94.92  | 80.79 | 76.33 | 64.78 | 69.63 |
| scottish_gaelic-arcosg-ud-2.12-230717         | Gold tokenization | —      | —      | 96.40 | 92.49 | 94.04  | 91.41   | 97.47  | 87.25 | 82.84 | 71.75 | 76.18 |
| serbian-set-ud-2.12-230717                    | Raw text          | 99.99  | 93.00  | 99.03 | 95.94 | 96.13  | 95.71   | 97.82  | 93.48 | 90.99 | 83.45 | 86.89 |
| serbian-set-ud-2.12-230717                    | Gold tokenization | —      | —      | 99.05 | 95.95 | 96.16  | 95.74   | 97.83  | 94.18 | 91.65 | 84.17 | 87.56 |
| slovak-snk-ud-2.12-230717                     | Raw text          | 100.00 | 81.69  | 97.68 | 90.26 | 93.35  | 89.42   | 96.51  | 91.48 | 89.73 | 80.19 | 84.62 |
| slovak-snk-ud-2.12-230717                     | Gold tokenization | —      | —      | 97.83 | 90.34 | 93.44  | 89.56   | 96.54  | 93.88 | 92.00 | 82.42 | 86.89 |
| slovenian-ssj-ud-2.12-230717                  | Raw text          | 99.94  | 98.95  | 98.96 | 97.09 | 97.26  | 96.78   | 98.57  | 94.23 | 92.86 | 87.37 | 89.22 |
| slovenian-ssj-ud-2.12-230717                  | Gold tokenization | —      | —      | 99.02 | 97.15 | 97.33  | 96.84   | 98.61  | 94.40 | 93.02 | 87.52 | 89.34 |
| slovenian-sst-ud-2.12-230717                  | Raw text          | 99.97  | 24.74  | 95.82 | 93.37 | 93.58  | 91.64   | 97.69  | 66.76 | 61.98 | 52.04 | 55.84 |
| slovenian-sst-ud-2.12-230717                  | Gold tokenization | —      | —      | 96.10 | 93.49 | 93.79  | 91.91   | 97.73  | 78.52 | 73.20 | 63.83 | 68.34 |
| spanish-ancora-ud-2.12-230717                 | Raw text          | 99.95  | 98.78  | 99.06 | 96.12 | 98.76  | 95.71   | 99.39  | 93.68 | 91.92 | 86.79 | 88.29 |
| spanish-ancora-ud-2.12-230717                 | Gold tokenization | —      | —      | 99.11 | 96.16 | 98.81  | 95.75   | 99.43  | 93.87 | 92.10 | 86.95 | 88.45 |
| spanish-gsd-ud-2.12-230717                    | Raw text          | 99.72  | 94.90  | 97.10 | —     | 96.74  | 95.07   | 98.58  | 92.51 | 90.28 | 78.76 | 84.27 |
| spanish-gsd-ud-2.12-230717                    | Gold tokenization | —      | —      | 97.36 | —     | 97.01  | 95.32   | 98.83  | 93.38 | 91.10 | 79.58 | 85.17 |
| swedish-talbanken-ud-2.12-230717              | Raw text          | 99.84  | 96.53  | 98.37 | 97.23 | 97.31  | 96.43   | 98.17  | 92.08 | 89.72 | 83.69 | 85.90 |
| swedish-talbanken-ud-2.12-230717              | Gold tokenization | —      | —      | 98.53 | 97.42 | 97.49  | 96.61   | 98.33  | 92.53 | 90.15 | 84.20 | 86.44 |
| swedish-lines-ud-2.12-230717                  | Raw text          | 99.96  | 88.00  | 97.61 | 95.49 | 90.93  | 88.18   | 97.82  | 90.48 | 87.14 | 71.66 | 81.94 |
| swedish-lines-ud-2.12-230717                  | Gold tokenization | —      | —      | 97.71 | 95.50 | 90.96  | 88.23   | 97.86  | 91.19 | 87.78 | 72.25 | 82.63 |
| tamil-ttb-ud-2.12-230717                      | Raw text          | 94.26  | 97.52  | 84.29 | 82.92 | 84.09  | 77.76   | 88.79  | 70.63 | 61.98 | 50.53 | 54.69 |
| tamil-ttb-ud-2.12-230717                      | Gold tokenization | —      | —      | 89.14 | 87.13 | 89.19  | 81.80   | 93.87  | 78.38 | 68.88 | 56.48 | 60.89 |
| telugu-mtg-ud-2.12-230717                     | Raw text          | 99.58  | 96.62  | 92.94 | 92.94 | 98.61  | 92.94   | —      | 90.72 | 84.07 | 76.19 | 80.19 |
| telugu-mtg-ud-2.12-230717                     | Gold tokenization | —      | —      | 93.48 | 93.48 | 99.03  | 93.48   | —      | 91.68 | 85.02 | 77.22 | 81.03 |
| turkish-boun-ud-2.12-230717                   | Raw text          | 96.57  | 86.25  | 90.03 | 86.05 | 81.12  | 71.69   | 90.47  | 73.32 | 67.11 | 49.09 | 60.77 |
| turkish-boun-ud-2.12-230717                   | Gold tokenization | —      | —      | 92.97 | 88.91 | 83.07  | 73.35   | 93.52  | 80.79 | 73.91 | 52.78 | 66.50 |
| turkish-atis-ud-2.12-230717                   | Raw text          | 100.00 | 80.20  | 99.02 | —     | 98.57  | 98.40   | 99.11  | 89.03 | 87.24 | 84.91 | 85.71 |
| turkish-atis-ud-2.12-230717                   | Gold tokenization | —      | —      | 99.04 | —     | 98.57  | 98.42   | 99.11  | 90.96 | 89.09 | 86.83 | 87.69 |
| turkish-framenet-ud-2.12-230717               | Raw text          | 100.00 | 100.00 | 96.52 | —     | 94.75  | 93.87   | 96.39  | 93.73 | 84.80 | 74.12 | 77.96 |
| turkish-framenet-ud-2.12-230717               | Gold tokenization | —      | —      | 96.52 | —     | 94.75  | 93.87   | 96.39  | 93.73 | 84.80 | 74.12 | 77.96 |
| turkish-imst-ud-2.12-230717                   | Raw text          | 97.94  | 97.70  | 93.70 | 93.46 | 90.63  | 88.13   | 94.46  | 75.22 | 66.28 | 55.69 | 60.80 |
| turkish-imst-ud-2.12-230717                   | Gold tokenization | —      | —      | 95.46 | 95.27 | 92.41  | 89.75   | 96.32  | 78.94 | 69.42 | 57.62 | 62.96 |
| turkish-kenet-ud-2.12-230717                  | Raw text          | 100.00 | 98.12  | 93.80 | —     | 92.10  | 90.85   | 93.50  | 83.94 | 71.51 | 62.32 | 65.35 |
| turkish-kenet-ud-2.12-230717                  | Gold tokenization | —      | —      | 93.83 | —     | 92.12  | 90.89   | 93.51  | 84.10 | 71.63 | 62.46 | 65.49 |
| turkish-penn-ud-2.12-230717                   | Raw text          | 99.34  | 80.59  | 95.50 | —     | 94.48  | 93.29   | 94.14  | 84.40 | 71.73 | 62.08 | 64.36 |
| turkish-penn-ud-2.12-230717                   | Gold tokenization | —      | —      | 96.14 | —     | 95.12  | 93.94   | 94.71  | 86.84 | 73.89 | 63.46 | 65.77 |
| turkish-tourism-ud-2.12-230717                | Raw text          | 99.96  | 99.86  | 98.92 | —     | 94.98  | 94.67   | 98.27  | 97.04 | 91.43 | 81.58 | 87.09 |
| turkish-tourism-ud-2.12-230717                | Gold tokenization | —      | —      | 98.96 | —     | 95.02  | 94.72   | 98.31  | 97.10 | 91.50 | 81.66 | 87.18 |
| turkish_german-sagt-ud-2.12-230717            | Raw text          | 98.91  | 99.44  | 90.21 | —     | 80.24  | 75.45   | 90.80  | 71.42 | 61.22 | 41.16 | 50.92 |
| turkish_german-sagt-ud-2.12-230717            | Gold tokenization | —      | —      | 91.09 | —     | 80.82  | 75.93   | 91.49  | 72.76 | 62.20 | 41.63 | 51.55 |
| ukrainian-iu-ud-2.12-230717                   | Raw text          | 99.81  | 96.23  | 97.84 | 94.28 | 94.25  | 93.16   | 97.47  | 90.37 | 87.94 | 78.30 | 82.74 |
| ukrainian-iu-ud-2.12-230717                   | Gold tokenization | —      | —      | 98.03 | 94.44 | 94.39  | 93.31   | 97.67  | 90.97 | 88.52 | 78.70 | 83.18 |
| urdu-udtb-ud-2.12-230717                      | Raw text          | 100.00 | 98.31  | 94.09 | 92.20 | 82.76  | 78.43   | 97.41  | 88.02 | 82.68 | 57.25 | 74.71 |
| urdu-udtb-ud-2.12-230717                      | Gold tokenization | —      | —      | 94.06 | 92.19 | 82.76  | 78.41   | 97.41  | 88.13 | 82.81 | 57.30 | 74.88 |
| uyghur-udt-ud-2.12-230717                     | Raw text          | 99.54  | 81.87  | 89.77 | 91.72 | 88.23  | 80.82   | 94.71  | 75.32 | 64.44 | 50.04 | 57.14 |
| uyghur-udt-ud-2.12-230717                     | Gold tokenization | —      | —      | 90.23 | 92.21 | 88.65  | 81.27   | 95.22  | 77.05 | 66.02 | 50.95 | 58.33 |
| vietnamese-vtb-ud-2.12-230717                 | Raw text          | 86.06  | 93.73  | 78.61 | 77.61 | —      | 77.50   | 85.76  | 56.86 | 50.02 | 41.40 | 45.54 |
| vietnamese-vtb-ud-2.12-230717                 | Gold tokenization | —      | —      | 90.02 | 88.88 | —      | 88.73   | 99.50  | 76.31 | 65.90 | 55.36 | 61.24 |
| welsh-ccg-ud-2.12-230717                      | Raw text          | 99.46  | 97.68  | 95.27 | 94.28 | 89.57  | 87.25   | 94.43  | 86.73 | 80.81 | 63.09 | 69.84 |
| welsh-ccg-ud-2.12-230717                      | Gold tokenization | —      | —      | 95.74 | 94.73 | 90.03  | 87.66   | 94.94  | 87.83 | 81.85 | 64.04 | 70.94 |
| western_armenian-armtdp-ud-2.12-230717        | Raw text          | 99.89  | 98.68  | 96.67 | —     | 92.31  | 91.63   | 97.14  | 89.26 | 84.68 | 69.80 | 76.24 |
| western_armenian-armtdp-ud-2.12-230717        | Gold tokenization | —      | —      | 96.75 | —     | 92.40  | 91.71   | 97.23  | 89.51 | 84.91 | 69.99 | 76.44 |
| wolof-wtb-ud-2.12-230717                      | Raw text          | 99.23  | 91.95  | 94.16 | 94.17 | 93.56  | 91.48   | 95.18  | 83.75 | 78.61 | 66.55 | 69.91 |
| wolof-wtb-ud-2.12-230717                      | Gold tokenization | —      | —      | 95.11 | 95.08 | 94.34  | 92.33   | 95.88  | 85.90 | 80.57 | 68.67 | 71.71 |
