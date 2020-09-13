#!/usr/bin/env python3
import re
import sys

re_number = re.compile(r"(?:^|(?<=[:\s]\s))([0-9]+(?:[.][0-9]*)?)")

line = sys.stdin.readline().rstrip("\n")
print(line)
references = [float(piece) if re_number.fullmatch(piece) else None for piece in re_number.split(line)]

for line in sys.stdin:
    line = line.rstrip("\n")
    for i, piece in enumerate(re_number.split(line)):
        if i >= len(references): continue
        if references[i] is not None and re_number.fullmatch(piece):
            piece = "{:.2f}".format(float(piece) - references[i])
        print(piece, end="")
    print()
