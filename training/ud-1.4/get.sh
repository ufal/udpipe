#!/bin/sh

wget --content-disposition 'https://lindat.mff.cuni.cz/repository/xmlui/bitstream/handle/11234/1-1827/ud-treebanks-v1.4.tgz?sequence=4&isAllowed=y'
tar xf ud-treebanks-v1.4.tgz
rm ud-treebanks-v1.4.tgz

for tb in ud-treebanks-v1.4/*/*-ud-dev.conllu; do
  dir=`dirname $tb`
  code=`basename $tb`
  code=${code%%-*}

  mv $dir $code
done
rmdir ud-treebanks-v1.4
