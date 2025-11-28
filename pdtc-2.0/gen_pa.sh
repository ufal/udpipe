#!/bin/sh

# mkdir -p pdtc-2.0-pdt-onlym-pa-ls/
# (cd ../ && sh pdtc-2.0/parse.sh pdtc-2.0/pdtc-2.0-pdt-onlym-ls/pdtc-2.0-pdt-onlym-train.conllu models/pdtc-2.0-pdtc-2.0-a-ls/ pdtc-2.0/pdtc-2.0-pdt-onlym-pa-ls/pdtc-2.0-pdt-onlym-pa-train.conllu)
# (cd pdtc-2.0-pdt-onlym-pa-ls/ && ln -s ../pdtc-2.0-pdt-onlym-ls/pdtc-2.0-pdt-onlym-train.conllu.npz pdtc-2.0-pdt-onlym-pa-train.conllu.npz)

# mkdir -p pdtc-2.0-pdt-onlym-papdt-ls/
# (cd ../ && sh pdtc-2.0/parse.sh pdtc-2.0/pdtc-2.0-pdt-onlym-ls/pdtc-2.0-pdt-onlym-train.conllu models/pdtc-2.0-pdtc-2.0-pdt-a-ls/ pdtc-2.0/pdtc-2.0-pdt-onlym-papdt-ls/pdtc-2.0-pdt-onlym-papdt-train.conllu)
# (cd pdtc-2.0-pdt-onlym-papdt-ls/ && ln -s ../pdtc-2.0-pdt-onlym-ls/pdtc-2.0-pdt-onlym-train.conllu.npz pdtc-2.0-pdt-onlym-papdt-train.conllu.npz)

for c in faust pcedt pdtsc; do
  mkdir -p pdtc-2.0-$c-papdt-ls
  (cd ../ && sh pdtc-2.0/parse.sh pdtc-2.0/pdtc-2.0-$c-a-ls/pdtc-2.0-$c-a-train.conllu models/pdtc-2.0-pdtc-2.0-pdt-a-ls/ pdtc-2.0/pdtc-2.0-$c-papdt-ls/pdtc-2.0-$c-papdt-train.conllu)
  (cd pdtc-2.0-$c-papdt-ls/ && ln -s ../pdtc-2.0-$c-a-ls/pdtc-2.0-$c-a-train.conllu.npz pdtc-2.0-$c-papdt-train.conllu.npz)
done
