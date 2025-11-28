## Universal Dependencies 2.17 Models #universal_dependencies_217_models

Universal Dependencies 2.17 Models are distributed under the
[CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The models are based solely on [Universal Dependencies
2.17](http://hdl.handle.net/11234/1-6036) treebanks, and additionally
use [multilingual BERT](https://github.com/google-research/bert/blob/master/multilingual.md)
and [RobeCzech](https://huggingface.co/ufal/robeczech-base).

The models require [UDPipe 2](https://ufal.mff.cuni.cz/udpipe/2).

### Download

The latest version 251125 of the Universal Dependencies 2.17 models
can be downloaded from [LINDAT/CLARIN repository](http://hdl.handle.net/11234/1-6046).

The models are also available in the [REST service](https://lindat.mff.cuni.cz/services/udpipe/).

### Acknowledgements #universal_dependencies_217_models_acknowledgements

This work has been supported by the Ministry of Education, Youth and Sports of
the Czech Republic, Project No. LM2023062 LINDAT/CLARIAH-CZ.

The models were trained on [Universal Dependencies 2.17](http://hdl.handle.net/11234/1-6036) treebanks.

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

The Universal Dependencies 2.17 models contain 169 models of 93 languages, each
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
| afrikaans-afribooms-ud-2.17-251125            | Raw text          | 99.91  | 99.29  | 98.64 | 95.86  | 98.23  | 95.65   | 98.29  | 90.57 | 87.56 | 79.14 | 79.99 |
| afrikaans-afribooms-ud-2.17-251125            | Gold tokenization | —      | —      | 98.73 | 95.94  | 98.32  | 95.73   | 98.36  | 90.76 | 87.76 | 79.36 | 80.16 |
| albanian-staf-ud-2.17-251125                  | Raw text          | 98.39  | 92.68  | 87.90 | 98.39  | 70.16  | 67.47   | 80.65  | 77.96 | 65.86 | 33.74 | 37.65 |
| albanian-staf-ud-2.17-251125                  | Gold tokenization | —      | —      | 88.71 | 100.00 | 70.16  | 67.47   | 81.99  | 77.15 | 64.52 | 33.09 | 36.98 |
| ancient_greek-proiel-ud-2.17-251125           | Raw text          | 99.98  | 49.19  | 97.66 | 97.95  | 92.14  | 90.80   | 94.69  | 82.05 | 78.23 | 62.37 | 66.33 |
| ancient_greek-proiel-ud-2.17-251125           | Gold tokenization | —      | —      | 97.85 | 98.15  | 92.55  | 91.36   | 94.74  | 86.71 | 82.82 | 68.49 | 72.07 |
| ancient_greek-perseus-ud-2.17-251125          | Raw text          | 99.97  | 98.85  | 92.60 | 85.66  | 91.69  | 84.95   | 86.55  | 80.00 | 74.55 | 54.32 | 55.63 |
| ancient_greek-perseus-ud-2.17-251125          | Gold tokenization | —      | —      | 92.67 | 85.73  | 91.75  | 85.03   | 86.58  | 80.18 | 74.71 | 54.48 | 55.76 |
| ancient_greek-ptnk-ud-2.17-251125             | Raw text          | 99.97  | 55.12  | 98.57 | —      | 91.75  | 90.85   | 94.84  | 88.44 | 84.30 | 66.93 | 71.38 |
| ancient_greek-ptnk-ud-2.17-251125             | Gold tokenization | —      | —      | 98.55 | —      | 91.80  | 90.85   | 94.85  | 92.12 | 87.85 | 70.15 | 74.70 |
| ancient_hebrew-ptnk-ud-2.17-251125            | Raw text          | 82.97  | 94.40  | 81.80 | 81.86  | 81.52  | 80.75   | 81.20  | 66.33 | 64.81 | 53.12 | 53.83 |
| ancient_hebrew-ptnk-ud-2.17-251125            | Gold tokenization | —      | —      | 98.31 | 98.46  | 97.78  | 96.72   | 96.38  | 94.56 | 92.04 | 83.11 | 82.01 |
| arabic-padt-ud-2.17-251125                    | Raw text          | 94.58  | 82.09  | 91.74 | 89.06  | 89.14  | 88.71   | 90.30  | 78.93 | 75.81 | 67.29 | 69.21 |
| arabic-padt-ud-2.17-251125                    | Gold tokenization | —      | —      | 96.96 | 94.31  | 94.46  | 93.93   | 95.24  | 87.30 | 83.79 | 75.36 | 76.72 |
| armenian-armtdp-ud-2.17-251125                | Raw text          | 99.54  | 98.74  | 97.67 | —      | 94.17  | 93.36   | 96.76  | 89.91 | 86.17 | 76.41 | 80.50 |
| armenian-armtdp-ud-2.17-251125                | Gold tokenization | —      | —      | 98.06 | —      | 94.57  | 93.75   | 97.13  | 90.79 | 87.01 | 76.95 | 81.03 |
| armenian-bsut-ud-2.17-251125                  | Raw text          | 99.76  | 97.63  | 97.53 | —      | 93.83  | 93.04   | 97.42  | 90.35 | 86.18 | 73.96 | 79.95 |
| armenian-bsut-ud-2.17-251125                  | Gold tokenization | —      | —      | 97.79 | —      | 94.10  | 93.32   | 97.68  | 91.10 | 86.90 | 74.66 | 80.63 |
| basque-bdt-ud-2.17-251125                     | Raw text          | 99.97  | 99.83  | 96.41 | —      | 93.39  | 91.56   | 96.38  | 87.99 | 84.93 | 75.06 | 79.52 |
| basque-bdt-ud-2.17-251125                     | Gold tokenization | —      | —      | 96.43 | —      | 93.42  | 91.58   | 96.39  | 88.02 | 84.97 | 75.09 | 79.54 |
| belarusian-hse-ud-2.17-251125                 | Raw text          | 99.37  | 86.58  | 97.61 | 97.06  | 94.43  | 93.01   | 93.11  | 86.81 | 84.81 | 75.76 | 75.98 |
| belarusian-hse-ud-2.17-251125                 | Gold tokenization | —      | —      | 98.23 | 97.64  | 95.09  | 93.62   | 93.69  | 89.42 | 87.24 | 77.85 | 77.96 |
| bororo-bdt-ud-2.17-251125                     | Raw text          | 99.99  | 97.36  | 91.68 | 90.68  | 89.76  | 84.54   | 91.56  | 78.80 | 72.57 | 59.79 | 63.63 |
| bororo-bdt-ud-2.17-251125                     | Gold tokenization | —      | —      | 91.73 | 90.74  | 89.82  | 84.62   | 91.56  | 79.11 | 72.85 | 59.97 | 63.83 |
| bulgarian-btb-ud-2.17-251125                  | Raw text          | 99.91  | 94.17  | 99.15 | 97.32  | 97.95  | 96.88   | 98.06  | 94.25 | 91.63 | 85.75 | 86.44 |
| bulgarian-btb-ud-2.17-251125                  | Gold tokenization | —      | —      | 99.25 | 97.41  | 98.04  | 96.97   | 98.15  | 95.12 | 92.44 | 86.50 | 87.26 |
| catalan-ancora-ud-2.17-251125                 | Raw text          | 99.94  | 99.49  | 99.07 | 97.16  | 98.67  | 96.88   | 99.43  | 94.84 | 93.32 | 87.89 | 89.33 |
| catalan-ancora-ud-2.17-251125                 | Gold tokenization | —      | —      | 99.15 | 97.27  | 98.75  | 96.98   | 99.49  | 94.98 | 93.46 | 88.08 | 89.49 |
| chinese-gsdsimp-ud-2.17-251125                | Raw text          | 90.13  | 98.80  | 86.67 | 86.83  | 89.52  | 86.16   | 90.09  | 71.74 | 69.52 | 62.23 | 65.70 |
| chinese-gsdsimp-ud-2.17-251125                | Gold tokenization | —      | —      | 95.73 | 95.97  | 99.28  | 95.20   | 99.90  | 86.76 | 83.60 | 77.62 | 81.78 |
| chinese-gsd-ud-2.17-251125                    | Raw text          | 89.81  | 99.10  | 86.31 | 86.58  | 89.14  | 85.85   | 89.76  | 71.83 | 69.46 | 61.93 | 65.42 |
| chinese-gsd-ud-2.17-251125                    | Gold tokenization | —      | —      | 95.85 | 96.03  | 99.28  | 95.30   | 99.91  | 86.84 | 83.71 | 77.61 | 81.87 |
| classical_armenian-caval-ud-2.17-251125       | Raw text          | 99.04  | 60.83  | 97.38 | —      | 95.43  | 94.59   | 97.75  | 83.15 | 79.86 | 69.39 | 73.98 |
| classical_armenian-caval-ud-2.17-251125       | Gold tokenization | —      | —      | 98.29 | —      | 96.28  | 95.41   | 98.54  | 88.83 | 85.33 | 73.67 | 78.52 |
| classical_chinese-kyoto-ud-2.17-251125        | Raw text          | 98.00  | 47.23  | 89.58 | 88.95  | 91.65  | 85.98   | 97.58  | 71.49 | 66.32 | 62.75 | 64.93 |
| classical_chinese-kyoto-ud-2.17-251125        | Gold tokenization | —      | —      | 93.21 | 92.07  | 94.56  | 89.74   | 99.53  | 84.45 | 79.13 | 75.29 | 78.04 |
| coptic-scriptorium-ud-2.17-251125             | Raw text          | 75.42  | 28.57  | 73.46 | 73.26  | 72.83  | 71.84   | 74.21  | 52.08 | 50.03 | 38.53 | 41.03 |
| coptic-scriptorium-ud-2.17-251125             | Gold tokenization | —      | —      | 97.42 | 97.16  | 97.94  | 96.10   | 97.67  | 91.07 | 88.27 | 78.68 | 81.71 |
| coptic-bohairic-ud-2.17-251125                | Raw text          | 73.99  | 60.56  | 71.69 | 71.31  | 71.79  | 70.20   | 72.70  | 52.52 | 50.73 | 38.25 | 40.68 |
| coptic-bohairic-ud-2.17-251125                | Gold tokenization | —      | —      | 97.37 | 96.94  | 97.72  | 95.81   | 97.12  | 91.96 | 89.38 | 79.67 | 81.49 |
| croatian-set-ud-2.17-251125                   | Raw text          | 99.93  | 94.79  | 98.46 | 95.81  | 96.36  | 95.57   | 97.64  | 92.36 | 89.46 | 81.81 | 84.52 |
| croatian-set-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.53 | 95.90  | 96.44  | 95.65   | 97.71  | 92.88 | 89.97 | 82.25 | 84.97 |
| czech-pdtc-ud-2.17-251125                     | Raw text          | 99.96  | 94.82  | 99.24 | 98.42  | 98.88  | 98.23   | 99.54  | 95.62 | 94.59 | 91.51 | 93.05 |
| czech-pdtc-ud-2.17-251125                     | Gold tokenization | —      | —      | 99.28 | 98.47  | 98.92  | 98.27   | 99.58  | 96.24 | 95.21 | 92.04 | 93.59 |
| czech-cac-ud-2.17-251125                      | Raw text          | 99.99  | 99.68  | 99.58 | 98.20  | 97.85  | 97.41   | 99.27  | 96.25 | 94.95 | 90.44 | 92.97 |
| czech-cac-ud-2.17-251125                      | Gold tokenization | —      | —      | 99.59 | 98.20  | 97.85  | 97.42   | 99.28  | 96.25 | 94.95 | 90.46 | 92.99 |
| czech-cltt-ud-2.17-251125                     | Raw text          | 99.32  | 96.92  | 98.95 | 93.77  | 94.05  | 93.69   | 98.45  | 89.75 | 88.17 | 80.70 | 86.21 |
| czech-cltt-ud-2.17-251125                     | Gold tokenization | —      | —      | 99.40 | 94.20  | 94.53  | 94.11   | 98.90  | 89.85 | 88.02 | 80.35 | 85.98 |
| czech-fictree-ud-2.17-251125                  | Raw text          | 99.99  | 98.95  | 99.18 | 96.37  | 97.25  | 96.19   | 99.29  | 96.31 | 94.90 | 88.76 | 92.67 |
| czech-fictree-ud-2.17-251125                  | Gold tokenization | —      | —      | 99.19 | 96.38  | 97.26  | 96.19   | 99.30  | 96.40 | 94.97 | 88.85 | 92.76 |
| danish-ddt-ud-2.17-251125                     | Raw text          | 99.82  | 89.80  | 97.99 | 99.82  | 97.44  | 96.65   | 97.52  | 88.59 | 86.60 | 79.55 | 81.59 |
| danish-ddt-ud-2.17-251125                     | Gold tokenization | —      | —      | 98.20 | 100.00 | 97.63  | 96.88   | 97.65  | 89.84 | 87.87 | 80.77 | 82.73 |
| dutch-alpino-ud-2.17-251125                   | Raw text          | 99.78  | 91.16  | 97.97 | 96.89  | 97.52  | 96.56   | 95.99  | 94.12 | 92.25 | 85.14 | 83.40 |
| dutch-alpino-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.17 | 97.12  | 97.75  | 96.73   | 96.20  | 95.30 | 93.37 | 86.35 | 84.46 |
| dutch-lassysmall-ud-2.17-251125               | Raw text          | 99.84  | 86.58  | 97.77 | 96.97  | 97.74  | 96.49   | 96.33  | 93.47 | 91.65 | 84.37 | 83.03 |
| dutch-lassysmall-ud-2.17-251125               | Gold tokenization | —      | —      | 97.97 | 97.24  | 98.01  | 96.81   | 96.48  | 95.17 | 93.29 | 86.49 | 85.16 |
| egyptian-ujaen-ud-2.17-251125                 | Raw text          | 98.72  | 16.94  | 89.97 | —      | 77.52  | 75.21   | 89.34  | 73.02 | 63.89 | 50.42 | 56.11 |
| egyptian-ujaen-ud-2.17-251125                 | Gold tokenization | —      | —      | 91.74 | —      | 79.44  | 77.31   | 90.38  | 86.12 | 75.93 | 60.36 | 67.62 |
| english-ewt-ud-2.17-251125                    | Raw text          | 99.07  | 88.42  | 96.75 | 96.27  | 97.21  | 95.51   | 97.13  | 91.59 | 89.91 | 83.87 | 84.91 |
| english-ewt-ud-2.17-251125                    | Gold tokenization | —      | —      | 97.61 | 97.13  | 98.13  | 96.34   | 97.96  | 94.09 | 92.36 | 86.29 | 87.26 |
| english-atis-ud-2.17-251125                   | Raw text          | 100.00 | 80.03  | 98.95 | —      | 98.37  | 97.98   | 99.64  | 94.30 | 92.95 | 87.74 | 90.23 |
| english-atis-ud-2.17-251125                   | Gold tokenization | —      | —      | 99.07 | —      | 98.53  | 98.15   | 99.64  | 96.08 | 94.51 | 90.02 | 92.37 |
| english-childes-ud-2.17-251125                | Raw text          | 99.97  | 100.00 | 97.05 | 95.72  | 99.97  | 94.85   | 98.17  | 94.07 | 91.32 | 84.93 | 87.25 |
| english-childes-ud-2.17-251125                | Gold tokenization | —      | —      | 97.08 | 95.74  | 100.00 | 94.87   | 98.20  | 94.13 | 91.38 | 84.97 | 87.30 |
| english-eslspok-ud-2.17-251125                | Raw text          | 99.87  | 88.60  | 98.46 | 98.68  | —      | 98.15   | —      | 93.82 | 92.46 | 90.02 | 91.77 |
| english-eslspok-ud-2.17-251125                | Gold tokenization | —      | —      | 98.63 | 98.81  | —      | 98.28   | —      | 95.85 | 94.44 | 91.60 | 93.44 |
| english-gum-ud-2.17-251125                    | Raw text          | 99.74  | 95.77  | 98.12 | 98.06  | 98.04  | 97.09   | 98.83  | 93.20 | 91.54 | 86.12 | 87.61 |
| english-gum-ud-2.17-251125                    | Gold tokenization | —      | —      | 98.36 | 98.31  | 98.28  | 97.32   | 99.07  | 93.82 | 92.14 | 86.67 | 88.15 |
| english-lines-ud-2.17-251125                  | Raw text          | 99.92  | 89.02  | 97.74 | 96.83  | 96.87  | 94.13   | 98.32  | 91.09 | 88.45 | 80.04 | 83.47 |
| english-lines-ud-2.17-251125                  | Gold tokenization | —      | —      | 97.79 | 96.90  | 96.90  | 94.14   | 98.37  | 91.89 | 89.21 | 80.70 | 84.26 |
| english-partut-ud-2.17-251125                 | Raw text          | 99.72  | 98.04  | 97.46 | 97.40  | 96.52  | 95.61   | 98.28  | 93.76 | 91.91 | 83.08 | 86.86 |
| english-partut-ud-2.17-251125                 | Gold tokenization | —      | —      | 97.68 | 97.65  | 96.77  | 95.86   | 98.56  | 93.98 | 92.12 | 83.33 | 87.17 |
| erzya-jr-ud-2.17-251125                       | Raw text          | 99.12  | 97.02  | 88.50 | 88.04  | 79.36  | 74.38   | 84.27  | 73.33 | 63.92 | 42.32 | 48.20 |
| erzya-jr-ud-2.17-251125                       | Gold tokenization | —      | —      | 89.25 | 88.73  | 79.93  | 74.86   | 84.87  | 74.25 | 64.70 | 42.76 | 48.63 |
| estonian-edt-ud-2.17-251125                   | Raw text          | 99.94  | 91.88  | 97.90 | 98.44  | 96.78  | 95.79   | 95.42  | 89.33 | 87.01 | 81.75 | 80.59 |
| estonian-edt-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.02 | 98.50  | 96.88  | 95.90   | 95.49  | 90.28 | 87.89 | 82.60 | 81.37 |
| estonian-ewt-ud-2.17-251125                   | Raw text          | 98.63  | 78.03  | 94.91 | 96.06  | 93.99  | 91.76   | 93.98  | 83.31 | 80.02 | 72.33 | 73.67 |
| estonian-ewt-ud-2.17-251125                   | Gold tokenization | —      | —      | 96.31 | 97.45  | 95.27  | 93.16   | 95.23  | 87.38 | 83.84 | 75.51 | 76.74 |
| faroese-farpahc-ud-2.17-251125                | Raw text          | 99.74  | 92.77  | 97.48 | 92.97  | 94.37  | 92.46   | 99.74  | 86.48 | 82.71 | 68.94 | 75.86 |
| faroese-farpahc-ud-2.17-251125                | Gold tokenization | —      | —      | 97.65 | 93.15  | 94.59  | 92.62   | 100.00 | 87.41 | 83.58 | 69.89 | 76.99 |
| finnish-tdt-ud-2.17-251125                    | Raw text          | 99.66  | 89.97  | 97.67 | 98.25  | 96.10  | 95.21   | 92.07  | 90.20 | 88.07 | 81.95 | 77.83 |
| finnish-tdt-ud-2.17-251125                    | Gold tokenization | —      | —      | 98.04 | 98.60  | 96.42  | 95.57   | 92.36  | 91.65 | 89.50 | 83.20 | 78.95 |
| finnish-ftb-ud-2.17-251125                    | Raw text          | 99.91  | 86.84  | 96.73 | 94.90  | 96.69  | 93.89   | 95.66  | 90.21 | 87.31 | 80.22 | 80.78 |
| finnish-ftb-ud-2.17-251125                    | Gold tokenization | —      | —      | 97.05 | 95.17  | 96.85  | 94.23   | 95.81  | 92.38 | 89.42 | 82.72 | 83.19 |
| french-gsd-ud-2.17-251125                     | Raw text          | 98.88  | 93.82  | 97.43 | —      | 97.15  | 96.70   | 97.73  | 93.26 | 91.21 | 85.02 | 86.87 |
| french-gsd-ud-2.17-251125                     | Gold tokenization | —      | —      | 98.55 | —      | 98.30  | 97.83   | 98.85  | 94.99 | 93.11 | 86.95 | 88.12 |
| french-alts-ud-2.17-251125                    | Raw text          | 97.99  | 77.92  | 96.06 | 95.90  | 95.43  | 93.24   | 91.44  | 87.04 | 83.72 | 75.81 | 73.43 |
| french-alts-ud-2.17-251125                    | Gold tokenization | —      | —      | 98.03 | 97.62  | 97.43  | 94.94   | 93.32  | 90.98 | 87.72 | 79.47 | 76.25 |
| french-parisstories-ud-2.17-251125            | Raw text          | 99.72  | 94.65  | 98.06 | —      | 96.20  | 95.08   | 99.08  | 82.68 | 79.46 | 70.29 | 76.26 |
| french-parisstories-ud-2.17-251125            | Gold tokenization | —      | —      | 98.33 | —      | 96.43  | 95.31   | 99.35  | 83.59 | 80.39 | 70.94 | 76.98 |
| french-partut-ud-2.17-251125                  | Raw text          | 99.42  | 98.64  | 97.85 | 97.47  | 95.24  | 94.71   | 98.12  | 94.74 | 92.87 | 82.71 | 88.36 |
| french-partut-ud-2.17-251125                  | Gold tokenization | —      | —      | 98.46 | 98.08  | 95.85  | 95.31   | 98.69  | 95.24 | 93.51 | 83.52 | 88.72 |
| french-rhapsodie-ud-2.17-251125               | Raw text          | 99.16  | 99.82  | 97.59 | —      | 95.65  | 94.70   | 98.26  | 87.65 | 84.59 | 75.14 | 80.49 |
| french-rhapsodie-ud-2.17-251125               | Gold tokenization | —      | —      | 98.44 | —      | 96.47  | 95.55   | 99.08  | 88.89 | 85.81 | 76.16 | 81.24 |
| french-sequoia-ud-2.17-251125                 | Raw text          | 99.09  | 90.20  | 98.48 | —      | 97.69  | 97.44   | 98.37  | 93.87 | 92.58 | 87.16 | 89.31 |
| french-sequoia-ud-2.17-251125                 | Gold tokenization | —      | —      | 99.36 | —      | 98.61  | 98.34   | 99.24  | 95.59 | 94.36 | 89.13 | 90.51 |
| galician-treegal-ud-2.17-251125               | Raw text          | 98.74  | 87.99  | 96.10 | 94.35  | 95.22  | 93.36   | 97.31  | 83.47 | 79.23 | 68.14 | 72.39 |
| galician-treegal-ud-2.17-251125               | Gold tokenization | —      | —      | 97.35 | 95.42  | 96.30  | 94.41   | 98.56  | 86.85 | 82.33 | 71.66 | 76.23 |
| galician-ctg-ud-2.17-251125                   | Raw text          | 99.22  | 97.22  | 97.20 | 97.03  | 99.06  | 96.58   | 98.07  | 85.38 | 82.90 | 71.22 | 75.73 |
| galician-ctg-ud-2.17-251125                   | Gold tokenization | —      | —      | 97.94 | 97.75  | 99.84  | 97.30   | 98.83  | 87.05 | 84.46 | 73.10 | 77.61 |
| georgian-glc-ud-2.17-251125                   | Raw text          | 99.12  | 95.88  | 96.37 | 96.34  | 91.80  | 91.43   | 94.16  | 83.58 | 79.76 | 70.43 | 74.36 |
| georgian-glc-ud-2.17-251125                   | Gold tokenization | —      | —      | 97.19 | 97.08  | 92.52  | 92.09   | 94.86  | 85.33 | 81.35 | 71.39 | 75.64 |
| georgian-gnc-ud-2.17-251125                   | Raw text          | 98.25  | 91.00  | 90.74 | 71.76  | 81.85  | 70.24   | 85.62  | 82.19 | 75.71 | 53.87 | 58.00 |
| georgian-gnc-ud-2.17-251125                   | Gold tokenization | —      | —      | 92.23 | 72.72  | 83.13  | 71.15   | 86.85  | 85.60 | 78.69 | 55.57 | 59.90 |
| german-gsd-ud-2.17-251125                     | Raw text          | 99.79  | 83.32  | 96.87 | 97.71  | 91.17  | 88.82   | 97.19  | 87.44 | 84.36 | 67.63 | 76.76 |
| german-gsd-ud-2.17-251125                     | Gold tokenization | —      | —      | 97.11 | 97.93  | 91.53  | 89.21   | 97.37  | 89.24 | 86.16 | 69.30 | 78.65 |
| german-hdt-ud-2.17-251125                     | Raw text          | 99.89  | 92.57  | 98.54 | 98.46  | 94.20  | 93.79   | 97.98  | 96.90 | 96.04 | 84.97 | 91.01 |
| german-hdt-ud-2.17-251125                     | Gold tokenization | —      | —      | 98.66 | 98.59  | 94.32  | 93.92   | 98.08  | 97.58 | 96.74 | 85.65 | 91.71 |
| gothic-proiel-ud-2.17-251125                  | Raw text          | 100.00 | 31.12  | 96.19 | 96.73  | 90.02  | 88.02   | 94.70  | 78.65 | 72.66 | 58.56 | 63.24 |
| gothic-proiel-ud-2.17-251125                  | Gold tokenization | —      | —      | 96.79 | 97.24  | 91.28  | 89.61   | 94.80  | 86.83 | 80.92 | 68.96 | 72.81 |
| greek-gdt-ud-2.17-251125                      | Raw text          | 99.87  | 90.19  | 98.16 | 98.17  | 95.76  | 95.10   | 96.08  | 93.15 | 91.38 | 82.07 | 82.11 |
| greek-gdt-ud-2.17-251125                      | Gold tokenization | —      | —      | 98.29 | 98.29  | 95.79  | 95.12   | 96.17  | 93.98 | 92.16 | 82.77 | 82.88 |
| greek-cretan-ud-2.17-251125                   | Raw text          | 100.00 | 100.00 | 92.90 | 87.83  | 86.00  | 76.88   | 88.84  | 85.80 | 78.90 | 53.66 | 60.41 |
| greek-cretan-ud-2.17-251125                   | Gold tokenization | —      | —      | 92.90 | 87.83  | 86.00  | 76.88   | 88.84  | 85.80 | 78.90 | 53.66 | 60.41 |
| greek-gud-ud-2.17-251125                      | Raw text          | 99.94  | 95.24  | 98.08 | —      | 94.36  | 93.24   | 95.93  | 93.61 | 90.37 | 77.62 | 81.34 |
| greek-gud-ud-2.17-251125                      | Gold tokenization | —      | —      | 98.13 | —      | 94.41  | 93.30   | 95.97  | 94.05 | 90.79 | 77.89 | 81.70 |
| haitian_creole-adolphe-ud-2.17-251125         | Raw text          | 99.97  | 98.99  | 95.53 | 99.97  | 97.98  | 94.44   | 99.02  | 64.82 | 60.63 | 50.13 | 57.42 |
| haitian_creole-adolphe-ud-2.17-251125         | Gold tokenization | —      | —      | 95.55 | 100.00 | 98.02  | 94.47   | 99.05  | 64.83 | 60.63 | 50.10 | 57.44 |
| hebrew-htb-ud-2.17-251125                     | Raw text          | 85.09  | 99.69  | 82.85 | 82.81  | 81.34  | 80.64   | 82.96  | 70.67 | 68.16 | 55.59 | 59.82 |
| hebrew-htb-ud-2.17-251125                     | Gold tokenization | —      | —      | 97.51 | 97.51  | 95.96  | 95.29   | 97.37  | 92.78 | 90.26 | 79.83 | 82.61 |
| hebrew-iahltknesset-ud-2.17-251125            | Raw text          | 88.01  | 100.00 | 85.33 | 85.33  | 81.64  | 80.93   | 86.84  | 71.65 | 69.38 | 56.52 | 63.27 |
| hebrew-iahltknesset-ud-2.17-251125            | Gold tokenization | —      | —      | 97.04 | 97.03  | 92.64  | 91.93   | 98.27  | 90.34 | 87.70 | 73.21 | 81.46 |
| hebrew-iahltwiki-ud-2.17-251125               | Raw text          | 88.56  | 97.16  | 86.16 | 86.17  | 81.75  | 80.87   | 87.24  | 75.73 | 74.01 | 58.38 | 67.03 |
| hebrew-iahltwiki-ud-2.17-251125               | Gold tokenization | —      | —      | 97.12 | 97.11  | 92.55  | 91.52   | 98.30  | 93.63 | 91.23 | 75.93 | 85.50 |
| hindi-hdtb-ud-2.17-251125                     | Raw text          | 100.00 | 98.72  | 97.61 | 97.16  | 94.07  | 92.14   | 98.86  | 95.29 | 92.30 | 79.44 | 87.60 |
| hindi-hdtb-ud-2.17-251125                     | Gold tokenization | —      | —      | 97.60 | 97.15  | 94.10  | 92.17   | 98.87  | 95.38 | 92.38 | 79.54 | 87.70 |
| hungarian-szeged-ud-2.17-251125               | Raw text          | 99.85  | 95.89  | 96.73 | —      | 94.16  | 93.48   | 95.11  | 88.12 | 84.56 | 74.51 | 78.03 |
| hungarian-szeged-ud-2.17-251125               | Gold tokenization | —      | —      | 96.83 | —      | 94.28  | 93.58   | 95.22  | 88.63 | 85.01 | 74.78 | 78.34 |
| icelandic-modern-ud-2.17-251125               | Raw text          | 99.44  | 94.59  | 97.70 | 95.30  | 89.29  | 86.45   | 97.17  | 86.45 | 83.47 | 65.32 | 75.89 |
| icelandic-modern-ud-2.17-251125               | Gold tokenization | —      | —      | 98.20 | 95.83  | 89.72  | 86.87   | 97.66  | 87.37 | 84.21 | 66.04 | 76.63 |
| icelandic-gc-ud-2.17-251125                   | Raw text          | 99.72  | 94.64  | 94.74 | 82.37  | 85.02  | 79.85   | 91.69  | 83.53 | 79.16 | 58.89 | 69.28 |
| icelandic-gc-ud-2.17-251125                   | Gold tokenization | —      | —      | 95.04 | 82.85  | 85.51  | 80.33   | 91.91  | 84.42 | 79.98 | 59.46 | 69.97 |
| icelandic-icepahc-ud-2.17-251125              | Raw text          | 99.82  | 92.69  | 96.92 | 93.34  | 92.10  | 87.27   | 96.33  | 87.27 | 83.41 | 66.86 | 74.66 |
| icelandic-icepahc-ud-2.17-251125              | Gold tokenization | —      | —      | 97.08 | 93.57  | 92.24  | 87.48   | 96.47  | 87.73 | 83.82 | 67.26 | 75.13 |
| indonesian-gsd-ud-2.17-251125                 | Raw text          | 99.49  | 93.04  | 94.35 | 93.95  | 95.55  | 88.87   | 98.14  | 87.65 | 81.91 | 72.73 | 77.29 |
| indonesian-gsd-ud-2.17-251125                 | Gold tokenization | —      | —      | 94.79 | 94.39  | 95.99  | 89.24   | 98.59  | 88.53 | 82.72 | 73.61 | 78.26 |
| indonesian-csui-ud-2.17-251125                | Raw text          | 99.45  | 91.01  | 95.93 | 96.06  | 96.70  | 95.37   | 98.26  | 86.10 | 81.65 | 76.00 | 78.29 |
| indonesian-csui-ud-2.17-251125                | Gold tokenization | —      | —      | 96.45 | 96.62  | 97.20  | 95.92   | 98.89  | 87.43 | 82.77 | 76.85 | 79.26 |
| irish-idt-ud-2.17-251125                      | Raw text          | 99.88  | 97.58  | 96.35 | 95.09  | 91.32  | 88.41   | 95.77  | 87.31 | 81.93 | 65.68 | 72.41 |
| irish-idt-ud-2.17-251125                      | Gold tokenization | —      | —      | 96.46 | 95.27  | 91.43  | 88.57   | 95.88  | 87.61 | 82.20 | 65.80 | 72.51 |
| irish-twittirish-ud-2.17-251125               | Raw text          | 98.50  | 46.62  | 90.78 | —      | —      | 90.78   | 88.48  | 78.71 | 72.69 | 58.78 | 57.30 |
| irish-twittirish-ud-2.17-251125               | Gold tokenization | —      | —      | 92.03 | —      | —      | 92.03   | 89.72  | 85.81 | 79.35 | 66.65 | 64.40 |
| italian-isdt-ud-2.17-251125                   | Raw text          | 99.74  | 99.07  | 98.49 | 98.40  | 98.17  | 97.70   | 98.67  | 94.76 | 93.10 | 86.93 | 88.08 |
| italian-isdt-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.73 | 98.66  | 98.42  | 97.96   | 98.92  | 95.19 | 93.55 | 87.35 | 88.54 |
| italian-markit-ud-2.17-251125                 | Raw text          | 99.62  | 98.24  | 97.02 | 97.03  | 94.02  | 92.37   | 88.31  | 88.68 | 84.79 | 70.25 | 77.98 |
| italian-markit-ud-2.17-251125                 | Gold tokenization | —      | —      | 97.36 | 97.36  | 94.28  | 92.60   | 88.62  | 89.41 | 85.54 | 70.84 | 78.75 |
| italian-old-ud-2.17-251125                    | Raw text          | 99.11  | 96.41  | 96.38 | 86.42  | 92.04  | 83.07   | 96.53  | 85.13 | 80.63 | 64.29 | 72.33 |
| italian-old-ud-2.17-251125                    | Gold tokenization | —      | —      | 97.23 | 87.00  | 92.78  | 83.68   | 97.38  | 87.64 | 82.95 | 66.60 | 74.98 |
| italian-parlamint-ud-2.17-251125              | Raw text          | 99.42  | 94.12  | 98.58 | 97.99  | 97.95  | 97.09   | 98.69  | 91.79 | 89.72 | 83.94 | 85.74 |
| italian-parlamint-ud-2.17-251125              | Gold tokenization | —      | —      | 99.19 | 98.55  | 98.49  | 97.61   | 99.20  | 93.24 | 91.13 | 85.52 | 87.24 |
| italian-partut-ud-2.17-251125                 | Raw text          | 99.73  | 100.00 | 98.38 | 98.49  | 98.27  | 97.61   | 98.63  | 96.10 | 94.20 | 87.40 | 88.62 |
| italian-partut-ud-2.17-251125                 | Gold tokenization | —      | —      | 98.54 | 98.65  | 98.41  | 97.75   | 98.82  | 96.10 | 94.18 | 87.32 | 88.60 |
| italian-postwita-ud-2.17-251125               | Raw text          | 99.36  | 49.53  | 96.43 | 96.12  | 96.10  | 94.74   | 96.41  | 82.83 | 79.11 | 69.18 | 70.74 |
| italian-postwita-ud-2.17-251125               | Gold tokenization | —      | —      | 97.07 | 96.77  | 96.59  | 95.27   | 97.03  | 88.03 | 84.00 | 75.61 | 77.21 |
| italian-twittiro-ud-2.17-251125               | Raw text          | 98.94  | 46.67  | 95.84 | 95.64  | 95.00  | 93.35   | 94.50  | 82.55 | 77.77 | 64.64 | 65.26 |
| italian-twittiro-ud-2.17-251125               | Gold tokenization | —      | —      | 96.74 | 96.40  | 95.93  | 94.12   | 95.26  | 87.90 | 83.13 | 71.30 | 72.03 |
| italian-vit-ud-2.17-251125                    | Raw text          | 99.75  | 95.06  | 98.15 | 97.33  | 97.62  | 96.16   | 98.84  | 92.35 | 89.54 | 81.45 | 84.18 |
| italian-vit-ud-2.17-251125                    | Gold tokenization | —      | —      | 98.39 | 97.67  | 97.84  | 96.50   | 99.07  | 93.14 | 90.29 | 82.26 | 84.96 |
| japanese-gsdluw-ud-2.17-251125                | Raw text          | 95.16  | 98.72  | 93.96 | 93.67  | 95.15  | 93.59   | 93.60  | 86.29 | 85.66 | 76.46 | 76.76 |
| japanese-gsdluw-ud-2.17-251125                | Gold tokenization | —      | —      | 98.52 | 98.24  | 100.00 | 98.15   | 97.96  | 95.18 | 94.28 | 86.38 | 85.35 |
| japanese-gsd-ud-2.17-251125                   | Raw text          | 96.17  | 100.00 | 95.07 | 94.36  | 96.16  | 94.07   | 95.10  | 88.04 | 87.28 | 80.84 | 81.15 |
| japanese-gsd-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.61 | 97.69  | 99.99  | 97.37   | 98.53  | 94.94 | 94.03 | 88.73 | 88.68 |
| khoekhoe-kdt-ud-2.17-251125                   | Raw text          | 96.83  | 77.55  | 90.60 | 93.87  | 89.79  | 86.96   | 93.60  | 74.39 | 70.37 | 54.10 | 60.75 |
| khoekhoe-kdt-ud-2.17-251125                   | Gold tokenization | —      | —      | 93.65 | 96.75  | 92.74  | 89.78   | 96.51  | 86.55 | 81.70 | 65.86 | 73.25 |
| korean-kaist-ud-2.17-251125                   | Raw text          | 100.00 | 100.00 | 96.21 | 87.81  | —      | 87.60   | 94.39  | 88.95 | 86.97 | 82.88 | 80.67 |
| korean-kaist-ud-2.17-251125                   | Gold tokenization | —      | —      | 96.21 | 87.81  | —      | 87.60   | 94.39  | 88.95 | 86.97 | 82.88 | 80.67 |
| korean-gsd-ud-2.17-251125                     | Raw text          | 99.87  | 93.93  | 96.60 | 90.63  | 99.67  | 88.34   | 93.90  | 88.20 | 84.57 | 81.30 | 77.54 |
| korean-gsd-ud-2.17-251125                     | Gold tokenization | —      | —      | 96.74 | 90.81  | 99.79  | 88.51   | 94.02  | 88.96 | 85.24 | 82.04 | 78.21 |
| korean-ksl-ud-2.17-251125                     | Raw text          | 99.75  | 98.72  | 96.51 | 91.05  | 99.75  | 90.33   | 95.89  | 90.59 | 87.85 | 83.66 | 82.77 |
| korean-ksl-ud-2.17-251125                     | Gold tokenization | —      | —      | 96.77 | 91.26  | 100.00 | 90.53   | 96.10  | 91.06 | 88.28 | 84.01 | 83.08 |
| kyrgyz-ktmu-ud-2.17-251125                    | Raw text          | 99.25  | 97.83  | 90.96 | 90.35  | 77.29  | 72.81   | 88.71  | 84.01 | 73.36 | 54.17 | 63.39 |
| kyrgyz-ktmu-ud-2.17-251125                    | Gold tokenization | —      | —      | 91.66 | 91.06  | 77.98  | 73.47   | 89.39  | 84.78 | 74.02 | 54.61 | 63.89 |
| latin-ittb-ud-2.17-251125                     | Raw text          | 99.98  | 91.79  | 99.06 | 96.37  | 97.11  | 95.41   | 99.22  | 89.50 | 87.21 | 80.76 | 84.45 |
| latin-ittb-ud-2.17-251125                     | Gold tokenization | —      | —      | 99.09 | 96.40  | 97.14  | 95.44   | 99.25  | 90.52 | 88.20 | 81.33 | 85.05 |
| latin-circse-ud-2.17-251125                   | Raw text          | 98.78  | 1.39   | 91.07 | 84.85  | 71.50  | 64.90   | 89.48  | 51.27 | 45.66 | 29.12 | 39.32 |
| latin-circse-ud-2.17-251125                   | Gold tokenization | —      | —      | 92.76 | 87.15  | 73.30  | 67.01   | 90.69  | 68.06 | 61.16 | 41.85 | 54.41 |
| latin-llct-ud-2.17-251125                     | Raw text          | 99.99  | 99.49  | 99.77 | 97.05  | 97.12  | 96.83   | 97.80  | 95.03 | 94.06 | 88.64 | 89.91 |
| latin-llct-ud-2.17-251125                     | Gold tokenization | —      | —      | 99.78 | 97.05  | 97.12  | 96.84   | 97.80  | 95.05 | 94.07 | 88.66 | 89.92 |
| latin-perseus-ud-2.17-251125                  | Raw text          | 98.23  | 99.09  | 91.90 | 80.01  | 83.52  | 76.93   | 87.99  | 78.04 | 71.45 | 53.94 | 59.71 |
| latin-perseus-ud-2.17-251125                  | Gold tokenization | —      | —      | 93.59 | 81.47  | 85.12  | 78.41   | 89.61  | 79.59 | 72.84 | 54.60 | 60.54 |
| latin-proiel-ud-2.17-251125                   | Raw text          | 100.00 | 37.07  | 97.03 | 97.15  | 91.07  | 89.89   | 96.44  | 76.45 | 72.24 | 59.17 | 64.82 |
| latin-proiel-ud-2.17-251125                   | Gold tokenization | —      | —      | 97.25 | 97.40  | 91.55  | 90.47   | 96.51  | 83.80 | 79.43 | 67.27 | 73.27 |
| latin-udante-ud-2.17-251125                   | Raw text          | 99.54  | 98.45  | 91.24 | 76.03  | 84.72  | 73.11   | 87.56  | 76.54 | 69.31 | 49.13 | 52.62 |
| latin-udante-ud-2.17-251125                   | Gold tokenization | —      | —      | 91.49 | 76.15  | 85.02  | 73.19   | 87.82  | 76.78 | 69.47 | 49.18 | 52.73 |
| latvian-lvtb-ud-2.17-251125                   | Raw text          | 99.31  | 97.90  | 97.43 | 92.02  | 95.28  | 91.60   | 96.81  | 89.35 | 86.48 | 79.11 | 81.81 |
| latvian-lvtb-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.09 | 92.69  | 95.97  | 92.27   | 97.48  | 90.37 | 87.47 | 80.23 | 82.94 |
| lithuanian-alksnis-ud-2.17-251125             | Raw text          | 99.91  | 87.87  | 95.96 | 90.29  | 91.02  | 89.36   | 93.40  | 82.45 | 78.63 | 67.83 | 70.97 |
| lithuanian-alksnis-ud-2.17-251125             | Gold tokenization | —      | —      | 96.09 | 90.39  | 91.19  | 89.46   | 93.47  | 83.76 | 79.92 | 68.90 | 71.85 |
| lithuanian-hse-ud-2.17-251125                 | Raw text          | 97.30  | 97.30  | 89.66 | 89.56  | 82.39  | 79.22   | 88.26  | 71.67 | 62.72 | 46.78 | 54.43 |
| lithuanian-hse-ud-2.17-251125                 | Gold tokenization | —      | —      | 91.60 | 91.89  | 84.25  | 80.75   | 90.66  | 74.81 | 64.91 | 47.93 | 55.92 |
| low_saxon-lsdc-ud-2.17-251125                 | Raw text          | 99.25  | 90.23  | 90.25 | —      | 71.74  | 69.09   | 83.84  | 73.91 | 64.88 | 36.33 | 47.68 |
| low_saxon-lsdc-ud-2.17-251125                 | Gold tokenization | —      | —      | 90.80 | —      | 72.23  | 69.46   | 84.33  | 75.13 | 65.93 | 36.74 | 48.15 |
| maghrebi_arabic_french-arabizi-ud-2.17-251125 | Raw text          | 91.65  | 7.00   | 78.43 | 71.21  | 82.47  | 70.04   | 50.87  | 57.85 | 49.93 | 35.97 | 24.36 |
| maghrebi_arabic_french-arabizi-ud-2.17-251125 | Gold tokenization | —      | —      | 85.45 | 78.20  | 90.27  | 76.87   | 54.47  | 74.94 | 64.48 | 45.74 | 30.68 |
| maltese-mudt-ud-2.17-251125                   | Raw text          | 99.84  | 86.29  | 95.78 | 95.79  | 99.84  | 95.34   | —      | 84.86 | 80.04 | 68.41 | 72.50 |
| maltese-mudt-ud-2.17-251125                   | Gold tokenization | —      | —      | 95.88 | 95.90  | 100.00 | 95.43   | —      | 85.36 | 80.47 | 68.61 | 72.75 |
| manx-cadhan-ud-2.17-251125                    | Raw text          | 97.36  | 98.25  | 94.28 | —      | 95.82  | 93.48   | 93.31  | 87.46 | 84.00 | 77.71 | 77.70 |
| manx-cadhan-ud-2.17-251125                    | Gold tokenization | —      | —      | 96.82 | —      | 98.40  | 95.95   | 95.87  | 92.42 | 88.96 | 82.72 | 81.51 |
| marathi-ufal-ud-2.17-251125                   | Raw text          | 94.16  | 92.63  | 83.94 | —      | 74.45  | 71.05   | 84.67  | 67.15 | 59.85 | 38.10 | 46.79 |
| marathi-ufal-ud-2.17-251125                   | Gold tokenization | —      | —      | 88.59 | —      | 76.94  | 73.30   | 87.86  | 73.06 | 64.81 | 40.50 | 49.59 |
| middle_french-profiterole-ud-2.17-251125      | Raw text          | 100.00 | 77.90  | 98.13 | 98.41  | 99.32  | 97.72   | 95.96  | 90.22 | 87.71 | 82.15 | 79.74 |
| middle_french-profiterole-ud-2.17-251125      | Gold tokenization | —      | —      | 98.20 | 98.45  | 99.36  | 97.77   | 95.93  | 92.11 | 89.57 | 84.02 | 81.22 |
| naija-nsc-ud-2.17-251125                      | Raw text          | 99.97  | 100.00 | 98.00 | —      | 98.90  | 97.49   | 99.28  | 92.62 | 89.97 | 84.08 | 86.70 |
| naija-nsc-ud-2.17-251125                      | Gold tokenization | —      | —      | 98.02 | —      | 98.93  | 97.51   | 99.31  | 92.68 | 90.03 | 84.13 | 86.75 |
| nheengatu-complin-ud-2.17-251125              | Raw text          | 92.57  | 58.83  | 88.49 | 87.90  | 88.02  | 85.70   | 91.31  | 76.28 | 71.19 | 64.81 | 69.49 |
| nheengatu-complin-ud-2.17-251125              | Gold tokenization | —      | —      | 95.59 | 94.56  | 95.17  | 92.29   | 98.56  | 87.23 | 81.77 | 74.27 | 78.61 |
| north_sami-giella-ud-2.17-251125              | Raw text          | 99.87  | 98.79  | 91.83 | 93.50  | 89.47  | 85.48   | 86.94  | 76.20 | 71.26 | 60.71 | 58.83 |
| north_sami-giella-ud-2.17-251125              | Gold tokenization | —      | —      | 91.94 | 93.65  | 89.62  | 85.61   | 87.05  | 76.44 | 71.52 | 60.91 | 59.05 |
| norwegian-bokmaal-ud-2.17-251125              | Raw text          | 99.82  | 97.27  | 98.38 | 98.97  | 97.38  | 96.81   | 98.67  | 94.05 | 92.84 | 87.36 | 89.26 |
| norwegian-bokmaal-ud-2.17-251125              | Gold tokenization | —      | —      | 98.57 | 99.17  | 97.59  | 97.01   | 98.87  | 94.69 | 93.47 | 87.91 | 89.90 |
| norwegian-nynorsk-ud-2.17-251125              | Raw text          | 99.93  | 94.54  | 98.43 | 99.15  | 97.22  | 96.52   | 98.42  | 94.09 | 92.74 | 86.21 | 88.47 |
| norwegian-nynorsk-ud-2.17-251125              | Gold tokenization | —      | —      | 98.60 | 99.26  | 97.38  | 96.71   | 98.56  | 94.79 | 93.47 | 87.08 | 89.41 |
| occitan-ttb-ud-2.17-251125                    | Raw text          | 99.28  | 89.07  | 94.22 | 91.93  | 99.28  | 90.59   | 93.36  | 88.69 | 82.29 | 68.23 | 65.82 |
| occitan-ttb-ud-2.17-251125                    | Gold tokenization | —      | —      | 94.96 | 92.57  | 100.00 | 91.23   | 93.86  | 90.11 | 83.64 | 69.84 | 66.90 |
| old_church_slavonic-proiel-ud-2.17-251125     | Raw text          | 100.00 | 40.05  | 96.15 | 96.48  | 89.77  | 88.06   | 90.31  | 77.91 | 73.48 | 60.74 | 61.95 |
| old_church_slavonic-proiel-ud-2.17-251125     | Gold tokenization | —      | —      | 96.65 | 96.94  | 90.43  | 89.04   | 90.39  | 85.00 | 80.39 | 68.50 | 69.16 |
| old_east_slavic-torot-ud-2.17-251125          | Raw text          | 100.00 | 34.53  | 95.41 | 95.42  | 89.71  | 87.46   | 88.40  | 76.85 | 72.15 | 58.26 | 58.14 |
| old_east_slavic-torot-ud-2.17-251125          | Gold tokenization | —      | —      | 95.87 | 95.97  | 90.59  | 88.61   | 88.47  | 85.24 | 80.49 | 68.13 | 66.40 |
| old_east_slavic-birchbark-ud-2.17-251125      | Raw text          | 99.99  | 16.69  | 89.07 | 98.12  | 75.51  | 70.75   | 66.20  | 64.82 | 58.54 | 33.05 | 28.58 |
| old_east_slavic-birchbark-ud-2.17-251125      | Gold tokenization | —      | —      | 89.22 | 98.10  | 76.16  | 71.38   | 66.50  | 76.24 | 69.47 | 40.30 | 34.26 |
| old_east_slavic-rnc-ud-2.17-251125            | Raw text          | 99.77  | 94.56  | 97.77 | 91.36  | 90.13  | 82.43   | 90.60  | 80.58 | 77.10 | 58.83 | 60.58 |
| old_east_slavic-rnc-ud-2.17-251125            | Gold tokenization | —      | —      | 98.02 | 91.56  | 90.38  | 82.65   | 90.81  | 82.12 | 78.58 | 60.21 | 62.07 |
| old_east_slavic-ruthenian-ud-2.17-251125      | Raw text          | 99.81  | 99.23  | 96.30 | 89.55  | 88.43  | 81.51   | 83.77  | 80.04 | 76.42 | 56.10 | 51.60 |
| old_east_slavic-ruthenian-ud-2.17-251125      | Gold tokenization | —      | —      | 96.46 | 89.81  | 88.53  | 81.69   | 83.90  | 80.26 | 76.65 | 56.14 | 51.73 |
| old_french-profiterole-ud-2.17-251125         | Raw text          | 99.74  | 100.00 | 97.03 | 96.84  | —      | 96.37   | —      | 91.62 | 88.13 | 82.12 | 85.27 |
| old_french-profiterole-ud-2.17-251125         | Gold tokenization | —      | —      | 97.30 | 97.10  | —      | 96.63   | —      | 91.98 | 88.50 | 82.54 | 85.68 |
| old_occitan-corag-ud-2.17-251125              | Raw text          | 98.55  | 83.64  | 95.25 | —      | 95.37  | 93.27   | —      | 77.79 | 74.80 | 63.84 | 69.05 |
| old_occitan-corag-ud-2.17-251125              | Gold tokenization | —      | —      | 96.83 | —      | 97.02  | 95.02   | —      | 83.90 | 80.85 | 69.04 | 74.31 |
| ottoman_turkish-boun-ud-2.17-251125           | Raw text          | 99.44  | 87.39  | 85.77 | 90.42  | 80.51  | 70.91   | 81.99  | 63.60 | 52.37 | 32.91 | 37.18 |
| ottoman_turkish-boun-ud-2.17-251125           | Gold tokenization | —      | —      | 86.18 | 90.81  | 80.96  | 71.24   | 82.36  | 64.41 | 53.06 | 33.26 | 37.52 |
| persian-perdt-ud-2.17-251125                  | Raw text          | 99.66  | 99.83  | 97.50 | 97.35  | 97.60  | 95.58   | 98.94  | 93.71 | 91.55 | 86.45 | 88.94 |
| persian-perdt-ud-2.17-251125                  | Gold tokenization | —      | —      | 97.81 | 97.65  | 97.91  | 95.87   | 99.27  | 94.26 | 92.07 | 87.01 | 89.52 |
| persian-seraji-ud-2.17-251125                 | Raw text          | 99.65  | 99.25  | 97.97 | 97.93  | 97.95  | 97.49   | 98.57  | 92.00 | 89.17 | 84.70 | 85.39 |
| persian-seraji-ud-2.17-251125                 | Gold tokenization | —      | —      | 98.30 | 98.28  | 98.27  | 97.82   | 98.85  | 92.62 | 89.76 | 85.29 | 85.94 |
| polish-pdb-ud-2.17-251125                     | Raw text          | 99.86  | 97.00  | 99.00 | 96.04  | 96.13  | 95.46   | 98.30  | 94.64 | 92.76 | 86.26 | 89.29 |
| polish-pdb-ud-2.17-251125                     | Gold tokenization | —      | —      | 99.12 | 96.19  | 96.28  | 95.60   | 98.42  | 95.20 | 93.32 | 86.73 | 89.76 |
| polish-lfg-ud-2.17-251125                     | Raw text          | 99.85  | 99.65  | 99.05 | 95.99  | 96.63  | 95.34   | 98.43  | 97.14 | 95.75 | 90.22 | 92.81 |
| polish-lfg-ud-2.17-251125                     | Gold tokenization | —      | —      | 99.21 | 96.15  | 96.80  | 95.49   | 98.56  | 97.54 | 96.13 | 90.58 | 93.12 |
| polish-mpdt-ud-2.17-251125                    | Raw text          | 98.22  | 87.46  | 94.59 | 88.43  | 89.44  | 86.51   | 94.70  | 78.97 | 75.06 | 60.76 | 68.09 |
| polish-mpdt-ud-2.17-251125                    | Gold tokenization | —      | —      | 96.29 | 90.02  | 91.02  | 88.01   | 96.35  | 82.66 | 78.54 | 63.76 | 71.12 |
| pomak-philotis-ud-2.17-251125                 | Raw text          | 99.79  | 89.42  | 95.42 | —      | 88.98  | 87.97   | 91.42  | 88.35 | 81.64 | 64.46 | 67.38 |
| pomak-philotis-ud-2.17-251125                 | Gold tokenization | —      | —      | 95.52 | —      | 89.11  | 88.07   | 91.56  | 89.30 | 82.53 | 65.05 | 68.05 |
| portuguese-bosque-ud-2.17-251125              | Raw text          | 99.68  | 89.73  | 97.77 | —      | 96.89  | 95.82   | 98.33  | 92.16 | 89.84 | 80.52 | 84.37 |
| portuguese-bosque-ud-2.17-251125              | Gold tokenization | —      | —      | 98.09 | —      | 97.15  | 96.07   | 98.63  | 93.31 | 90.93 | 81.55 | 85.49 |
| portuguese-cintil-ud-2.17-251125              | Raw text          | 99.41  | 78.66  | 97.41 | 96.04  | 95.32  | 93.24   | 97.51  | 85.33 | 82.26 | 72.23 | 75.94 |
| portuguese-cintil-ud-2.17-251125              | Gold tokenization | —      | —      | 98.03 | 96.66  | 95.93  | 93.83   | 98.08  | 87.64 | 84.49 | 74.45 | 78.18 |
| portuguese-dantestocks-ud-2.17-251125         | Raw text          | 99.44  | 64.31  | 97.04 | 99.42  | 97.01  | 95.90   | 95.54  | 88.33 | 86.05 | 79.57 | 78.28 |
| portuguese-dantestocks-ud-2.17-251125         | Gold tokenization | —      | —      | 97.66 | 99.98  | 97.51  | 96.54   | 95.94  | 92.83 | 90.37 | 84.66 | 83.01 |
| portuguese-gsd-ud-2.17-251125                 | Raw text          | 99.29  | 86.25  | 97.58 | 90.09  | 94.87  | 89.66   | 97.35  | 92.87 | 90.93 | 80.72 | 85.55 |
| portuguese-gsd-ud-2.17-251125                 | Gold tokenization | —      | —      | 98.32 | 92.06  | 96.26  | 91.57   | 98.16  | 94.29 | 92.44 | 83.15 | 87.17 |
| portuguese-petrogold-ud-2.17-251125           | Raw text          | 99.59  | 93.11  | 98.74 | —      | 98.64  | 98.12   | 99.12  | 94.70 | 93.60 | 88.61 | 90.12 |
| portuguese-petrogold-ud-2.17-251125           | Gold tokenization | —      | —      | 99.05 | —      | 98.93  | 98.39   | 99.51  | 95.63 | 94.45 | 89.47 | 91.11 |
| portuguese-porttinari-ud-2.17-251125          | Raw text          | 99.86  | 99.82  | 99.05 | —      | 98.65  | 98.32   | 99.32  | 96.21 | 94.99 | 90.09 | 91.69 |
| portuguese-porttinari-ud-2.17-251125          | Gold tokenization | —      | —      | 99.18 | —      | 98.75  | 98.43   | 99.46  | 96.41 | 95.18 | 90.29 | 91.91 |
| romanian-rrt-ud-2.17-251125                   | Raw text          | 99.70  | 95.50  | 97.86 | 97.22  | 97.38  | 96.95   | 98.10  | 92.01 | 88.84 | 82.36 | 84.05 |
| romanian-rrt-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.14 | 97.48  | 97.64  | 97.19   | 98.36  | 92.77 | 89.53 | 82.89 | 84.58 |
| romanian-nonstandard-ud-2.17-251125           | Raw text          | 98.83  | 96.77  | 96.14 | 91.90  | 90.63  | 89.23   | 94.77  | 88.96 | 84.94 | 68.65 | 76.62 |
| romanian-nonstandard-ud-2.17-251125           | Gold tokenization | —      | —      | 97.28 | 92.92  | 91.62  | 90.19   | 95.84  | 90.72 | 86.64 | 70.17 | 77.90 |
| romanian-simonero-ud-2.17-251125              | Raw text          | 99.84  | 100.00 | 98.45 | 98.00  | 97.53  | 97.27   | 98.95  | 93.89 | 91.89 | 85.20 | 87.96 |
| romanian-simonero-ud-2.17-251125              | Gold tokenization | —      | —      | 98.61 | 98.15  | 97.68  | 97.40   | 99.11  | 94.22 | 92.21 | 85.45 | 88.24 |
| russian-syntagrus-ud-2.17-251125              | Raw text          | 99.67  | 98.16  | 98.74 | —      | 97.34  | 97.06   | 98.53  | 94.18 | 92.73 | 88.55 | 90.67 |
| russian-syntagrus-ud-2.17-251125              | Gold tokenization | —      | —      | 99.05 | —      | 97.63  | 97.36   | 98.81  | 94.91 | 93.43 | 89.12 | 91.22 |
| russian-gsd-ud-2.17-251125                    | Raw text          | 99.50  | 96.49  | 98.06 | 97.41  | 93.77  | 92.56   | 97.20  | 92.04 | 89.11 | 80.18 | 85.28 |
| russian-gsd-ud-2.17-251125                    | Gold tokenization | —      | —      | 98.54 | 97.87  | 94.18  | 92.95   | 97.58  | 93.25 | 90.19 | 81.00 | 86.13 |
| russian-poetry-ud-2.17-251125                 | Raw text          | 99.61  | 96.36  | 98.12 | —      | 95.74  | 95.37   | 97.54  | 89.53 | 87.25 | 79.60 | 82.33 |
| russian-poetry-ud-2.17-251125                 | Gold tokenization | —      | —      | 98.55 | —      | 96.07  | 95.72   | 97.93  | 90.58 | 88.28 | 80.46 | 83.32 |
| russian-taiga-ud-2.17-251125                  | Raw text          | 99.29  | 88.78  | 97.14 | —      | 95.18  | 94.46   | 96.97  | 88.33 | 85.60 | 78.62 | 81.40 |
| russian-taiga-ud-2.17-251125                  | Gold tokenization | —      | —      | 97.78 | —      | 95.84  | 95.07   | 97.60  | 89.78 | 86.89 | 79.71 | 82.64 |
| sanskrit-vedic-ud-2.17-251125                 | Raw text          | 100.00 | 29.21  | 93.56 | —      | 89.24  | 85.37   | 93.45  | 65.47 | 56.91 | 49.07 | 52.26 |
| sanskrit-vedic-ud-2.17-251125                 | Gold tokenization | —      | —      | 93.83 | —      | 90.36  | 86.66   | 93.50  | 77.99 | 68.85 | 60.40 | 64.20 |
| scottish_gaelic-arcosg-ud-2.17-251125         | Raw text          | 97.48  | 61.55  | 93.87 | 89.17  | 90.71  | 87.70   | 95.19  | 81.04 | 76.86 | 65.33 | 70.42 |
| scottish_gaelic-arcosg-ud-2.17-251125         | Gold tokenization | —      | —      | 96.65 | 92.03  | 93.63  | 90.74   | 97.70  | 87.26 | 83.15 | 72.21 | 76.90 |
| serbian-set-ud-2.17-251125                    | Raw text          | 99.99  | 93.00  | 99.06 | 95.99  | 96.16  | 95.73   | 97.96  | 93.40 | 91.07 | 83.60 | 87.12 |
| serbian-set-ud-2.17-251125                    | Gold tokenization | —      | —      | 99.06 | 96.02  | 96.19  | 95.76   | 97.95  | 94.12 | 91.77 | 84.29 | 87.82 |
| shanghainese-shud-ud-2.17-251125              | Raw text          | 87.12  | 90.82  | 79.32 | 78.98  | —      | 76.38   | 87.12  | 58.08 | 53.11 | 40.72 | 46.39 |
| shanghainese-shud-ud-2.17-251125              | Gold tokenization | —      | —      | 88.56 | 89.13  | —      | 85.81   | 100.00 | 77.23 | 69.11 | 57.82 | 64.15 |
| sindhi-isra-ud-2.17-251125                    | Raw text          | 99.95  | 88.28  | 93.27 | 91.83  | 71.01  | 68.89   | 94.79  | 83.73 | 74.44 | 39.66 | 63.62 |
| sindhi-isra-ud-2.17-251125                    | Gold tokenization | —      | —      | 93.41 | 91.97  | 71.23  | 69.11   | 94.88  | 85.51 | 76.42 | 41.22 | 65.54 |
| slovak-snk-ud-2.17-251125                     | Raw text          | 100.00 | 81.69  | 97.65 | 89.96  | 93.14  | 88.96   | 96.59  | 91.41 | 89.66 | 79.53 | 84.60 |
| slovak-snk-ud-2.17-251125                     | Gold tokenization | —      | —      | 97.83 | 90.18  | 93.31  | 89.19   | 96.66  | 93.75 | 91.91 | 81.76 | 86.90 |
| slovenian-ssj-ud-2.17-251125                  | Raw text          | 99.94  | 98.95  | 98.85 | 97.04  | 97.10  | 96.60   | 98.69  | 94.47 | 92.92 | 87.33 | 89.45 |
| slovenian-ssj-ud-2.17-251125                  | Gold tokenization | —      | —      | 98.91 | 97.10  | 97.16  | 96.66   | 98.73  | 94.65 | 93.10 | 87.50 | 89.61 |
| slovenian-sst-ud-2.17-251125                  | Raw text          | 99.93  | 96.40  | 98.60 | 96.89  | 97.06  | 96.30   | 99.04  | 86.03 | 83.20 | 75.28 | 78.59 |
| slovenian-sst-ud-2.17-251125                  | Gold tokenization | —      | —      | 98.67 | 96.95  | 97.12  | 96.36   | 99.11  | 86.34 | 83.54 | 75.61 | 78.90 |
| southern_kurdish-garrusi-ud-2.17-251125       | Raw text          | 90.25  | 92.68  | 77.26 | —      | 72.92  | 64.26   | 67.15  | 68.59 | 62.09 | 40.76 | 36.02 |
| southern_kurdish-garrusi-ud-2.17-251125       | Gold tokenization | —      | —      | 83.82 | —      | 78.68  | 68.38   | 75.00  | 76.47 | 68.38 | 45.63 | 42.72 |
| spanish-ancora-ud-2.17-251125                 | Raw text          | 99.95  | 98.69  | 99.05 | 96.08  | 98.74  | 95.61   | 99.37  | 93.81 | 92.07 | 86.86 | 88.38 |
| spanish-ancora-ud-2.17-251125                 | Gold tokenization | —      | —      | 99.09 | 96.12  | 98.79  | 95.64   | 99.41  | 94.01 | 92.26 | 87.03 | 88.53 |
| spanish-gsd-ud-2.17-251125                    | Raw text          | 99.73  | 95.26  | 97.10 | —      | 96.90  | 95.20   | 98.62  | 92.36 | 90.26 | 78.77 | 84.35 |
| spanish-gsd-ud-2.17-251125                    | Gold tokenization | —      | —      | 97.36 | —      | 97.18  | 95.46   | 98.86  | 93.15 | 90.98 | 79.45 | 85.07 |
| swedish-talbanken-ud-2.17-251125              | Raw text          | 99.87  | 95.53  | 98.67 | 97.34  | 97.24  | 96.29   | 98.53  | 92.89 | 90.52 | 84.70 | 87.28 |
| swedish-talbanken-ud-2.17-251125              | Gold tokenization | —      | —      | 98.79 | 97.44  | 97.36  | 96.41   | 98.66  | 93.39 | 91.02 | 85.23 | 87.86 |
| swedish-lines-ud-2.17-251125                  | Raw text          | 99.95  | 87.70  | 98.03 | 95.34  | 93.06  | 90.18   | 97.90  | 91.16 | 88.14 | 75.91 | 83.31 |
| swedish-lines-ud-2.17-251125                  | Gold tokenization | —      | —      | 98.09 | 95.41  | 93.06  | 90.20   | 97.95  | 92.14 | 89.07 | 76.65 | 84.15 |
| tamil-ttb-ud-2.17-251125                      | Raw text          | 94.26  | 97.52  | 83.94 | 82.57  | 84.54  | 77.97   | 89.50  | 70.38 | 62.08 | 51.33 | 55.49 |
| tamil-ttb-ud-2.17-251125                      | Gold tokenization | —      | —      | 88.94 | 86.98  | 89.64  | 82.15   | 94.32  | 77.83 | 68.63 | 56.86 | 61.06 |
| telugu-mtg-ud-2.17-251125                     | Raw text          | 99.58  | 96.62  | 93.77 | 93.77  | 98.48  | 93.63   | —      | 90.44 | 84.35 | 77.03 | 80.65 |
| telugu-mtg-ud-2.17-251125                     | Gold tokenization | —      | —      | 94.17 | 94.17  | 98.89  | 94.04   | —      | 91.26 | 85.16 | 77.86 | 81.49 |
| thai-tud-ud-2.17-251125                       | Raw text          | 87.14  | 11.36  | 77.29 | —      | 87.14  | 77.29   | —      | 58.97 | 50.87 | 41.01 | 45.49 |
| thai-tud-ud-2.17-251125                       | Gold tokenization | —      | —      | 89.39 | —      | 100.00 | 89.39   | —      | 81.27 | 71.00 | 61.35 | 67.25 |
| turkish-boun-ud-2.17-251125                   | Raw text          | 96.57  | 86.25  | 90.01 | 86.01  | 81.08  | 71.65   | 90.59  | 73.16 | 67.27 | 49.26 | 61.34 |
| turkish-boun-ud-2.17-251125                   | Gold tokenization | —      | —      | 93.14 | 89.02  | 83.03  | 73.41   | 93.62  | 80.54 | 74.00 | 52.95 | 67.05 |
| turkish-atis-ud-2.17-251125                   | Raw text          | 99.90  | 79.28  | 98.28 | —      | 97.97  | 97.70   | 98.92  | 89.45 | 87.75 | 84.98 | 86.33 |
| turkish-atis-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.48 | —      | 98.13  | 97.90   | 99.02  | 91.71 | 89.91 | 87.25 | 88.52 |
| turkish-framenet-ud-2.17-251125               | Raw text          | 99.90  | 99.27  | 96.70 | —      | 94.65  | 93.97   | 96.42  | 93.49 | 84.36 | 74.70 | 77.72 |
| turkish-framenet-ud-2.17-251125               | Gold tokenization | —      | —      | 96.86 | —      | 94.75  | 94.14   | 96.52  | 93.66 | 84.53 | 74.83 | 77.84 |
| turkish-imst-ud-2.17-251125                   | Raw text          | 97.31  | 97.38  | 92.72 | 92.73  | 89.74  | 87.01   | 93.52  | 76.51 | 69.18 | 58.01 | 63.60 |
| turkish-imst-ud-2.17-251125                   | Gold tokenization | —      | —      | 95.22 | 95.12  | 91.96  | 89.05   | 95.83  | 81.39 | 73.61 | 60.77 | 66.82 |
| turkish-kenet-ud-2.17-251125                  | Raw text          | 100.00 | 98.12  | 93.68 | —      | 91.94  | 90.76   | 93.61  | 84.15 | 71.52 | 62.12 | 65.44 |
| turkish-kenet-ud-2.17-251125                  | Gold tokenization | —      | —      | 93.70 | —      | 91.96  | 90.78   | 93.62  | 84.28 | 71.62 | 62.24 | 65.57 |
| turkish-penn-ud-2.17-251125                   | Raw text          | 99.27  | 82.89  | 95.56 | —      | 94.40  | 93.35   | 94.10  | 84.58 | 71.91 | 62.43 | 64.64 |
| turkish-penn-ud-2.17-251125                   | Gold tokenization | —      | —      | 96.31 | —      | 95.15  | 94.12   | 94.79  | 86.85 | 73.90 | 63.73 | 65.90 |
| turkish-tourism-ud-2.17-251125                | Raw text          | 99.99  | 100.00 | 98.67 | —      | 94.88  | 94.45   | 98.36  | 97.29 | 91.67 | 81.69 | 87.40 |
| turkish-tourism-ud-2.17-251125                | Gold tokenization | —      | —      | 98.68 | —      | 94.89  | 94.47   | 98.38  | 97.31 | 91.69 | 81.71 | 87.42 |
| turkish_german-sagt-ud-2.17-251125            | Raw text          | 98.91  | 99.44  | 90.24 | —      | 80.31  | 75.43   | 90.79  | 71.11 | 60.81 | 40.99 | 50.44 |
| turkish_german-sagt-ud-2.17-251125            | Gold tokenization | —      | —      | 91.12 | —      | 80.93  | 75.94   | 91.49  | 72.73 | 62.03 | 41.63 | 51.35 |
| ukrainian-iu-ud-2.17-251125                   | Raw text          | 99.81  | 96.23  | 97.86 | 94.56  | 94.64  | 93.39   | 97.54  | 90.76 | 88.36 | 79.20 | 83.48 |
| ukrainian-iu-ud-2.17-251125                   | Gold tokenization | —      | —      | 98.08 | 94.73  | 94.82  | 93.58   | 97.75  | 91.35 | 88.95 | 79.57 | 83.93 |
| ukrainian-parlamint-ud-2.17-251125            | Raw text          | 99.95  | 99.81  | 98.56 | 98.47  | 95.01  | 93.81   | 99.07  | 93.95 | 91.91 | 82.96 | 88.90 |
| ukrainian-parlamint-ud-2.17-251125            | Gold tokenization | —      | —      | 98.60 | 98.49  | 95.02  | 93.81   | 99.12  | 93.96 | 91.92 | 82.96 | 88.91 |
| urdu-udtb-ud-2.17-251125                      | Raw text          | 100.00 | 98.31  | 94.13 | 92.34  | 82.74  | 78.50   | 97.35  | 87.94 | 82.55 | 56.75 | 74.76 |
| urdu-udtb-ud-2.17-251125                      | Gold tokenization | —      | —      | 94.13 | 92.34  | 82.76  | 78.53   | 97.35  | 87.98 | 82.63 | 56.84 | 74.83 |
| uyghur-udt-ud-2.17-251125                     | Raw text          | 99.54  | 81.87  | 89.87 | 91.73  | 88.03  | 80.73   | 94.64  | 75.68 | 64.94 | 50.28 | 57.55 |
| uyghur-udt-ud-2.17-251125                     | Gold tokenization | —      | —      | 90.26 | 92.33  | 88.51  | 81.17   | 95.15  | 77.54 | 66.62 | 51.26 | 58.87 |
| uzbek-uzudt-ud-2.17-251125                    | Raw text          | 100.00 | 72.26  | 86.15 | 85.25  | 70.68  | 60.26   | 29.27  | 71.16 | 55.78 | 28.87 | 11.22 |
| uzbek-uzudt-ud-2.17-251125                    | Gold tokenization | —      | —      | 86.24 | 85.58  | 70.49  | 60.21   | 29.22  | 73.63 | 57.97 | 30.19 | 11.42 |
| vietnamese-vtb-ud-2.17-251125                 | Raw text          | 86.06  | 93.73  | 78.40 | 77.39  | —      | 77.26   | 85.77  | 56.34 | 49.57 | 40.87 | 45.29 |
| vietnamese-vtb-ud-2.17-251125                 | Gold tokenization | —      | —      | 89.92 | 88.74  | —      | 88.59   | 99.47  | 75.84 | 65.51 | 55.15 | 61.06 |
| welsh-ccg-ud-2.17-251125                      | Raw text          | 99.54  | 97.47  | 95.50 | 94.56  | 89.93  | 87.67   | 94.89  | 87.52 | 81.76 | 64.32 | 71.52 |
| welsh-ccg-ud-2.17-251125                      | Gold tokenization | —      | —      | 95.89 | 94.97  | 90.30  | 88.02   | 95.32  | 88.45 | 82.61 | 65.14 | 72.40 |
| western_armenian-armtdp-ud-2.17-251125        | Raw text          | 99.87  | 98.89  | 96.81 | —      | 92.37  | 91.67   | 96.89  | 89.35 | 84.85 | 70.21 | 76.47 |
| western_armenian-armtdp-ud-2.17-251125        | Gold tokenization | —      | —      | 96.90 | —      | 92.47  | 91.75   | 97.01  | 89.65 | 85.14 | 70.46 | 76.77 |
| wolof-wtb-ud-2.17-251125                      | Raw text          | 99.23  | 91.95  | 94.32 | 94.24  | 93.47  | 91.44   | 95.18  | 83.49 | 78.02 | 66.05 | 69.42 |
| wolof-wtb-ud-2.17-251125                      | Gold tokenization | —      | —      | 95.29 | 95.14  | 94.33  | 92.39   | 95.94  | 85.75 | 80.16 | 68.29 | 71.41 |
| yiddish-yitb-ud-2.17-251125                   | Raw text          | 99.61  | 97.73  | 94.50 | —      | 99.61  | 94.50   | 93.53  | 89.38 | 85.56 | 75.85 | 73.70 |
| yiddish-yitb-ud-2.17-251125                   | Gold tokenization | —      | —      | 94.85 | —      | 100.00 | 94.85   | 93.87  | 90.34 | 86.48 | 76.74 | 74.31 |
| zaar-autogramm-ud-2.17-251125                 | Raw text          | 94.06  | 98.93  | 84.94 | 92.38  | 85.78  | 79.25   | 82.47  | 70.99 | 62.81 | 43.14 | 44.13 |
| zaar-autogramm-ud-2.17-251125                 | Gold tokenization | —      | —      | 90.67 | 98.45  | 91.37  | 84.59   | 87.42  | 83.04 | 73.34 | 53.71 | 54.34 |
