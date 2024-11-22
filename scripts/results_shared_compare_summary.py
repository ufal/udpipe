#!/usr/bin/env python3
import sys

treebank, results = None, []
while (line := sys.stdin.readline()):
    parts = line.rstrip("\r\n").split(" ")
    if len(parts) == 1:
        if results:
            print(treebank, *results)
        treebank = parts[0]
        results = []
    else:
        values = []
        for i in range(2):
            if i:
                parts = sys.stdin.readline().rstrip("\r\n").split(" ")
            values.append(sum(float(parts[parts.index(f"{key}:") + 1].rstrip(",")) for key in ["AllTags", "Lemmas", "LAS"]))
        results.append(f"{values[1] - values[0]:.3f}")

if results:
    print(treebank, *results)
