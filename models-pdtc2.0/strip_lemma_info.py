#!/usr/bin/env python3
import fileinput
import re

lemma_strip = re.compile(r"(?<=.)(?:`|_|-[^0-9]).*$")

for line in fileinput.input():
    line = line.rstrip("\n")
    columns = line.split("\t")
    if not line.startswith("#") and len(columns) == 10:
        columns[2] = lemma_strip.sub("", columns[2])
        line = "\t".join(columns)
    print(line)
