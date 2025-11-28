#!/bin/sh

for data in pdtc-2.0-a-ls pdtc-2.0-pdt-a-ls; do
  sh pdtc-2.0/train.sh pdtc-2.0 $data
  EXP=s43 sh pdtc-2.0/train.sh pdtc-2.0 $data --seed=43
done

for data in pdtc-2.0-pa-ls pdtc-2.0-papdt-ls; do
  sh pdtc-2.0/train.sh pdtc-2.0 $data
  EXP=s43 sh pdtc-2.0/train.sh pdtc-2.0 $data --seed=43
done
