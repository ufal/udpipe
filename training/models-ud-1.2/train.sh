#!/bin/bash

l="$1"; shift

mkdir -p $l
./udpipe --train --tokenizer=`grep "^$l " params_tokenizer | sed "s/^$l //"` --tagger=`grep "^$l " params_tagger | sed "s/^$l //"` --parser=`grep "^$l " params_parser | sed "s/^$l //"` --heldout=../ud-1.2/$l/$l-ud-dev.conllu $l/$l.model ../ud-1.2/$l/$l-ud-train*.conllu
./udpipe --accuracy --tokenize --tag --parse "$l/$l.model" <../ud-1.2/$l/$l-ud-dev.conllu >"$l/$l.dev"
./udpipe --accuracy --tokenize --tag --parse "$l/$l.model" <../ud-1.2/$l/$l-ud-test.conllu >"$l/$l.test"
