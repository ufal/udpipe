#!/bin/sh

[ $# -lt 1 ] && { echo Usage: $0 language_code >&2; exit 1; }

./udpipe --output=horizontal none ../ud-2.5/"$1"/*train.conllu | ./normalize_form >"$1".in
./word2vec -train "$1".in -output "$1".skip.forms.50.vectors -cbow 0 -size 50 -window 10 -negative 5 -hs 0 -sample 1e-1 -threads 1 -binary 0 -iter 15 -min-count 2
rm "$1".in
