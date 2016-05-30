#!/bin/sh

wget --content-disposition 'https://lindat.mff.cuni.cz/repository/xmlui/bitstream/handle/11234/1-1699/ud-treebanks-v1.3.tgz?sequence=1&isAllowed=y'
tar xf ud-treebanks-v1.3.tgz
rm ud-treebanks-v1.3.tgz

for tb in ud-treebanks-v1.3/*/*-ud-dev.conllu; do
  dir=`dirname $tb`
  code=`basename $tb`
  code=${code%%-*}

  mv $dir $code
done
rmdir ud-treebanks-v1.3
