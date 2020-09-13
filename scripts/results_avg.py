#!/usr/bin/env python3
import re
import sys

re_number = re.compile(r"([0-9]+(?:[.][0-9]*)?)")

sums = []
entries = 0
for line in sys.stdin:
    line = line.rstrip("\n")
    for i, piece in enumerate(re_number.split(line)):
        if i == len(sums):
            sums.append(float(piece) if re_number.fullmatch(piece) else piece)
        elif isinstance(sums[i], float):
            sums[i] += float(piece)
    assert i + 1 == len(sums)
    for piece in re_number.split(line):
        print("{:.2f}".format(float(piece)) if re_number.fullmatch(piece) else piece, end="")
    print()
    entries += 1

first = True
for entry in sums:
    if isinstance(entry, float):
        print("{:.2f}".format(entry / entries), end="")
        first = False
    else:
        print(re.sub(r"\S", "*", entry) if first else entry, end="")
print()
