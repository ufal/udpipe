#!/bin/sh

wget http://ufal.mff.cuni.cz/~zeman/soubory/ud-treebanks-v2.3.tgz
tar xf ud-treebanks-v2.3.tgz
rm ud-treebanks-v2.3.tgz
for tb in ud-treebanks-v2.3/*/*-ud-train.conllu; do
  dir=`dirname $tb`
  long_name=`basename $dir`
  code=`basename $tb`
  code=${code%%-*}

  case $long_name in
    UD_Japanese-BCCWJ|UD_Hindi_English-HIENCS|UD_English-ESL|UD_Arabic-NYUAD|UD_French-FTB) echo Skipping treebank $long_name without forms; continue;;
  esac

  mkdir -p "$code"
  cp "$dir"/* "$code"
  if [ -f "$dir"/"$code"-ud-dev.conllu ]; then
    cp "$dir"/"$code"-ud-train.conllu "$dir"/"$code"-ud-dev.conllu "$code"
  else
    perl conllu_split.pl "$code" "$code" <"$dir"/"$code"-ud-train.conllu
  fi

  train_words=$(grep -c '^[0-9]' $code/*-ud-train.conllu)
  [ "$train_words" -lt 1000 ] && { echo Skipping treebank $code with $train_words training words. >&2; rm -r "$code/"; continue; }
  echo $code $long_name | tee -a iso_names.txt

  for conllu in "$code"/*.conllu; do
    perl conllu_to_text.pl --language="$code" <"$conllu" >"${conllu%.conllu}.txt"
  done
done
rm -r ud-treebanks-v2.3
