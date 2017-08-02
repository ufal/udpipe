#!/bin/sh

wget --content-disposition 'https://lindat.mff.cuni.cz/repository/xmlui/bitstream/handle/11234/1-1983/ud-treebanks-v2.0.tgz?sequence=1&isAllowed=y'
tar xf ud-treebanks-v2.0.tgz
rm ud-treebanks-v2.0.tgz
for tb in ud-treebanks-v2.0/*/*-ud-dev.conllu; do
  dir=`dirname $tb`
  long_name=`basename $dir`
  code=`basename $tb`
  code=${code%%-*}
  echo $code $long_name >>iso_names.txt

  mv $dir $code
  rm $code/$code-ud-*.txt
  perl conllu_resplit.pl $code/$code-ud-train.conllu $code/$code-ud-dev.conllu
done
rmdir ud-treebanks-v2.0

sed 's/^9\.2/9.1/; s/9\.2:nmod/9.1:nmod/' -i ru_syntagrus/ru_syntagrus-ud-dev.conllu
rm -rf ar_nyuad/ it_partut/

wget --content-disposition 'https://lindat.mff.cuni.cz/repository/xmlui/bitstream/handle/11234/1-2184/ud-test-v2.0-conll2017.tgz?sequence=3&isAllowed=y'
tar xf ud-test-v2.0-conll2017.tgz
rm ud-test-v2.0-conll2017.tgz
for conllu in ud-test-v2.0-conll2017/gold/conll17-ud-test-2017-05-09/*.conllu; do
  code=`basename $conllu .conllu`

  case "$code" in
    *_pud | bxr | hsb | kmr | sme) continue;;
  esac

  [ -d "$code" ] || echo Gold file for $code has no training data in UD 2.0 release

  target="$code/$code-ud-test.conllu"
  [ -f "$target" ] && echo Gold file for $code already has fold data in UD 2.0 release

  mv "$conllu" "$target"
done
rm -rf ud-test-v2.0-conll2017
