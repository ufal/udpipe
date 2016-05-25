#!/bin/sh

[ $# -lt 1 ] && { echo Usage: $0 language_code >&2; exit 1; }

cat ../../ud-1.2/"$1"/*train*.conllu | grep -v -e "^#" -e "^[0-9]*-" | cut -f2 | perl -e '$w=""; while(<>) {chomp; if (length $_) {$w .= (length $w ? " " : "") . $_;} else {print "$w\n"; $w="";}} print "$w\n" if length $w' > "$1".in
./word2vec -train "$1".in -output "$1".skip.forms.50.vectors -cbow 0 -size 50 -window 10 -negative 5 -hs 0 -sample 1e-1 -threads 12 -binary 0 -iter 15 -min-count 2
#./word2vec -train "$1".in -output "$1".cbow.forms.50.vectors -cbow 1 -size 50 -window 8 -negative 0 -hs 1 -sample 1e-1 -threads 12 -binary 0 -iter 15 -min-count 2
rm "$1".in
