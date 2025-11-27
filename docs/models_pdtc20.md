## Czech PDT-C 2.0 Model #czech_pdtc2.0_model

PDT-C 2.0 Model is distributed under the
[CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) licence.
The model is trained on [PDT-C 2.0 treebank](http://hdl.handle.net/11234/1-5813)
using [RobeCzech model](https://hdl.handle.net/11234/1-3691), and performs
morphological analysis using the [MorfFlex CZ 2.1](http://hdl.handle.net/11234/1-5985)
morphological dictionary via [MorphoDiTa](https://ufal.mff.cuni.cz/morphodita).

The model requires [UDPipe 2.1](https://ufal.mff.cuni.cz/udpipe/2), together
with Python packages [ufal.udpipe](https://pypi.org/project/ufal.udpipe/)
version at least 1.3.1.1 and [ufal.morphodita](https://pypi.org/project/ufal.morphodita/)
version at least 1.11.2.1.

### Download

The latest version 251025 of the Czech PDT-C 2.0 model
can be downloaded from the [LINDAT/CLARIN repository](http://hdl.handle.net/11234/1-6045).

The model is also available in the [REST service](https://lindat.mff.cuni.cz/services/udpipe/).

### PDT-C 2.0 Morphological System

PDT-C 2.0 uses the _PDT-C tag set_ from MorfFlex CZ 2.0, which is an evolution
of the original _PDT tag set_ devised by Jan Hajič
([Hajič, 2004](https://books.google.cz/books?id=sB63AAAACAAJ)).
The tags are positional with 15 positions corresponding to part of speech,
detailed part of speech, gender, number, case, etc. (e.g. `NNFS1-----A----`).
Different meanings of same lemmas are distinguished and additional comments can
be provided for every lemma meaning. The complete reference can be found in the
[Manual for Morphological Annotation, Revision for the Prague Dependency
Treebank - Consolidated 2020 release](https://ufal.mff.cuni.cz/techrep/tr64.pdf)
and quick reference is available in the [PDT-C positional morphological tags
overview](https://ufal.mff.cuni.cz/pdt-c/publications/Appendix_M_Tags_2020.pdf).

The PDT-C 2.0 employ dependency relations from the [PDT analytical
level](https://ufal.mff.cuni.cz/pdt-c/publications/PDT20-a-man-en.pdf), with
a quick reference available in the [PDT-C analytical functions and clause
segmentation overview](http://ufal.mff.cuni.cz/pdt-c/publications/Appendix_A_Tags_2020.pdf).

In the CoNLL-U format, the
- tags are filled in the `XPOS` column, and
- the dependency relations are filled in the `DEPREL`, even if they are
  different from the universal dependency relations.

### PDT-C 2.0 Train/Dev/Test Split

The PDT-C 2.0 corpus contains four datasets, each with an official train/dev/test split.

Note that this is different to PDT-C 1.0, where some datasets do not have an
official split. The PDT-C 2.0 official splits are nearly identical to the ones
used in UDPipe PDT-C 1.0 models, with the exception of PCEDT, which is
a translated version of the Wall Street Journal. Unfortunately, WSJ has several
splits; PDT-C 2.0 uses sections 02-21 as train, sections 00, 01, 22, 24 as dev,
and section 23 as test, while UDPipe PDT-C 1.0 models use sections 0-18 as
train, sections 19-21 as dev, and sections 22-24 as test.

Furthermore, the PDT dataset has three subsets: all data have morphological
annotation, smaller subset has morphological and analytical annotation, and
smallest subset has morphological, analytical, and tectogrammatical annotation;
we utilize the subset with morphological and analytical annotation (while in
UDPipe PDT-C 1.0 model, the full PDT with morphological annotation was used for
training and evaluation of tagging and lemmatization).

### Acknowledgements #czech_pdtc2.0_model_acknowledgements

This work has been supported by the LINDAT/CLARIAH-CZ project funded by Ministry
of Education, Youth and Sports of the Czech Republic (project LM2023062).

### Publications

- Milan Straka, Jakub Náplava, Jana Straková, David Samuel (2020): [RobeCzech: Czech RoBERTa, a monolingual contextualized language representation model](https://doi.org/10.1007/978-3-030-83527-9_17). In: Text, Speech, and Dialogue. TSD 2021. Lecture Notes in Computer Science, vol 12848. Springer, Cham.
- Jan Hajič, Eduard Bejček, Jaroslava Hlavacova, Marie Mikulová, Milan Straka, Jan Štěpánek, and Barbora Štěpánková (2020). [Prague Dependency Treebank - Consolidated 1.0](https://aclanthology.org/2020.lrec-1.641.pdf). In: Proceedings of the 12th Language Resources and Evaluation Conference, pages 5208–5218, Marseille, France. European Language Resources Association.
- Milan Straka (2018): [UDPipe 2.0 Prototype at CoNLL 2018 UD Shared Task](https://www.aclweb.org/anthology/K18-2020/). In: Proceedings of CoNLL 2018: The SIGNLL Conference on Computational Natural Language Learning, pp. 197-207, Association for Computational Linguistics, Stroudsburg, PA, USA, ISBN 978-1-948087-72-8
- Straková Jana, Straka Milan and Hajič Jan (2014): [Open-Source Tools for Morphology, Lemmatization, POS Tagging and Named Entity Recognition](https://aclanthology.org/P14-5003/). In: Proceedings of 52nd Annual Meeting of the Association for Computational Linguistics: System Demonstrations, pages 13-18, Baltimore, Maryland, June 2014. Association for Computational Linguistics.

### Model Performance

We evaluate tagging, lemmatization, and dependency parsing on the four datasets
of PDT-C 2.0, and we also compute a macro-average. For lemmatization, we use the
following metrics:
- `Lemmas`: a primary metric comparing the _lemma proper_, which is the lemma
  with an optional lemma number (but we ignore the additional lemma comments
  like “this is a given name”);
- `LemmasEM`: an exact match comparing also the lemma comments. This metric is
  less or equal to `Lemmas`. Our model directly predicts only lemma proper (no
  additional comments), and relies on the morphological dictionary to supply the
  comments, so it fails to generate comments for unknown words (like an unknown
  given name).

We perform the evaluation using the
[udpipe2_eval.py](https://github.com/ufal/udpipe/blob/udpipe-2/udpipe2_eval.py),
which is a minor extension of the [CoNLL 2018 Shared
Task](https://universaldependencies.org/conll18/evaluation.html) evaluation
script.

Because the model also include a rule-based tokenizer and sentence splitter,
we evaluate both:
- using raw input text, which must first be tokenized and split into sentences.
  The resulting scores are in fact F1-scores. Note that the FAUST dataset does
  not contain any discernible sentence boundaries.
- using gold tokenization.

| Treebank | Mode              | Tokens | Sents | XPOS  | Lemma | LemmaEM | UAS   | LAS   |
|:---------|:------------------|-------:|------:|------:|------:|--------:|------:|------:|
| PDT      | Raw text          | 99.94  | 88.52 | 98.38 | 98.80 | 98.62   | 94.02 | 91.53 |
| PDT      | Gold tokenization | —      | —     | 98.45 | 98.86 | 98.67   | 95.25 | 92.81 |
| PCEDT    | Raw text          | 99.99  | 93.36 | 99.05 | 99.53 | 98.88   | 95.97 | 94.39 |
| PCEDT    | Gold tokenization | —      | —     | 99.06 | 99.55 | 98.90   | 96.52 | 94.97 |
| PDTSC    | Raw text          | 100.00 | 98.31 | 98.96 | 99.35 | 99.29   | 94.82 | 92.18 |
| PDTSC    | Gold tokenization | —      | —     | 98.96 | 99.35 | 99.29   | 94.96 | 92.33 |
| FAUST    | Raw text          | 100.00 | 10.98 | 97.37 | 99.10 | 98.62   | 84.31 | 80.98 |
| FAUST    | Gold tokenization | —      | —     | 97.82 | 99.15 | 98.70   | 91.44 | 88.40 |
| MacroAvg | Gold tokenization | —      | —     | 98.57 | 99.23 | 98.89   | 94.54 | 92.13 |
