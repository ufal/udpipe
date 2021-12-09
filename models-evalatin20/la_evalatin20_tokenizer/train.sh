#!/bin/bash

set -e

lang="$1"; shift
id="$1"; shift
args="$1"; shift

mkdir -p $lang
exec >$lang/$id.log; exec 2>&1
echo "$args" >$lang/$id.cmd

./udpipe --train --tokenizer="$args" --tagger=none --parser=none --heldout=../ud-2.6/$lang/$lang-dev.conllu $lang/$id.model ../ud-2.6/$lang/$lang-train.conllu
./udpipe --accuracy --tokenize "$lang/$id.model" <../ud-2.6/$lang/$lang-dev.conllu >"$lang/$id.dev"
./udpipe --accuracy --tokenize "$lang/$id.model" <../ud-2.6/$lang/$lang-test.conllu >"$lang/$id.test"

echo Done
