# UDPipe
[![Build Status](https://travis-ci.org/ufal/udpipe.svg?branch=master)](https://travis-ci.org/ufal/udpipe)

UDPipe is an trainable pipeline for tokenization, tagging, lemmatization and
parsing of CoNLL-U files. UDPipe is language-agnostic and can be trained given
only annotated data in CoNLL-U format. (Nevertheless, to train the tokenizer,
either the `SpaceAfter` feature must be present, or at least some plain text
must be available; also morphological analyzer and lemmatizer can be improved
if morphological dictionary is provided.) Trained models are provided for
nearly all UD treebanks.  UDPipe is available as a binary, as a library for
C++, Python, Perl, Java, C#, and as a web service.

UDPipe is a free software under
[Mozilla Public License 2.0](http://www.mozilla.org/MPL/2.0/) and the linguistic models
are free for non-commercial use and distributed under
[CC BY-NC-SA](http://creativecommons.org/licenses/by-nc-sa/4.0/) license, although for some
models the original data used to create the model may impose additional
licensing conditions. UDPipe is versioned using [Semantic Versioning](http://semver.org/).

Copyright 2016 by Institute of Formal and Applied Linguistics, Faculty of
Mathematics and Physics, Charles University in Prague, Czech Republic.

UDPipe website http://ufal.mff.cuni.cz/udpipe contains download links
of both the released packages and trained models, hosts documentation and
offers online demo.

UDPipe development repository http://github.com/ufal/udpipe is hosted
on GitHub.
