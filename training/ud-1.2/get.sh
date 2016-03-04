#!/bin/sh

wget --content-disposition 'https://lindat.mff.cuni.cz/repository/xmlui/bitstream/handle/11234/1-1548/ud-treebanks-v1.2.tgz?sequence=1&isAllowed=y'
tar xf ud-treebanks-v1.2.tgz
rm ud-treebanks-v1.2.tgz

for tb in universal-dependencies-1.2/*/*-ud-dev.conllu; do
  dir=`dirname $tb`
  code=`basename $tb`
  code=${code%%-*}

  mv $dir $code
done
rmdir universal-dependencies-1.2
