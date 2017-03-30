#!/bin/sh

set -e

git clone --depth=1 --branch=pages-source https://github.com/UniversalDependencies/docs

cat docs/gen_index/flags.json | perl -nle '/^"([^"]*)":"([^"]*)",?$/ and print lc($1) . " $2"' | while read name code; do
  cp docs/flags/svg/$code.svg $name.svg
done

rm -rf docs

echo All done
