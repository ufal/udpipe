#!/usr/bin/env python3
import argparse
import struct

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=str, help="Input MorphoDiTa model")
    parser.add_argument("output", type=str, help="Output UDPipe model")
    args = parser.parse_args()

    with open(args.input, mode="rb") as input_file, open(args.output, mode="wb") as output_file:
        # Model ID and version
        output_file.write(struct.pack("B", len(b"morphodita_parsito")))
        output_file.write(b"morphodita_parsito")
        output_file.write(struct.pack("3B", 3, 0x7F, 0x7F))

        # Tokenizer
        output_file.write(struct.pack("4B", 1, 0, 0, 2))

        # Splitter
        output_file.write(struct.pack(
            "25B", 0x02, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x4b,
                   0xc1, 0x0a, 0x9d, 0x5d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
                   0x6a, 0x7e, 0x56, 0x00, 0x00))

        # Tagger
        output_file.write(struct.pack("4B", 1, 1, 1, 0))
        output_file.write(input_file.read())

        # Parser
        output_file.write(struct.pack("B", 0))

