#!/bin/bash

set -e

[ "$#" -ge 2 ] || { echo Usage: $0 model_family model_version >&2; exit 1; }
family="$1"
version="$2"

dir="udpipe-$family-$version"
[ -d "$dir" ] && { echo Release $dir already exists >&2; exit 1; }

mkdir "$dir"

cp LICENSE.CC-BY-NC-SA-4 "$dir"/LICENSE

make -C ../doc manual_model_${family}_readme.{txt,html}
cp ../doc/manual_model_${family}_readme.txt "$dir"/README
cp ../doc/manual_model_${family}_readme.html "$dir"/README.html
make -C ../doc clean

for model in ../training/models-$family/*/*.model; do
  lang=`basename $model .model`
  long_name=`awk "/^$lang /{print \\\$2}" lang-names.txt`
  [ -z "$long_name" ] && { echo Unknown language code $lang >&2; exit 1; }
  ln -s ../$model $dir/$long_name-$family-$version.udpipe
done
