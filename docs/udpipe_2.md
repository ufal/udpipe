# UDPipe 2

UDPipe 2 is a Python prototype, capable of performing tagging, lemmatization and
syntactic analysis of CoNLL-U input. It took part in several competitions,
reaching excellent results in all of them:
- one of the winners of the [CoNLL 2018 Shared Task](https://www.aclweb.org/anthology/K18-2001/)
- one of the winners of the [SIGMORPHON 2019 Shared Task 2](https://www.aclweb.org/anthology/W19-4226/)
- the winner of [EvaLatin 2020](https://www.aclweb.org/anthology/2020.lt4hala-1.16/)

Compared to UDPipe 1, it is Python-only, it does not perform tokenization, and
the models require more computation power.

## REST Service

The UDPipe 2 models are currently available from the
[LINDAT UDPipe REST Service](https://lindat.mff.cuni.cz/services/udpipe/).
Apart from the web interface, you can use the
[udpipe2_client.py](https://github.com/ufal/udpipe/blob/udpipe-2/udpipe2_client.py)
script to process your files through the service.

## Getting UDPipe 2

You can get UDPipe 2 sources from the
[udpipe-2 branch of the UDPipe repository](https://github.com/ufal/udpipe/tree/udpipe-2).
The sources can be used to both train a new model and to run a local REST server
for inference.

## Models

The available models are described [on a separate page](/udpipe/2/models).

<h2 id="udpipe2_acknowledgements">Acknowledgements</h2>

This work has been supported by the Ministry of Education, Youth and Sports of
the Czech Republic, Project No. LM2018101 LINDAT/CLARIAH-CZ.

## Publications

- Milan Straka (2018): [UDPipe 2.0 Prototype at CoNLL 2018 UD Shared Task](https://www.aclweb.org/anthology/K18-2020/). In: Proceedings of CoNLL 2018: The SIGNLL Conference on Computational Natural Language Learning, pp. 197-207, Association for Computational Linguistics, Stroudsburg, PA, USA, ISBN 978-1-948087-72-8
- Milan Straka, Jana Straková, Jan Hajič (2019): [UDPipe at SIGMORPHON 2019: Contextualized Embeddings, Regularization with Morphological Categories, Corpora Merging](https://www.aclweb.org/anthology/W19-4212/). In: Proceedings of the 16th SIGMORPHON Workshop on Computational Research in Phonetics, Phonology, and Morphology, pp. 95-103, Association for Computational Linguistics, Stroudsburg, PA, USA, ISBN 978-1-950737-36-9
- Milan Straka, Jana Straková, Jan Hajič (2019): [Evaluating Contextualized Embeddings on 54 Languages in POS Tagging, Lemmatization and Dependency Parsing](https://arxiv.org/abs/1908.07448). In: ArXiv.org Computing Research Repository, ISSN 2331-8422, 1904.02099

## Bibtex for Referencing

```
@InProceedings{straka-2018-udpipe,
    title = "{UDP}ipe 2.0 Prototype at {C}o{NLL} 2018 {UD} Shared Task",
    author = "Straka, Milan",
    booktitle = "Proceedings of the {C}o{NLL} 2018 Shared Task: Multilingual Parsing from Raw Text to Universal Dependencies",
    month = oct,
    year = "2018",
    address = "Brussels, Belgium",
    publisher = "Association for Computational Linguistics",
    url = "https://www.aclweb.org/anthology/K18-2020",
    doi = "10.18653/v1/K18-2020",
    pages = "197--207",
}
```
