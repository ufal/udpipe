#!/bin/sh
# 
# wget https://lindat.mff.cuni.cz/repository/xmlui/bitstream/handle/11234/1-5787/ud-treebanks-v2.15.tgz
# tar xf ud-treebanks-v2.15.tgz
# rm ud-treebanks-v2.15.tgz
for tb in ud-treebanks-v2.15/*/*-ud-train.conllu; do
  dir=`dirname $tb`
  long_name=`basename $dir`
  code=`basename $tb`
  code=${code%%-*}

  case $long_name in
    UD_Arabic-NYUAD|UD_English-GUMReddit|UD_Japanese-BCCWJ|UD_Japanese-BCCWJLUW) echo Skipping treebank $long_name without forms; continue;;
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
    sed "1i# variant = $code" -i "$conllu"
    case "$conllu" in
      *train.conllu) rm "${conllu%.conllu}.txt";;
      *) perl conllu_to_text.pl --language="$code" <"$conllu" >"${conllu%.conllu}.txt";;
    esac
  done
done
rm -r ud-treebanks-v2.15

for code in $(for d in */; do echo ${d%%[_/]*}; done | sort | uniq -d); do
  mkdir "${code}_all"
  cat "$code"*/*train.conllu >"${code}_all/${code}_all-ud-train.conllu"
done
