#!/usr/bin/env python3

# This file is part of UDPipe 2 <http://github.com/ufal/udpipe>.
#
# Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import argparse
import email.mime.multipart
import email.mime.nonmultipart
import email.policy
import json
import os
import sys
import urllib.error
import urllib.request

__version__ = "2.0.0-dev"


def perform_request(server, method, params={}):
    if not params:
        request_headers, request_data = {}, None
    else:
        message = email.mime.multipart.MIMEMultipart("form-data", policy=email.policy.HTTP)

        for name, value in params.items():
            payload = email.mime.nonmultipart.MIMENonMultipart("text", "plain")
            payload.add_header("Content-Disposition", "form-data; name=\"{}\"".format(name))
            payload.add_header("Content-Transfer-Encoding", "8bit")
            payload.set_payload(value, charset="utf-8")
            message.attach(payload)

        request_data = message.as_bytes().split(b"\r\n\r\n", maxsplit=1)[1]
        request_headers = {"Content-Type": message["Content-Type"]}

    try:
        with urllib.request.urlopen(urllib.request.Request(
            url="{}/{}".format(server, method), headers=request_headers, data=request_data
        )) as request:
            return json.loads(request.read())
    except urllib.error.HTTPError as e:
        print("An exception was raised during UDPipe 'process' REST request.\n"
              "The service returned the following error:\n"
              "  {}".format(e.fp.read().decode("utf-8")), file=sys.stderr)
        raise
    except json.JSONDecodeError as e:
        print("Cannot parse the JSON response of UDPipe 'process' REST request.\n"
              "  {}".format(e.msg), file=sys.stderr)
        raise


def list_models(args):
    response = perform_request(args.service, "models")
    if "models" in response:
        for model in response["models"]:
            print(model)
    if "default_model" in response:
        print("Default model:", response["default_model"])


def process(args, data):
    data = {
        "input": args.input,
        "output": args.output,
        "data": data,
    }
    for option in ["model", "tokenizer", "parser", "tagger"]:
        value = getattr(args, option)
        if value is not None:
            data[option] = value

    response = perform_request(args.service, "process", data)
    if "model" not in response or "result" not in response:
        raise ValueError("Cannot parse the UDPipe 'process' REST request response.")

    print("UDPipe generated an output using the model '{}'.".format(response["model"]), file=sys.stderr)
    print("Please respect the model licence (CC BY-NC-SA unless stated otherwise).", file=sys.stderr)

    return response["result"]


if __name__ == "__main__":
    # Parse the client arguments.
    parser = argparse.ArgumentParser(description=(
        "Most of the options are passed directly to the service. For documentation, "
        "see https://lindat.mff.cuni.cz/services/udpipe/api-reference.php ."))
    parser.add_argument("inputs", nargs="*", type=str, help="Optional input files; stdin if not specified.")
    parser.add_argument("--list_models", default=False, action="store_true", help="List available models")
    parser.add_argument("--input", default="conllu", type=str, help="Input format")
    parser.add_argument("--model", default=None, type=str, help="Model to use")
    parser.add_argument("--output", default="conllu", type=str, help="Output format")
    parser.add_argument("--parser", default=None, type=str, help="Parser options")
    parser.add_argument("--tagger", default=None, type=str, help="Tagger options")
    parser.add_argument("--tokenizer", default=None, type=str, help="Tokenizer options")
    parser.add_argument("--outfile", default=None, type=str, help="Output path template; use {} as basename")
    parser.add_argument("--service", default="https://lindat.mff.cuni.cz/services/udpipe/api", type=str, help="Service URL")
    args = parser.parse_args()

    if args.list_models:
        list_models(args)
    else:
        outfile = None  # No output file opened.

        for input_path in (args.inputs or [sys.stdin]):
            # Use stdin if no inputs are specified
            if input_path != sys.stdin:
                with open(input_path, "r", encoding="utf-8-sig") as input_file:
                    data = input_file.read()
            else:
                data = sys.stdin.read()

            if args.outfile and not outfile:
                outfile = args.outfile.replace("{}", (
                    os.path.splitext(os.path.basename(input_path))[0] if input_path != sys.stdin else "{}"))
                outfile = open(outfile, "w", encoding="utf-8")

            (outfile or sys.stdout).write(process(args, data))

            if args.outfile and "{}" in args.outfile:
                outfile.close()
                outfile = None

        if outfile:
            outfile.close()
