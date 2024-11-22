## Universal Dependencies 2.15 Models #universal_dependencies_215_models

Universal Dependencies 2.15 Models are distributed under the
[CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The models are based solely on [Universal Dependencies
2.15](https://hdl.handle.net/11234/1-5787) treebanks, and additionally
use [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md)
and [RobeCzech](https://huggingface.co/ufal/robeczech-base).

The models require [UDPipe 2](https://ufal.mff.cuni.cz/udpipe/2).

### Download

The latest version 241121 of the Universal Dependencies 2.15 models
can be downloaded from [LINDAT/CLARIN repository](https://hdl.handle.net/11234/1-5797).

The models are also available in the [REST service](https://lindat.mff.cuni.cz/services/udpipe/).

### Acknowledgements #universal_dependencies_215_models_acknowledgements

This work has been supported by the Ministry of Education, Youth and Sports of
the Czech Republic, Project No. LM2023062 LINDAT/CLARIAH-CZ.

The models were trained on [Universal Dependencies 2.15](https://hdl.handle.net/11234/1-5787) treebanks.

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

The Universal Dependencies 2.15 models contain 147 models of 78 languages, each
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

| Model                                         | Mode              | Words  | Sents  | UPOS  | XPOS   | UFeats | AllTags | Lemma  | UAS   | LAS   | MLAS  | BLEX  |
|:----------------------------------------------|:------------------|-------:|-------:|------:|-------:|-------:|--------:|-------:|------:|------:|------:|------:|
| afrikaans-afribooms-ud-2.15-241121            | Raw text          | 99.94  | 99.65  | 98.68 | 95.67  | 98.30  | 95.48   | 98.38  | 90.38 | 87.46 | 78.88 | 79.86 |
| afrikaans-afribooms-ud-2.15-241121            | Gold tokenization | —      | —      | 98.74 | 95.72  | 98.36  | 95.53   | 98.42  | 90.59 | 87.67 | 79.10 | 80.02 |
| albanian-staf-ud-2.15-241121                  | Raw text          | 98.39  | 92.68  | 88.98 | 98.39  | 70.16  | 68.01   | 82.80  | 77.42 | 66.40 | 34.15 | 40.49 |
| albanian-staf-ud-2.15-241121                  | Gold tokenization | —      | —      | 89.52 | 100.00 | 70.43  | 68.01   | 83.60  | 77.69 | 66.13 | 33.98 | 41.75 |
| ancient_greek-proiel-ud-2.15-241121           | Raw text          | 99.98  | 49.19  | 97.68 | 97.92  | 92.04  | 90.69   | 94.65  | 82.00 | 78.22 | 62.39 | 66.26 |
| ancient_greek-proiel-ud-2.15-241121           | Gold tokenization | —      | —      | 97.88 | 98.08  | 92.38  | 91.20   | 94.68  | 86.76 | 82.97 | 68.47 | 72.07 |
| ancient_greek-perseus-ud-2.15-241121          | Raw text          | 99.97  | 98.85  | 93.08 | 86.04  | 91.66  | 85.23   | 86.68  | 80.03 | 74.40 | 54.44 | 55.56 |
| ancient_greek-perseus-ud-2.15-241121          | Gold tokenization | —      | —      | 93.15 | 86.11  | 91.69  | 85.29   | 86.71  | 80.22 | 74.57 | 54.61 | 55.72 |
| ancient_greek-ptnk-ud-2.15-241121             | Raw text          | 99.97  | 55.12  | 98.49 | —      | 90.24  | 89.18   | 95.04  | 87.92 | 83.91 | 65.12 | 72.02 |
| ancient_greek-ptnk-ud-2.15-241121             | Gold tokenization | —      | —      | 98.47 | —      | 90.36  | 89.27   | 95.09  | 91.59 | 87.36 | 68.22 | 75.12 |
| ancient_hebrew-ptnk-ud-2.15-241121            | Raw text          | 76.05  | 98.06  | 74.12 | 74.17  | 73.42  | 72.32   | 72.23  | 55.78 | 54.01 | 39.71 | 40.12 |
| ancient_hebrew-ptnk-ud-2.15-241121            | Gold tokenization | —      | —      | 97.09 | 97.25  | 95.49  | 94.07   | 92.50  | 92.11 | 88.68 | 75.55 | 72.43 |
| arabic-padt-ud-2.15-241121                    | Raw text          | 94.58  | 82.09  | 91.70 | 89.04  | 89.15  | 88.68   | 90.31  | 78.66 | 74.79 | 66.09 | 68.16 |
| arabic-padt-ud-2.15-241121                    | Gold tokenization | —      | —      | 96.99 | 94.37  | 94.54  | 94.01   | 95.26  | 88.14 | 83.66 | 74.87 | 76.34 |
| armenian-armtdp-ud-2.15-241121                | Raw text          | 99.28  | 95.70  | 96.25 | —      | 91.90  | 90.65   | 95.14  | 86.90 | 82.10 | 69.44 | 73.94 |
| armenian-armtdp-ud-2.15-241121                | Gold tokenization | —      | —      | 96.86 | —      | 92.53  | 91.22   | 95.73  | 88.46 | 83.56 | 70.13 | 74.73 |
| armenian-bsut-ud-2.15-241121                  | Raw text          | 99.79  | 98.73  | 97.30 | —      | 92.14  | 91.36   | 96.68  | 90.08 | 85.52 | 71.11 | 78.28 |
| armenian-bsut-ud-2.15-241121                  | Gold tokenization | —      | —      | 97.52 | —      | 92.35  | 91.58   | 96.89  | 90.59 | 86.02 | 71.60 | 78.76 |
| basque-bdt-ud-2.15-241121                     | Raw text          | 99.97  | 99.83  | 96.27 | —      | 93.34  | 91.43   | 96.33  | 88.11 | 84.98 | 74.95 | 79.47 |
| basque-bdt-ud-2.15-241121                     | Gold tokenization | —      | —      | 96.30 | —      | 93.37  | 91.45   | 96.34  | 88.16 | 85.03 | 74.99 | 79.50 |
| belarusian-hse-ud-2.15-241121                 | Raw text          | 99.37  | 86.58  | 98.22 | 97.62  | 94.51  | 93.65   | 93.09  | 87.06 | 85.09 | 76.62 | 76.27 |
| belarusian-hse-ud-2.15-241121                 | Gold tokenization | —      | —      | 98.84 | 98.21  | 95.17  | 94.26   | 93.69  | 89.64 | 87.43 | 78.58 | 78.05 |
| bulgarian-btb-ud-2.15-241121                  | Raw text          | 99.91  | 94.17  | 99.17 | 97.27  | 97.95  | 96.85   | 98.00  | 94.49 | 91.80 | 85.90 | 86.51 |
| bulgarian-btb-ud-2.15-241121                  | Gold tokenization | —      | —      | 99.29 | 97.38  | 98.05  | 96.96   | 98.10  | 95.31 | 92.57 | 86.55 | 87.25 |
| catalan-ancora-ud-2.15-241121                 | Raw text          | 99.94  | 99.49  | 99.09 | 97.18  | 98.69  | 96.92   | 99.43  | 94.77 | 93.22 | 87.87 | 89.32 |
| catalan-ancora-ud-2.15-241121                 | Gold tokenization | —      | —      | 99.17 | 97.28  | 98.76  | 97.01   | 99.49  | 94.93 | 93.37 | 88.05 | 89.49 |
| chinese-gsdsimp-ud-2.15-241121                | Raw text          | 90.29  | 99.10  | 86.92 | 87.19  | 89.68  | 86.49   | 90.21  | 72.55 | 70.08 | 62.74 | 66.30 |
| chinese-gsdsimp-ud-2.15-241121                | Gold tokenization | —      | —      | 95.81 | 95.98  | 99.37  | 95.25   | 99.90  | 86.68 | 83.56 | 77.72 | 81.86 |
| chinese-gsd-ud-2.15-241121                    | Raw text          | 90.27  | 99.10  | 86.67 | 86.98  | 89.65  | 86.23   | 90.20  | 72.24 | 69.81 | 62.53 | 66.10 |
| chinese-gsd-ud-2.15-241121                    | Gold tokenization | —      | —      | 95.85 | 95.99  | 99.42  | 95.29   | 99.92  | 86.88 | 83.84 | 77.75 | 82.10 |
| classical_armenian-caval-ud-2.15-241121       | Raw text          | 98.80  | 60.28  | 97.16 | —      | 94.95  | 94.10   | 97.40  | 82.79 | 79.50 | 68.83 | 73.76 |
| classical_armenian-caval-ud-2.15-241121       | Gold tokenization | —      | —      | 98.23 | —      | 96.05  | 95.13   | 98.49  | 88.81 | 85.30 | 73.40 | 78.39 |
| classical_chinese-kyoto-ud-2.15-241121        | Raw text          | 97.94  | 46.37  | 89.65 | 88.83  | 91.59  | 86.00   | 97.51  | 71.18 | 65.95 | 62.52 | 64.56 |
| classical_chinese-kyoto-ud-2.15-241121        | Gold tokenization | —      | —      | 93.28 | 92.05  | 94.54  | 89.77   | 99.53  | 84.51 | 79.24 | 75.52 | 78.15 |
| coptic-scriptorium-ud-2.15-241121             | Raw text          | 75.42  | 28.57  | 73.38 | 73.36  | 73.30  | 72.45   | 74.03  | 52.11 | 50.26 | 38.67 | 41.12 |
| coptic-scriptorium-ud-2.15-241121             | Gold tokenization | —      | —      | 97.01 | 96.92  | 97.67  | 95.71   | 97.24  | 90.69 | 87.85 | 77.23 | 80.75 |
| croatian-set-ud-2.15-241121                   | Raw text          | 99.93  | 94.79  | 98.46 | 95.79  | 96.19  | 95.50   | 97.70  | 92.34 | 89.48 | 81.66 | 84.64 |
| croatian-set-ud-2.15-241121                   | Gold tokenization | —      | —      | 98.52 | 95.89  | 96.28  | 95.59   | 97.76  | 92.83 | 89.97 | 82.08 | 85.09 |
| czech-pdt-ud-2.15-241121                      | Raw text          | 99.93  | 93.37  | 99.29 | 98.42  | 98.78  | 98.22   | 99.39  | 94.92 | 93.55 | 90.68 | 92.23 |
| czech-pdt-ud-2.15-241121                      | Gold tokenization | —      | —      | 99.36 | 98.51  | 98.86  | 98.31   | 99.47  | 95.70 | 94.32 | 91.34 | 92.90 |
| czech-cac-ud-2.15-241121                      | Raw text          | 99.99  | 99.68  | 99.67 | 98.32  | 98.13  | 97.76   | 99.18  | 96.16 | 94.95 | 90.89 | 92.85 |
| czech-cac-ud-2.15-241121                      | Gold tokenization | —      | —      | 99.68 | 98.33  | 98.14  | 97.77   | 99.19  | 96.16 | 94.95 | 90.92 | 92.87 |
| czech-cltt-ud-2.15-241121                     | Raw text          | 99.32  | 96.92  | 98.79 | 93.98  | 94.10  | 93.72   | 98.54  | 90.80 | 89.09 | 81.43 | 86.95 |
| czech-cltt-ud-2.15-241121                     | Gold tokenization | —      | —      | 99.24 | 94.37  | 94.56  | 94.13   | 99.01  | 92.09 | 90.11 | 82.04 | 87.76 |
| czech-fictree-ud-2.15-241121                  | Raw text          | 99.99  | 98.95  | 99.17 | 97.07  | 97.89  | 96.88   | 99.27  | 96.17 | 94.74 | 89.62 | 92.52 |
| czech-fictree-ud-2.15-241121                  | Gold tokenization | —      | —      | 99.18 | 97.08  | 97.90  | 96.89   | 99.28  | 96.25 | 94.80 | 89.74 | 92.64 |
| danish-ddt-ud-2.15-241121                     | Raw text          | 99.82  | 89.80  | 97.83 | 99.82  | 97.32  | 96.44   | 97.52  | 88.74 | 86.71 | 79.57 | 81.68 |
| danish-ddt-ud-2.15-241121                     | Gold tokenization | —      | —      | 98.06 | 100.00 | 97.56  | 96.71   | 97.68  | 89.97 | 87.93 | 80.65 | 82.80 |
| dutch-alpino-ud-2.15-241121                   | Raw text          | 99.75  | 89.10  | 98.02 | 97.09  | 97.82  | 96.73   | 95.66  | 93.52 | 91.46 | 85.25 | 82.50 |
| dutch-alpino-ud-2.15-241121                   | Gold tokenization | —      | —      | 98.23 | 97.28  | 97.98  | 96.88   | 95.89  | 94.92 | 92.86 | 86.60 | 83.78 |
| dutch-lassysmall-ud-2.15-241121               | Raw text          | 99.86  | 84.61  | 97.78 | 96.78  | 97.55  | 96.34   | 96.24  | 92.96 | 90.85 | 83.36 | 81.99 |
| dutch-lassysmall-ud-2.15-241121               | Gold tokenization | —      | —      | 98.02 | 97.10  | 97.86  | 96.72   | 96.39  | 95.18 | 92.98 | 86.11 | 84.68 |
| english-ewt-ud-2.15-241121                    | Raw text          | 99.01  | 87.55  | 96.57 | 96.24  | 97.06  | 95.27   | 96.94  | 90.97 | 89.14 | 82.66 | 83.86 |
| english-ewt-ud-2.15-241121                    | Gold tokenization | —      | —      | 97.50 | 97.17  | 97.97  | 96.20   | 97.87  | 93.42 | 91.52 | 85.10 | 86.21 |
| english-atis-ud-2.15-241121                   | Raw text          | 100.00 | 80.03  | 98.91 | —      | 98.50  | 98.04   | 99.67  | 94.19 | 92.71 | 87.41 | 89.83 |
| english-atis-ud-2.15-241121                   | Gold tokenization | —      | —      | 99.01 | —      | 98.56  | 98.12   | 99.65  | 96.09 | 94.47 | 90.01 | 92.36 |
| english-eslspok-ud-2.15-241121                | Raw text          | 99.87  | 88.60  | 98.59 | 98.68  | —      | 98.06   | —      | 94.04 | 92.68 | 90.53 | 92.21 |
| english-eslspok-ud-2.15-241121                | Gold tokenization | —      | —      | 98.72 | 98.81  | —      | 98.19   | —      | 96.12 | 94.66 | 91.90 | 93.81 |
| english-gum-ud-2.15-241121                    | Raw text          | 99.71  | 96.06  | 98.14 | 98.08  | 97.98  | 97.13   | 98.90  | 93.00 | 91.23 | 85.88 | 87.29 |
| english-gum-ud-2.15-241121                    | Gold tokenization | —      | —      | 98.39 | 98.36  | 98.27  | 97.40   | 99.17  | 93.74 | 91.92 | 86.48 | 87.90 |
| english-lines-ud-2.15-241121                  | Raw text          | 99.93  | 87.77  | 97.64 | 96.87  | 97.08  | 94.47   | 98.38  | 91.07 | 88.44 | 80.63 | 83.67 |
| english-lines-ud-2.15-241121                  | Gold tokenization | —      | —      | 97.72 | 96.96  | 97.16  | 94.53   | 98.43  | 91.89 | 89.22 | 81.24 | 84.39 |
| english-partut-ud-2.15-241121                 | Raw text          | 99.72  | 99.02  | 97.34 | 97.26  | 96.82  | 95.82   | 98.25  | 93.82 | 91.86 | 84.00 | 87.14 |
| english-partut-ud-2.15-241121                 | Gold tokenization | —      | —      | 97.59 | 97.51  | 97.07  | 96.07   | 98.53  | 94.07 | 92.11 | 84.31 | 87.40 |
| erzya-jr-ud-2.15-241121                       | Raw text          | 99.10  | 97.02  | 87.75 | 87.33  | 78.87  | 73.69   | 84.02  | 72.78 | 62.99 | 41.51 | 47.05 |
| erzya-jr-ud-2.15-241121                       | Gold tokenization | —      | —      | 88.52 | 87.97  | 79.52  | 74.24   | 84.64  | 73.74 | 63.78 | 41.84 | 47.46 |
| estonian-edt-ud-2.15-241121                   | Raw text          | 99.94  | 91.46  | 97.80 | 98.40  | 96.65  | 95.60   | 95.45  | 88.98 | 86.50 | 81.03 | 80.14 |
| estonian-edt-ud-2.15-241121                   | Gold tokenization | —      | —      | 97.91 | 98.45  | 96.73  | 95.71   | 95.53  | 89.85 | 87.35 | 81.84 | 80.91 |
| estonian-ewt-ud-2.15-241121                   | Raw text          | 98.63  | 78.03  | 94.95 | 96.19  | 94.09  | 91.86   | 94.04  | 83.48 | 80.27 | 72.42 | 73.82 |
| estonian-ewt-ud-2.15-241121                   | Gold tokenization | —      | —      | 96.30 | 97.53  | 95.38  | 93.17   | 95.28  | 87.47 | 83.97 | 75.36 | 76.72 |
| faroese-farpahc-ud-2.15-241121                | Raw text          | 99.74  | 92.77  | 97.38 | 93.09  | 94.39  | 92.38   | 99.74  | 86.35 | 82.48 | 68.50 | 75.88 |
| faroese-farpahc-ud-2.15-241121                | Gold tokenization | —      | —      | 97.56 | 93.28  | 94.60  | 92.52   | 100.00 | 87.32 | 83.35 | 69.30 | 76.97 |
| finnish-tdt-ud-2.15-241121                    | Raw text          | 99.70  | 90.82  | 97.61 | 98.23  | 95.97  | 95.06   | 92.07  | 90.34 | 88.38 | 82.15 | 78.29 |
| finnish-tdt-ud-2.15-241121                    | Gold tokenization | —      | —      | 97.92 | 98.54  | 96.26  | 95.41   | 92.34  | 91.72 | 89.70 | 83.17 | 79.26 |
| finnish-ftb-ud-2.15-241121                    | Raw text          | 99.91  | 86.84  | 96.74 | 95.08  | 96.71  | 94.11   | 95.65  | 90.26 | 87.61 | 80.42 | 81.05 |
| finnish-ftb-ud-2.15-241121                    | Gold tokenization | —      | —      | 97.03 | 95.33  | 96.84  | 94.38   | 95.77  | 92.32 | 89.62 | 82.90 | 83.38 |
| french-gsd-ud-2.15-241121                     | Raw text          | 98.95  | 94.67  | 97.47 | 98.95  | 97.34  | 96.76   | 97.83  | 93.56 | 91.56 | 85.36 | 87.34 |
| french-gsd-ud-2.15-241121                     | Gold tokenization | —      | —      | 98.48 | 100.00 | 98.39  | 97.79   | 98.86  | 95.04 | 93.23 | 87.01 | 88.35 |
| french-parisstories-ud-2.15-241121            | Raw text          | 99.64  | 93.36  | 97.45 | 99.64  | 94.91  | 93.39   | 98.84  | 80.64 | 77.40 | 66.21 | 73.69 |
| french-parisstories-ud-2.15-241121            | Gold tokenization | —      | —      | 97.80 | 100.00 | 95.23  | 93.71   | 99.18  | 81.74 | 78.52 | 67.13 | 74.56 |
| french-partut-ud-2.15-241121                  | Raw text          | 99.42  | 98.64  | 97.78 | 97.51  | 95.28  | 94.44   | 97.97  | 94.94 | 93.25 | 82.97 | 88.47 |
| french-partut-ud-2.15-241121                  | Gold tokenization | —      | —      | 98.31 | 98.12  | 95.81  | 94.93   | 98.54  | 95.54 | 94.01 | 83.59 | 89.02 |
| french-rhapsodie-ud-2.15-241121               | Raw text          | 99.16  | 99.82  | 97.45 | 99.16  | 96.57  | 95.59   | 98.47  | 87.73 | 84.65 | 76.48 | 80.84 |
| french-rhapsodie-ud-2.15-241121               | Gold tokenization | —      | —      | 98.33 | 100.00 | 97.38  | 96.45   | 99.29  | 89.02 | 85.92 | 77.48 | 81.52 |
| french-sequoia-ud-2.15-241121                 | Raw text          | 99.12  | 88.77  | 98.38 | —      | 97.41  | 97.00   | 98.24  | 93.98 | 92.72 | 86.71 | 89.23 |
| french-sequoia-ud-2.15-241121                 | Gold tokenization | —      | —      | 99.28 | —      | 98.29  | 97.86   | 99.09  | 95.75 | 94.49 | 88.54 | 90.42 |
| galician-treegal-ud-2.15-241121               | Raw text          | 98.74  | 87.99  | 96.29 | 94.37  | 95.37  | 93.52   | 97.27  | 83.29 | 79.18 | 68.58 | 72.41 |
| galician-treegal-ud-2.15-241121               | Gold tokenization | —      | —      | 97.54 | 95.47  | 96.44  | 94.59   | 98.47  | 86.93 | 82.53 | 72.41 | 76.46 |
| galician-ctg-ud-2.15-241121                   | Raw text          | 99.22  | 97.22  | 97.16 | 96.99  | 99.06  | 96.55   | 98.07  | 85.36 | 82.85 | 71.13 | 75.73 |
| galician-ctg-ud-2.15-241121                   | Gold tokenization | —      | —      | 97.88 | 97.72  | 99.84  | 97.26   | 98.83  | 87.00 | 84.36 | 72.95 | 77.61 |
| georgian-glc-ud-2.15-241121                   | Raw text          | 99.12  | 95.88  | 95.89 | 95.87  | 91.35  | 90.93   | 94.03  | 83.18 | 78.90 | 68.71 | 72.92 |
| georgian-glc-ud-2.15-241121                   | Gold tokenization | —      | —      | 96.59 | 96.57  | 91.97  | 91.54   | 94.73  | 84.73 | 80.26 | 69.52 | 73.98 |
| german-gsd-ud-2.15-241121                     | Raw text          | 99.67  | 83.63  | 96.67 | 97.53  | 91.24  | 88.72   | 96.91  | 87.27 | 83.53 | 66.10 | 75.49 |
| german-gsd-ud-2.15-241121                     | Gold tokenization | —      | —      | 97.07 | 97.90  | 91.68  | 89.23   | 97.24  | 89.23 | 85.46 | 67.85 | 77.49 |
| german-hdt-ud-2.15-241121                     | Raw text          | 99.90  | 92.39  | 98.55 | 98.46  | 94.19  | 93.79   | 97.68  | 96.92 | 96.00 | 84.94 | 90.48 |
| german-hdt-ud-2.15-241121                     | Gold tokenization | —      | —      | 98.66 | 98.59  | 94.32  | 93.93   | 97.77  | 97.61 | 96.72 | 85.62 | 91.18 |
| gothic-proiel-ud-2.15-241121                  | Raw text          | 100.00 | 31.12  | 96.13 | 96.65  | 90.10  | 88.00   | 94.71  | 78.67 | 72.55 | 58.51 | 63.15 |
| gothic-proiel-ud-2.15-241121                  | Gold tokenization | —      | —      | 96.68 | 97.22  | 91.05  | 89.29   | 94.77  | 86.93 | 81.01 | 68.64 | 72.88 |
| greek-gdt-ud-2.15-241121                      | Raw text          | 99.87  | 90.19  | 98.12 | 98.15  | 95.68  | 95.00   | 96.04  | 92.91 | 91.13 | 81.57 | 81.64 |
| greek-gdt-ud-2.15-241121                      | Gold tokenization | —      | —      | 98.28 | 98.30  | 95.85  | 95.17   | 96.13  | 93.70 | 91.84 | 82.22 | 82.28 |
| greek-gud-ud-2.15-241121                      | Raw text          | 99.92  | 94.98  | 97.11 | 96.29  | 94.42  | 90.66   | 95.76  | 92.98 | 90.15 | 76.44 | 80.55 |
| greek-gud-ud-2.15-241121                      | Gold tokenization | —      | —      | 97.15 | 96.33  | 94.45  | 90.68   | 95.82  | 93.65 | 90.82 | 76.90 | 81.03 |
| hebrew-htb-ud-2.15-241121                     | Raw text          | 85.10  | 99.69  | 83.02 | 83.00  | 81.46  | 80.80   | 82.97  | 70.48 | 68.06 | 55.81 | 59.83 |
| hebrew-htb-ud-2.15-241121                     | Gold tokenization | —      | —      | 97.72 | 97.67  | 95.99  | 95.43   | 97.34  | 92.53 | 90.03 | 79.69 | 82.23 |
| hebrew-iahltknesset-ud-2.15-241121            | Raw text          | 87.98  | 100.00 | 85.29 | 85.25  | 81.63  | 80.79   | 86.84  | 71.33 | 68.88 | 56.06 | 62.80 |
| hebrew-iahltknesset-ud-2.15-241121            | Gold tokenization | —      | —      | 96.93 | 96.95  | 92.70  | 91.84   | 98.29  | 90.09 | 87.40 | 72.65 | 80.96 |
| hebrew-iahltwiki-ud-2.15-241121               | Raw text          | 88.64  | 96.78  | 86.13 | 86.12  | 81.93  | 80.95   | 87.41  | 75.89 | 74.01 | 58.29 | 67.00 |
| hebrew-iahltwiki-ud-2.15-241121               | Gold tokenization | —      | —      | 97.14 | 97.15  | 92.47  | 91.47   | 98.36  | 93.66 | 91.38 | 75.84 | 85.70 |
| hindi-hdtb-ud-2.15-241121                     | Raw text          | 100.00 | 98.72  | 97.59 | 97.19  | 94.21  | 92.26   | 98.92  | 95.30 | 92.39 | 79.46 | 87.81 |
| hindi-hdtb-ud-2.15-241121                     | Gold tokenization | —      | —      | 97.59 | 97.18  | 94.23  | 92.27   | 98.92  | 95.41 | 92.50 | 79.58 | 87.94 |
| hungarian-szeged-ud-2.15-241121               | Raw text          | 99.85  | 95.89  | 96.76 | —      | 94.29  | 93.58   | 94.91  | 88.24 | 84.66 | 74.89 | 78.03 |
| hungarian-szeged-ud-2.15-241121               | Gold tokenization | —      | —      | 96.84 | —      | 94.43  | 93.66   | 95.02  | 88.70 | 85.08 | 75.20 | 78.33 |
| icelandic-modern-ud-2.15-241121               | Raw text          | 99.44  | 94.59  | 97.74 | 95.29  | 89.29  | 86.46   | 97.09  | 85.69 | 82.73 | 64.77 | 75.00 |
| icelandic-modern-ud-2.15-241121               | Gold tokenization | —      | —      | 98.24 | 95.85  | 89.69  | 86.87   | 97.61  | 86.55 | 83.54 | 65.49 | 75.94 |
| icelandic-gc-ud-2.15-241121                   | Raw text          | 99.72  | 94.64  | 94.72 | 82.03  | 85.00  | 79.71   | 91.82  | 83.41 | 79.03 | 58.46 | 69.15 |
| icelandic-gc-ud-2.15-241121                   | Gold tokenization | —      | —      | 95.00 | 82.51  | 85.50  | 80.22   | 91.98  | 84.17 | 79.77 | 59.02 | 69.68 |
| icelandic-icepahc-ud-2.15-241121              | Raw text          | 99.82  | 92.69  | 96.89 | 93.33  | 92.09  | 87.24   | 96.37  | 87.08 | 83.25 | 66.85 | 74.53 |
| icelandic-icepahc-ud-2.15-241121              | Gold tokenization | —      | —      | 97.05 | 93.56  | 92.22  | 87.45   | 96.51  | 87.58 | 83.70 | 67.31 | 75.07 |
| indonesian-gsd-ud-2.15-241121                 | Raw text          | 99.49  | 93.04  | 94.30 | 93.86  | 95.56  | 88.79   | 98.08  | 87.84 | 81.86 | 72.68 | 77.23 |
| indonesian-gsd-ud-2.15-241121                 | Gold tokenization | —      | —      | 94.78 | 94.25  | 96.00  | 89.18   | 98.49  | 88.66 | 82.59 | 73.43 | 78.01 |
| indonesian-csui-ud-2.15-241121                | Raw text          | 99.45  | 91.01  | 95.96 | 96.11  | 96.81  | 95.36   | 98.17  | 86.51 | 82.20 | 76.66 | 78.83 |
| indonesian-csui-ud-2.15-241121                | Gold tokenization | —      | —      | 96.48 | 96.63  | 97.32  | 95.85   | 98.81  | 87.93 | 83.42 | 77.61 | 79.92 |
| irish-idt-ud-2.15-241121                      | Raw text          | 99.88  | 97.58  | 95.93 | 94.99  | 90.72  | 87.62   | 95.77  | 87.16 | 81.64 | 65.30 | 72.34 |
| irish-idt-ud-2.15-241121                      | Gold tokenization | —      | —      | 96.04 | 95.14  | 90.83  | 87.76   | 95.89  | 87.50 | 81.97 | 65.44 | 72.54 |
| irish-twittirish-ud-2.15-241121               | Raw text          | 98.50  | 46.62  | 90.63 | —      | —      | 90.63   | 88.28  | 78.98 | 72.60 | 58.85 | 57.12 |
| irish-twittirish-ud-2.15-241121               | Gold tokenization | —      | —      | 91.84 | —      | —      | 91.84   | 89.54  | 85.80 | 79.26 | 66.75 | 64.25 |
| italian-isdt-ud-2.15-241121                   | Raw text          | 99.74  | 99.07  | 98.51 | 98.40  | 98.06  | 97.67   | 98.58  | 94.65 | 92.95 | 86.68 | 87.71 |
| italian-isdt-ud-2.15-241121                   | Gold tokenization | —      | —      | 98.75 | 98.66  | 98.30  | 97.93   | 98.84  | 95.08 | 93.39 | 87.08 | 88.14 |
| italian-markit-ud-2.15-241121                 | Raw text          | 99.62  | 98.24  | 96.98 | 97.07  | 94.16  | 92.53   | 88.34  | 88.48 | 84.70 | 70.60 | 78.16 |
| italian-markit-ud-2.15-241121                 | Gold tokenization | —      | —      | 97.35 | 97.41  | 94.42  | 92.78   | 88.66  | 89.27 | 85.48 | 71.20 | 78.90 |
| italian-old-ud-2.15-241121                    | Raw text          | 99.08  | 97.76  | 96.30 | 86.81  | 91.87  | 83.24   | 96.49  | 85.37 | 80.93 | 64.37 | 72.68 |
| italian-old-ud-2.15-241121                    | Gold tokenization | —      | —      | 97.15 | 87.27  | 92.71  | 83.82   | 97.35  | 88.20 | 83.50 | 67.16 | 75.55 |
| italian-parlamint-ud-2.15-241121              | Raw text          | 99.42  | 94.12  | 98.64 | 98.05  | 97.96  | 97.02   | 98.70  | 91.94 | 89.98 | 84.45 | 86.25 |
| italian-parlamint-ud-2.15-241121              | Gold tokenization | —      | —      | 99.22 | 98.59  | 98.48  | 97.50   | 99.20  | 93.40 | 91.43 | 86.08 | 87.84 |
| italian-partut-ud-2.15-241121                 | Raw text          | 99.73  | 100.00 | 98.43 | 98.43  | 98.16  | 97.58   | 98.60  | 95.74 | 93.79 | 87.17 | 88.62 |
| italian-partut-ud-2.15-241121                 | Gold tokenization | —      | —      | 98.60 | 98.60  | 98.30  | 97.72   | 98.79  | 95.80 | 93.76 | 87.09 | 88.54 |
| italian-postwita-ud-2.15-241121               | Raw text          | 99.36  | 49.53  | 96.61 | 96.39  | 96.15  | 94.87   | 96.40  | 83.05 | 79.20 | 68.99 | 70.63 |
| italian-postwita-ud-2.15-241121               | Gold tokenization | —      | —      | 97.20 | 96.95  | 96.65  | 95.40   | 96.96  | 88.06 | 83.78 | 75.30 | 76.72 |
| italian-twittiro-ud-2.15-241121               | Raw text          | 98.94  | 46.67  | 95.84 | 95.61  | 94.70  | 93.12   | 94.30  | 82.95 | 78.28 | 65.69 | 66.30 |
| italian-twittiro-ud-2.15-241121               | Gold tokenization | —      | —      | 96.71 | 96.30  | 95.67  | 93.85   | 95.23  | 88.34 | 83.50 | 71.83 | 72.07 |
| italian-vit-ud-2.15-241121                    | Raw text          | 99.75  | 95.06  | 98.14 | 97.29  | 97.64  | 96.14   | 98.85  | 92.20 | 89.31 | 81.25 | 83.96 |
| italian-vit-ud-2.15-241121                    | Gold tokenization | —      | —      | 98.39 | 97.68  | 97.85  | 96.51   | 99.09  | 93.03 | 90.11 | 82.06 | 84.81 |
| japanese-gsdluw-ud-2.15-241121                | Raw text          | 95.18  | 99.72  | 93.91 | 93.66  | 95.18  | 93.59   | 93.65  | 86.30 | 85.69 | 76.64 | 76.59 |
| japanese-gsdluw-ud-2.15-241121                | Gold tokenization | —      | —      | 98.42 | 98.15  | 99.99  | 98.02   | 97.85  | 95.23 | 94.29 | 86.58 | 85.22 |
| japanese-gsd-ud-2.15-241121                   | Raw text          | 96.17  | 100.00 | 95.02 | 94.28  | 96.16  | 94.01   | 95.11  | 88.07 | 87.37 | 81.12 | 81.42 |
| japanese-gsd-ud-2.15-241121                   | Gold tokenization | —      | —      | 98.59 | 97.62  | 99.98  | 97.32   | 98.55  | 95.14 | 94.24 | 89.25 | 89.12 |
| korean-kaist-ud-2.15-241121                   | Raw text          | 100.00 | 100.00 | 96.26 | 87.65  | —      | 87.45   | 94.46  | 89.09 | 87.20 | 83.13 | 80.96 |
| korean-kaist-ud-2.15-241121                   | Gold tokenization | —      | —      | 96.26 | 87.65  | —      | 87.45   | 94.46  | 89.09 | 87.20 | 83.13 | 80.96 |
| korean-gsd-ud-2.15-241121                     | Raw text          | 99.87  | 93.93  | 96.50 | 90.63  | 99.68  | 88.32   | 93.87  | 88.25 | 84.65 | 81.69 | 77.71 |
| korean-gsd-ud-2.15-241121                     | Gold tokenization | —      | —      | 96.67 | 90.84  | 99.81  | 88.52   | 94.01  | 88.88 | 85.24 | 82.32 | 78.31 |
| korean-ksl-ud-2.15-241121                     | Raw text          | 100.00 | 99.22  | 96.75 | 89.63  | —      | 87.83   | 95.15  | 89.83 | 86.38 | 81.58 | 80.07 |
| korean-ksl-ud-2.15-241121                     | Gold tokenization | —      | —      | 96.74 | 89.64  | —      | 87.83   | 95.15  | 89.92 | 86.47 | 81.66 | 80.15 |
| kyrgyz-ktmu-ud-2.15-241121                    | Raw text          | 99.16  | 98.03  | 90.81 | 90.36  | 77.09  | 72.50   | 88.58  | 83.55 | 72.59 | 53.12 | 62.75 |
| kyrgyz-ktmu-ud-2.15-241121                    | Gold tokenization | —      | —      | 91.56 | 91.11  | 77.78  | 73.17   | 89.33  | 84.47 | 73.43 | 53.62 | 63.27 |
| latin-ittb-ud-2.15-241121                     | Raw text          | 99.98  | 91.79  | 99.11 | 96.63  | 97.19  | 95.80   | 99.17  | 89.48 | 87.54 | 81.42 | 84.91 |
| latin-ittb-ud-2.15-241121                     | Gold tokenization | —      | —      | 99.14 | 96.67  | 97.24  | 95.84   | 99.21  | 90.45 | 88.52 | 82.05 | 85.50 |
| latin-llct-ud-2.15-241121                     | Raw text          | 99.99  | 99.49  | 99.73 | 97.09  | 97.13  | 96.83   | 97.79  | 95.35 | 94.38 | 88.99 | 90.31 |
| latin-llct-ud-2.15-241121                     | Gold tokenization | —      | —      | 99.73 | 97.09  | 97.13  | 96.84   | 97.80  | 95.36 | 94.39 | 88.99 | 90.31 |
| latin-perseus-ud-2.15-241121                  | Raw text          | 98.23  | 99.09  | 91.46 | 80.03  | 83.36  | 76.59   | 87.60  | 76.93 | 70.09 | 52.28 | 58.28 |
| latin-perseus-ud-2.15-241121                  | Gold tokenization | —      | —      | 93.18 | 81.55  | 84.92  | 78.04   | 89.21  | 78.09 | 71.12 | 52.61 | 59.23 |
| latin-proiel-ud-2.15-241121                   | Raw text          | 99.85  | 37.40  | 96.52 | 96.58  | 90.71  | 89.42   | 96.08  | 76.57 | 72.36 | 59.00 | 64.80 |
| latin-proiel-ud-2.15-241121                   | Gold tokenization | —      | —      | 97.06 | 97.10  | 91.55  | 90.41   | 96.30  | 83.88 | 79.62 | 67.94 | 73.42 |
| latin-udante-ud-2.15-241121                   | Raw text          | 99.60  | 98.45  | 91.16 | 75.61  | 84.53  | 72.57   | 87.61  | 76.74 | 69.64 | 48.88 | 52.99 |
| latin-udante-ud-2.15-241121                   | Gold tokenization | —      | —      | 91.42 | 75.73  | 84.78  | 72.65   | 87.87  | 76.93 | 69.81 | 48.91 | 53.07 |
| latvian-lvtb-ud-2.15-241121                   | Raw text          | 99.27  | 98.09  | 97.15 | 91.74  | 95.18  | 91.32   | 96.76  | 89.45 | 86.56 | 78.80 | 81.97 |
| latvian-lvtb-ud-2.15-241121                   | Gold tokenization | —      | —      | 97.83 | 92.42  | 95.89  | 91.99   | 97.42  | 90.53 | 87.60 | 79.96 | 83.09 |
| lithuanian-alksnis-ud-2.15-241121             | Raw text          | 99.91  | 87.87  | 96.03 | 90.49  | 91.22  | 89.72   | 93.59  | 82.91 | 79.35 | 68.74 | 71.87 |
| lithuanian-alksnis-ud-2.15-241121             | Gold tokenization | —      | —      | 96.15 | 90.59  | 91.31  | 89.83   | 93.68  | 84.17 | 80.59 | 69.66 | 72.80 |
| lithuanian-hse-ud-2.15-241121                 | Raw text          | 97.30  | 97.30  | 90.31 | 89.93  | 82.20  | 78.75   | 88.35  | 71.67 | 62.35 | 45.18 | 53.97 |
| lithuanian-hse-ud-2.15-241121                 | Gold tokenization | —      | —      | 92.17 | 91.89  | 84.06  | 80.38   | 90.85  | 75.28 | 64.91 | 46.72 | 55.70 |
| low_saxon-lsdc-ud-2.15-241121                 | Raw text          | 99.25  | 90.23  | 89.96 | —      | 71.84  | 69.14   | 83.89  | 74.31 | 65.21 | 37.02 | 48.45 |
| low_saxon-lsdc-ud-2.15-241121                 | Gold tokenization | —      | —      | 90.59 | —      | 72.43  | 69.64   | 84.39  | 75.48 | 66.25 | 37.13 | 48.74 |
| maghrebi_arabic_french-arabizi-ud-2.15-241121 | Raw text          | 91.65  | 7.00   | 78.90 | 72.06  | 83.03  | 70.37   | 51.43  | 57.85 | 49.98 | 36.37 | 24.60 |
| maghrebi_arabic_french-arabizi-ud-2.15-241121 | Gold tokenization | —      | —      | 86.55 | 78.66  | 90.64  | 77.33   | 54.89  | 76.14 | 65.63 | 47.29 | 31.71 |
| maltese-mudt-ud-2.15-241121                   | Raw text          | 99.84  | 86.29  | 95.64 | 95.55  | —      | 95.24   | —      | 84.61 | 79.54 | 67.91 | 71.94 |
| maltese-mudt-ud-2.15-241121                   | Gold tokenization | —      | —      | 95.75 | 95.68  | —      | 95.34   | —      | 85.32 | 80.19 | 68.41 | 72.44 |
| manx-cadhan-ud-2.15-241121                    | Raw text          | 97.36  | 98.25  | 94.09 | —      | 95.84  | 93.32   | 93.34  | 87.60 | 84.14 | 77.74 | 77.73 |
| manx-cadhan-ud-2.15-241121                    | Gold tokenization | —      | —      | 96.68 | —      | 98.43  | 95.85   | 95.88  | 92.57 | 89.12 | 82.90 | 81.70 |
| marathi-ufal-ud-2.15-241121                   | Raw text          | 94.16  | 92.63  | 82.73 | —      | 75.18  | 71.53   | 84.18  | 66.67 | 60.34 | 40.00 | 47.84 |
| marathi-ufal-ud-2.15-241121                   | Gold tokenization | —      | —      | 87.14 | —      | 78.64  | 74.51   | 87.14  | 72.33 | 65.29 | 43.71 | 51.13 |
| naija-nsc-ud-2.15-241121                      | Raw text          | 99.97  | 100.00 | 98.12 | —      | 98.94  | 97.59   | 99.39  | 93.10 | 90.55 | 87.51 | 89.19 |
| naija-nsc-ud-2.15-241121                      | Gold tokenization | —      | —      | 98.15 | —      | 98.95  | 97.60   | 99.42  | 93.13 | 90.58 | 87.52 | 89.21 |
| north_sami-giella-ud-2.15-241121              | Raw text          | 99.87  | 98.79  | 91.64 | 93.42  | 89.16  | 84.95   | 87.07  | 75.74 | 70.92 | 60.14 | 58.71 |
| north_sami-giella-ud-2.15-241121              | Gold tokenization | —      | —      | 91.78 | 93.57  | 89.30  | 85.08   | 87.19  | 75.99 | 71.18 | 60.30 | 58.92 |
| norwegian-bokmaal-ud-2.15-241121              | Raw text          | 99.82  | 97.27  | 98.39 | 98.95  | 97.48  | 96.82   | 98.62  | 94.00 | 92.78 | 87.18 | 89.00 |
| norwegian-bokmaal-ud-2.15-241121              | Gold tokenization | —      | —      | 98.59 | 99.13  | 97.65  | 96.99   | 98.82  | 94.67 | 93.43 | 87.79 | 89.65 |
| norwegian-nynorsk-ud-2.15-241121              | Raw text          | 99.93  | 94.54  | 98.36 | 99.06  | 97.29  | 96.45   | 98.40  | 93.94 | 92.47 | 85.90 | 88.09 |
| norwegian-nynorsk-ud-2.15-241121              | Gold tokenization | —      | —      | 98.55 | 99.20  | 97.46  | 96.68   | 98.55  | 94.69 | 93.24 | 86.85 | 89.04 |
| old_church_slavonic-proiel-ud-2.15-241121     | Raw text          | 100.00 | 40.05  | 96.23 | 96.48  | 89.78  | 88.01   | 90.21  | 78.01 | 73.64 | 60.75 | 62.07 |
| old_church_slavonic-proiel-ud-2.15-241121     | Gold tokenization | —      | —      | 96.68 | 96.97  | 90.42  | 88.99   | 90.29  | 85.13 | 80.56 | 68.45 | 69.16 |
| old_east_slavic-torot-ud-2.15-241121          | Raw text          | 100.00 | 34.53  | 95.34 | 95.41  | 89.70  | 87.47   | 88.42  | 77.02 | 72.42 | 58.65 | 58.48 |
| old_east_slavic-torot-ud-2.15-241121          | Gold tokenization | —      | —      | 95.87 | 95.91  | 90.63  | 88.71   | 88.48  | 85.60 | 80.77 | 68.28 | 66.57 |
| old_east_slavic-birchbark-ud-2.15-241121      | Raw text          | 99.99  | 16.66  | 88.74 | 99.35  | 75.36  | 71.18   | 65.21  | 65.19 | 58.34 | 32.66 | 27.06 |
| old_east_slavic-birchbark-ud-2.15-241121      | Gold tokenization | —      | —      | 88.88 | 99.35  | 75.99  | 71.81   | 65.28  | 76.89 | 69.91 | 40.37 | 32.66 |
| old_east_slavic-rnc-ud-2.15-241121            | Raw text          | 99.77  | 94.56  | 97.57 | 91.49  | 89.51  | 82.05   | 90.63  | 76.91 | 73.22 | 54.56 | 57.05 |
| old_east_slavic-rnc-ud-2.15-241121            | Gold tokenization | —      | —      | 97.82 | 91.69  | 89.79  | 82.32   | 90.89  | 79.53 | 75.69 | 56.31 | 58.99 |
| old_east_slavic-ruthenian-ud-2.15-241121      | Raw text          | 99.87  | 99.61  | 96.17 | 89.35  | 87.73  | 80.61   | 82.89  | 78.09 | 74.30 | 53.77 | 49.03 |
| old_east_slavic-ruthenian-ud-2.15-241121      | Gold tokenization | —      | —      | 96.23 | 89.77  | 87.78  | 81.02   | 82.96  | 78.16 | 74.35 | 53.73 | 49.06 |
| old_french-profiterole-ud-2.15-241121         | Raw text          | 99.82  | 100.00 | 97.15 | 97.05  | 97.54  | 95.63   | 99.79  | 91.04 | 87.47 | 80.05 | 84.53 |
| old_french-profiterole-ud-2.15-241121         | Gold tokenization | —      | —      | 97.33 | 97.24  | 97.72  | 95.82   | 99.97  | 91.29 | 87.72 | 80.31 | 84.80 |
| ottoman_turkish-boun-ud-2.15-241121           | Raw text          | 99.41  | 87.96  | 87.32 | 90.51  | 80.87  | 72.90   | 82.19  | 61.58 | 51.26 | 32.83 | 36.22 |
| ottoman_turkish-boun-ud-2.15-241121           | Gold tokenization | —      | —      | 87.77 | 90.97  | 81.24  | 73.21   | 82.52  | 62.34 | 51.82 | 33.10 | 36.63 |
| persian-perdt-ud-2.15-241121                  | Raw text          | 99.66  | 99.83  | 97.45 | 97.40  | 97.65  | 95.65   | 98.96  | 93.58 | 91.39 | 86.23 | 88.74 |
| persian-perdt-ud-2.15-241121                  | Gold tokenization | —      | —      | 97.75 | 97.70  | 97.95  | 95.94   | 99.28  | 94.09 | 91.87 | 86.77 | 89.30 |
| persian-seraji-ud-2.15-241121                 | Raw text          | 99.65  | 98.75  | 97.95 | 97.92  | 97.95  | 97.47   | 98.27  | 91.66 | 88.85 | 84.42 | 84.51 |
| persian-seraji-ud-2.15-241121                 | Gold tokenization | —      | —      | 98.27 | 98.23  | 98.27  | 97.76   | 98.54  | 92.37 | 89.52 | 85.00 | 85.11 |
| polish-pdb-ud-2.15-241121                     | Raw text          | 99.86  | 97.00  | 98.99 | 96.07  | 96.05  | 95.35   | 98.12  | 94.40 | 92.57 | 85.84 | 88.86 |
| polish-pdb-ud-2.15-241121                     | Gold tokenization | —      | —      | 99.11 | 96.23  | 96.22  | 95.51   | 98.23  | 94.95 | 93.10 | 86.32 | 89.30 |
| polish-lfg-ud-2.15-241121                     | Raw text          | 99.85  | 99.65  | 99.01 | 96.18  | 96.68  | 95.22   | 98.17  | 96.91 | 95.62 | 90.04 | 92.40 |
| polish-lfg-ud-2.15-241121                     | Gold tokenization | —      | —      | 99.18 | 96.35  | 96.86  | 95.39   | 98.31  | 97.29 | 96.00 | 90.44 | 92.73 |
| pomak-philotis-ud-2.15-241121                 | Raw text          | 99.79  | 89.42  | 95.42 | —      | 88.85  | 87.88   | 91.37  | 88.30 | 81.80 | 63.73 | 67.56 |
| pomak-philotis-ud-2.15-241121                 | Gold tokenization | —      | —      | 95.54 | —      | 88.98  | 87.99   | 91.49  | 89.24 | 82.65 | 64.34 | 68.30 |
| portuguese-bosque-ud-2.15-241121              | Raw text          | 99.68  | 89.73  | 97.78 | —      | 96.92  | 95.85   | 98.36  | 92.31 | 89.98 | 80.69 | 84.65 |
| portuguese-bosque-ud-2.15-241121              | Gold tokenization | —      | —      | 98.11 | —      | 97.19  | 96.12   | 98.65  | 93.46 | 91.08 | 81.78 | 85.74 |
| portuguese-cintil-ud-2.15-241121              | Raw text          | 99.41  | 78.66  | 97.44 | 96.04  | 95.33  | 93.23   | 97.49  | 85.30 | 82.28 | 72.33 | 75.94 |
| portuguese-cintil-ud-2.15-241121              | Gold tokenization | —      | —      | 98.04 | 96.65  | 95.93  | 93.81   | 98.06  | 87.64 | 84.51 | 74.47 | 78.14 |
| portuguese-dantestocks-ud-2.15-241121         | Raw text          | 96.47  | 38.27  | 94.23 | 96.45  | 93.84  | 92.88   | 93.66  | 85.38 | 83.08 | 75.34 | 75.93 |
| portuguese-dantestocks-ud-2.15-241121         | Gold tokenization | —      | —      | 97.71 | 99.98  | 97.35  | 96.35   | 95.93  | 93.04 | 90.56 | 84.23 | 82.70 |
| portuguese-gsd-ud-2.15-241121                 | Raw text          | 99.29  | 86.25  | 97.49 | 89.64  | 94.60  | 89.18   | 97.14  | 92.75 | 90.80 | 80.17 | 85.14 |
| portuguese-gsd-ud-2.15-241121                 | Gold tokenization | —      | —      | 98.27 | 91.76  | 96.07  | 91.27   | 97.98  | 94.25 | 92.35 | 82.76 | 86.84 |
| portuguese-petrogold-ud-2.15-241121           | Raw text          | 99.59  | 93.11  | 98.79 | —      | 98.69  | 98.21   | 99.12  | 94.69 | 93.53 | 88.53 | 90.01 |
| portuguese-petrogold-ud-2.15-241121           | Gold tokenization | —      | —      | 99.10 | —      | 98.96  | 98.47   | 99.54  | 95.61 | 94.37 | 89.44 | 91.03 |
| portuguese-porttinari-ud-2.15-241121          | Raw text          | 94.68  | 28.05  | 93.90 | —      | 93.44  | 93.10   | 94.17  | 85.85 | 84.31 | 78.62 | 81.30 |
| portuguese-porttinari-ud-2.15-241121          | Gold tokenization | —      | —      | 99.20 | —      | 98.72  | 98.36   | 99.45  | 96.48 | 95.23 | 90.19 | 91.95 |
| romanian-rrt-ud-2.15-241121                   | Raw text          | 99.70  | 95.50  | 97.83 | 97.19  | 97.41  | 96.91   | 97.99  | 91.90 | 88.44 | 81.88 | 83.42 |
| romanian-rrt-ud-2.15-241121                   | Gold tokenization | —      | —      | 98.11 | 97.43  | 97.67  | 97.16   | 98.25  | 92.64 | 89.11 | 82.32 | 83.92 |
| romanian-nonstandard-ud-2.15-241121           | Raw text          | 98.83  | 96.77  | 96.16 | 91.94  | 90.58  | 89.24   | 94.86  | 89.06 | 84.99 | 68.53 | 76.59 |
| romanian-nonstandard-ud-2.15-241121           | Gold tokenization | —      | —      | 97.29 | 92.97  | 91.56  | 90.19   | 95.94  | 90.76 | 86.62 | 70.04 | 77.84 |
| romanian-simonero-ud-2.15-241121              | Raw text          | 99.84  | 100.00 | 98.46 | 97.94  | 97.55  | 97.22   | 98.88  | 94.01 | 92.09 | 85.42 | 88.34 |
| romanian-simonero-ud-2.15-241121              | Gold tokenization | —      | —      | 98.62 | 98.09  | 97.70  | 97.37   | 99.04  | 94.36 | 92.41 | 85.69 | 88.61 |
| russian-syntagrus-ud-2.15-241121              | Raw text          | 99.67  | 98.31  | 98.48 | —      | 94.01  | 93.76   | 98.18  | 93.80 | 91.67 | 82.76 | 88.83 |
| russian-syntagrus-ud-2.15-241121              | Gold tokenization | —      | —      | 98.81 | —      | 94.34  | 94.07   | 98.46  | 94.51 | 92.34 | 83.32 | 89.36 |
| russian-gsd-ud-2.15-241121                    | Raw text          | 99.50  | 96.49  | 98.04 | 97.52  | 94.55  | 93.40   | 96.91  | 91.59 | 88.62 | 80.99 | 84.54 |
| russian-gsd-ud-2.15-241121                    | Gold tokenization | —      | —      | 98.51 | 97.94  | 94.97  | 93.78   | 97.29  | 92.83 | 89.74 | 81.84 | 85.48 |
| russian-poetry-ud-2.15-241121                 | Raw text          | 99.59  | 95.96  | 97.86 | —      | 94.43  | 93.89   | 97.01  | 89.10 | 86.14 | 77.13 | 80.68 |
| russian-poetry-ud-2.15-241121                 | Gold tokenization | —      | —      | 98.24 | —      | 94.77  | 94.23   | 97.36  | 90.04 | 87.07 | 77.84 | 81.36 |
| russian-taiga-ud-2.15-241121                  | Raw text          | 98.07  | 86.01  | 95.55 | —      | 93.12  | 92.12   | 94.77  | 83.27 | 79.86 | 71.21 | 74.38 |
| russian-taiga-ud-2.15-241121                  | Gold tokenization | —      | —      | 97.27 | —      | 94.91  | 93.83   | 96.47  | 85.97 | 82.33 | 73.62 | 76.76 |
| sanskrit-vedic-ud-2.15-241121                 | Raw text          | 100.00 | 29.21  | 93.56 | —      | 89.19  | 85.34   | 93.43  | 65.35 | 56.83 | 49.01 | 52.10 |
| sanskrit-vedic-ud-2.15-241121                 | Gold tokenization | —      | —      | 93.95 | —      | 90.47  | 86.83   | 93.57  | 78.23 | 69.08 | 60.90 | 64.49 |
| scottish_gaelic-arcosg-ud-2.15-241121         | Raw text          | 97.42  | 61.26  | 93.83 | 89.65  | 91.09  | 88.52   | 95.13  | 80.86 | 76.41 | 65.27 | 70.06 |
| scottish_gaelic-arcosg-ud-2.15-241121         | Gold tokenization | —      | —      | 96.61 | 92.63  | 94.01  | 91.57   | 97.71  | 86.97 | 82.58 | 71.95 | 76.15 |
| serbian-set-ud-2.15-241121                    | Raw text          | 99.99  | 93.00  | 99.09 | 95.92  | 96.10  | 95.69   | 97.80  | 93.60 | 91.18 | 83.54 | 86.95 |
| serbian-set-ud-2.15-241121                    | Gold tokenization | —      | —      | 99.11 | 95.97  | 96.14  | 95.73   | 97.79  | 94.32 | 91.88 | 84.29 | 87.68 |
| slovak-snk-ud-2.15-241121                     | Raw text          | 100.00 | 81.69  | 97.69 | 90.12  | 93.40  | 89.34   | 96.54  | 91.58 | 89.94 | 80.37 | 84.70 |
| slovak-snk-ud-2.15-241121                     | Gold tokenization | —      | —      | 97.83 | 90.35  | 93.48  | 89.56   | 96.57  | 93.99 | 92.30 | 82.68 | 87.13 |
| slovenian-ssj-ud-2.15-241121                  | Raw text          | 99.94  | 98.95  | 98.78 | 97.01  | 97.12  | 96.57   | 98.59  | 94.37 | 92.78 | 87.22 | 89.16 |
| slovenian-ssj-ud-2.15-241121                  | Gold tokenization | —      | —      | 98.84 | 97.07  | 97.17  | 96.63   | 98.64  | 94.51 | 92.91 | 87.37 | 89.28 |
| slovenian-sst-ud-2.15-241121                  | Raw text          | 99.87  | 95.47  | 98.45 | 96.90  | 97.01  | 96.19   | 98.83  | 84.82 | 82.12 | 73.82 | 77.42 |
| slovenian-sst-ud-2.15-241121                  | Gold tokenization | —      | —      | 98.59 | 97.00  | 97.08  | 96.28   | 98.97  | 85.33 | 82.63 | 74.23 | 77.91 |
| spanish-ancora-ud-2.15-241121                 | Raw text          | 99.95  | 98.69  | 99.06 | 96.22  | 98.80  | 95.83   | 99.47  | 93.80 | 92.15 | 87.11 | 88.69 |
| spanish-ancora-ud-2.15-241121                 | Gold tokenization | —      | —      | 99.11 | 96.26  | 98.85  | 95.86   | 99.51  | 94.00 | 92.35 | 87.30 | 88.85 |
| spanish-gsd-ud-2.15-241121                    | Raw text          | 99.73  | 93.84  | 97.10 | —      | 96.86  | 95.15   | 98.61  | 92.52 | 90.36 | 78.94 | 84.50 |
| spanish-gsd-ud-2.15-241121                    | Gold tokenization | —      | —      | 97.35 | —      | 97.12  | 95.38   | 98.86  | 93.42 | 91.18 | 79.71 | 85.29 |
| swedish-talbanken-ud-2.15-241121              | Raw text          | 99.84  | 96.53  | 98.41 | 97.22  | 97.21  | 96.19   | 98.62  | 92.73 | 90.34 | 84.18 | 87.11 |
| swedish-talbanken-ud-2.15-241121              | Gold tokenization | —      | —      | 98.59 | 97.41  | 97.39  | 96.40   | 98.78  | 93.12 | 90.72 | 84.68 | 87.54 |
| swedish-lines-ud-2.15-241121                  | Raw text          | 99.96  | 88.50  | 97.70 | 95.42  | 92.96  | 89.90   | 97.78  | 91.13 | 87.97 | 75.30 | 82.66 |
| swedish-lines-ud-2.15-241121                  | Gold tokenization | —      | —      | 97.73 | 95.49  | 93.04  | 89.96   | 97.82  | 91.89 | 88.67 | 76.02 | 83.39 |
| tamil-ttb-ud-2.15-241121                      | Raw text          | 94.26  | 97.52  | 84.19 | 82.27  | 84.29  | 77.71   | 89.35  | 70.73 | 62.23 | 50.63 | 55.60 |
| tamil-ttb-ud-2.15-241121                      | Gold tokenization | —      | —      | 89.04 | 87.03  | 89.49  | 82.20   | 94.32  | 78.33 | 68.98 | 56.63 | 61.77 |
| telugu-mtg-ud-2.15-241121                     | Raw text          | 99.58  | 96.62  | 93.63 | 93.63  | 98.48  | 93.35   | —      | 90.03 | 83.24 | 76.00 | 79.24 |
| telugu-mtg-ud-2.15-241121                     | Gold tokenization | —      | —      | 94.04 | 94.04  | 98.89  | 93.76   | —      | 90.98 | 84.05 | 76.64 | 79.89 |
| turkish-boun-ud-2.15-241121                   | Raw text          | 96.57  | 86.25  | 89.96 | 85.96  | 80.92  | 71.62   | 90.60  | 72.93 | 66.98 | 49.09 | 61.03 |
| turkish-boun-ud-2.15-241121                   | Gold tokenization | —      | —      | 93.00 | 88.89  | 82.90  | 73.32   | 93.62  | 80.45 | 73.81 | 52.59 | 66.63 |
| turkish-atis-ud-2.15-241121                   | Raw text          | 99.90  | 79.28  | 98.42 | —      | 97.97  | 97.82   | 98.96  | 89.31 | 87.56 | 84.85 | 86.15 |
| turkish-atis-ud-2.15-241121                   | Gold tokenization | —      | —      | 98.53 | —      | 98.11  | 97.94   | 99.07  | 91.63 | 89.72 | 86.98 | 88.39 |
| turkish-framenet-ud-2.15-241121               | Raw text          | 99.90  | 99.27  | 96.83 | —      | 94.79  | 93.97   | 96.63  | 93.36 | 84.36 | 74.37 | 77.84 |
| turkish-framenet-ud-2.15-241121               | Gold tokenization | —      | —      | 96.93 | —      | 94.89  | 94.07   | 96.73  | 93.52 | 84.53 | 74.50 | 77.97 |
| turkish-imst-ud-2.15-241121                   | Raw text          | 97.31  | 97.38  | 92.73 | 92.53  | 89.44  | 86.59   | 93.55  | 76.48 | 69.29 | 58.17 | 63.97 |
| turkish-imst-ud-2.15-241121                   | Gold tokenization | —      | —      | 95.20 | 94.86  | 91.65  | 88.65   | 95.88  | 81.39 | 73.76 | 60.66 | 67.22 |
| turkish-kenet-ud-2.15-241121                  | Raw text          | 100.00 | 98.12  | 93.78 | —      | 91.90  | 90.80   | 93.52  | 84.15 | 71.53 | 62.31 | 65.22 |
| turkish-kenet-ud-2.15-241121                  | Gold tokenization | —      | —      | 93.80 | —      | 91.91  | 90.82   | 93.51  | 84.28 | 71.61 | 62.40 | 65.30 |
| turkish-penn-ud-2.15-241121                   | Raw text          | 99.27  | 82.89  | 95.68 | —      | 94.46  | 93.41   | 94.28  | 84.72 | 72.21 | 62.67 | 65.14 |
| turkish-penn-ud-2.15-241121                   | Gold tokenization | —      | —      | 96.39 | —      | 95.10  | 94.06   | 94.95  | 86.91 | 74.09 | 63.77 | 66.34 |
| turkish-tourism-ud-2.15-241121                | Raw text          | 99.99  | 100.00 | 98.79 | —      | 94.98  | 94.57   | 98.28  | 97.14 | 91.49 | 81.66 | 87.10 |
| turkish-tourism-ud-2.15-241121                | Gold tokenization | —      | —      | 98.80 | —      | 94.99  | 94.59   | 98.30  | 97.15 | 91.50 | 81.68 | 87.12 |
| turkish_german-sagt-ud-2.15-241121            | Raw text          | 98.91  | 99.44  | 90.21 | —      | 80.19  | 75.40   | 90.76  | 70.92 | 60.68 | 40.99 | 50.56 |
| turkish_german-sagt-ud-2.15-241121            | Gold tokenization | —      | —      | 91.11 | —      | 80.80  | 75.92   | 91.47  | 72.37 | 61.79 | 41.55 | 51.31 |
| ukrainian-iu-ud-2.15-241121                   | Raw text          | 99.81  | 96.23  | 98.02 | 94.29  | 94.51  | 93.26   | 97.63  | 90.72 | 88.37 | 79.31 | 83.57 |
| ukrainian-iu-ud-2.15-241121                   | Gold tokenization | —      | —      | 98.23 | 94.50  | 94.70  | 93.45   | 97.82  | 91.32 | 88.94 | 79.69 | 84.00 |
| ukrainian-parlamint-ud-2.15-241121            | Raw text          | 99.88  | 99.62  | 98.34 | 98.54  | 94.91  | 93.95   | 98.89  | 93.36 | 90.71 | 81.65 | 87.38 |
| ukrainian-parlamint-ud-2.15-241121            | Gold tokenization | —      | —      | 98.47 | 98.64  | 95.00  | 94.04   | 99.01  | 93.53 | 90.89 | 81.71 | 87.50 |
| urdu-udtb-ud-2.15-241121                      | Raw text          | 100.00 | 98.31  | 94.18 | 92.31  | 82.87  | 78.61   | 97.35  | 88.00 | 82.88 | 57.44 | 75.16 |
| urdu-udtb-ud-2.15-241121                      | Gold tokenization | —      | —      | 94.17 | 92.29  | 82.85  | 78.58   | 97.37  | 88.11 | 82.96 | 57.43 | 75.25 |
| uyghur-udt-ud-2.15-241121                     | Raw text          | 99.54  | 81.87  | 89.74 | 91.79  | 87.99  | 80.67   | 94.74  | 75.59 | 64.70 | 50.04 | 57.43 |
| uyghur-udt-ud-2.15-241121                     | Gold tokenization | —      | —      | 90.21 | 92.34  | 88.44  | 81.14   | 95.23  | 77.35 | 66.34 | 51.14 | 58.63 |
| vietnamese-vtb-ud-2.15-241121                 | Raw text          | 86.06  | 93.73  | 78.47 | 77.51  | —      | 77.34   | 85.77  | 56.62 | 49.62 | 41.05 | 45.25 |
| vietnamese-vtb-ud-2.15-241121                 | Gold tokenization | —      | —      | 89.83 | 88.75  | —      | 88.54   | 99.51  | 76.22 | 65.85 | 55.29 | 61.23 |
| welsh-ccg-ud-2.15-241121                      | Raw text          | 99.56  | 97.79  | 95.63 | 94.63  | 89.87  | 87.59   | 94.69  | 87.52 | 81.57 | 63.84 | 70.76 |
| welsh-ccg-ud-2.15-241121                      | Gold tokenization | —      | —      | 96.03 | 95.00  | 90.26  | 87.95   | 95.11  | 88.54 | 82.54 | 64.77 | 71.75 |
| western_armenian-armtdp-ud-2.15-241121        | Raw text          | 99.89  | 98.68  | 96.91 | —      | 92.71  | 91.97   | 97.10  | 89.37 | 84.87 | 70.30 | 76.52 |
| western_armenian-armtdp-ud-2.15-241121        | Gold tokenization | —      | —      | 96.98 | —      | 92.80  | 92.04   | 97.20  | 89.64 | 85.11 | 70.54 | 76.76 |
| wolof-wtb-ud-2.15-241121                      | Raw text          | 99.23  | 91.95  | 94.07 | 93.99  | 93.53  | 91.34   | 95.15  | 84.04 | 78.76 | 66.90 | 70.21 |
| wolof-wtb-ud-2.15-241121                      | Gold tokenization | —      | —      | 95.08 | 94.96  | 94.33  | 92.24   | 95.93  | 86.19 | 80.86 | 69.06 | 72.20 |
