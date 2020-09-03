#!/usr/bin/env python3

# This file is part of UDPipe 2.0 <http://github.com/ufal/udpipe>.
#
# Copyright 2020 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import argparse
import email.parser
import http.server
import itertools
import json
import os
import socketserver
import sys
import time
import urllib.parse

import ud_dataset
import ud_parser
import ufal.udpipe
import wembedding_service.wembeddings.wembeddings as wembeddings

class Models:
    class Model:
        def __init__(self, names, path, variant, acknowledgements, server_args, loaded_model=None):
            self.names = names
            self.acknowledgements = acknowledgements
            self._server_args = server_args
            self._variant = variant

            # Load the model if required
            if loaded_model is not None:
                self._network_args, self._train, self._network = loaded_model._network_args, loaded_model._train, loaded_model._network
            else:
                with open(os.path.join(path, "options.json"), mode="r") as options_file:
                    self._network_args = argparse.Namespace(**json.load(options_file))
                ud_parser.UDParser.postprocess_arguments(self._network_args)
                self._network_args.batch_size = server_args.batch_size

                self._train = ud_dataset.UDDataset.load_mappings(os.path.join(path, "mappings.pickle"))
                self._network = ud_parser.UDParser(threads=server_args.threads)
                self._network.construct(self._network_args, self._train, [], [], predict_only=True)
                self._network.saver.restore(self._network.session, os.path.join(path, "weights"))

            # Load the tokenizer
            tokenizer_path = os.path.join(path, "{}.tokenizer".format(variant))
            self._tokenizer = ufal.udpipe.Model.load(tokenizer_path)
            if self._tokenizer is None:
                raise RuntimeError("Cannot load tokenizer from {}".format(tokenizer_path))

            self._conllu_input = ufal.udpipe.InputFormat.newConlluInputFormat()
            if self._conllu_input is None:
                raise RuntimeError("Cannot create CoNLL-U input format")

            self._conllu_output = ufal.udpipe.OutputFormat.newConlluOutputFormat()
            if self._conllu_output is None:
                raise RuntimeError("Cannot create CoNLL-U output format")


        def read(self, text, input_format):
            reader = ufal.udpipe.InputFormat.newInputFormat(input_format)
            if reader is None:
                raise RuntimeError("Unknown input format '{}'".format(input_format))
            # Check the format is fine
            for _ in self._read(text, reader): pass
            return self._read(text, reader)

        def tokenize(self, text, tokenizer_options):
            tokenizer = self._tokenizer.newTokenizer(tokenizer_options)
            if tokenizer is None:
                raise RuntimeError("Cannot create tokenizer.")
            return self._read(text, tokenizer)

        def _read(self, text, reader):
            sentence, sentences = ufal.udpipe.Sentence(), []
            processing_error = ufal.udpipe.ProcessingError()
            reader.setText(text)
            while reader.nextSentence(sentence, processing_error):
                yield sentence
                sentence = ufal.udpipe.Sentence()
            if processing_error.occurred():
                raise RuntimeError("Cannot read input data: '{}'".format(processing_error.message))

        def create_writer(self, output_format):
            writer = ufal.udpipe.OutputFormat.newOutputFormat(output_format)
            if writer is None:
                raise RuntimeError("Unknown output format '{}'".format(output_format))
            return writer

        def predict(self, sentences, tag, parse, writer):
            # Run the model
            if tag or parse:
                time_we = time.time()
                wembedding_input, conllu_input = [], []
                for sentence in sentences:
                    wembedding_input.append([word.form for word in sentence.words[1:]])
                    conllu_input.append(self._conllu_output.writeSentence(sentence))

                # Compute the WEmbeddings
                wembeddings = self._server_args.wembedding_server.compute_embeddings(self._network_args.wembedding_model, wembedding_input)

                time_ds = time.time()
                # Create UDDataset
                dataset = ud_dataset.UDDataset(text="".join(conllu_input), embeddings=wembeddings, override_variant=self._variant,
                                               train=self._train, shuffle_batches=False)

                # Prepare network arguments
                network_args = argparse.Namespace(**vars(self._network_args))
                if not tag: network_args.tags = []
                if not parse: network_args.parse = 0

                time_nw = time.time()
                # Perform the prediction
                predicted = self._network.predict(dataset, evaluating=False, args=network_args)

                time_rd = time.time()
                # Load the predicted CoNLL-U to ufal.udpipe sentences
                sentences = self._read(predicted, self._conllu_input)

                print("Request, WE {:.2f}ms,".format(1000 * (time_ds - time_we)),
                      "DS {:.2f}ms,".format(1000 * (time_nw - time_ds)),
                      "NW {:.2f}ms,".format(1000 * (time_rd - time_nw)),
                      "RD {:.2f}ms.".format(1000 * (time.time() - time_rd)),
                      file=sys.stderr, flush=True)

            # Generate output
            output = []
            for sentence in sentences:
                output.append(writer.writeSentence(sentence))
            output.append(writer.finishDocument())
            return "".join(output)

    def __init__(self, server_args):
        self.default_model = server_args.default_model
        self.models_list = []
        self.models_by_names = {}
        models_by_path = {}

        for i in range(0, len(server_args.models), 4):
            names, path, variant, acknowledgements = server_args.models[i:i+4]
            names = names.split(":")
            names = [name.split("-") for name in names]
            names = ["-".join(parts[:None if not i else -i]) for parts in names for i in range(len(parts))]

            self.models_list.append(self.Model(names, path, variant, acknowledgements, server_args, loaded_model=models_by_path.get(path, None)))
            models_by_path.setdefault(path, self.models_list[-1])
            for name in names:
                self.models_by_names.setdefault(name, self.models_list[-1])

        # Check the default model exists
        assert self.default_model in self.models_by_names

class UDServer(socketserver.ThreadingTCPServer):
    class UDServerRequestHandler(http.server.BaseHTTPRequestHandler):
        protocol_version = "HTTP/1.1"

        def respond_error(request, message):
            request.close_connection = True
            request.send_response(400)
            request.send_header("Connection", "close")
            request.send_header("Content-Type", "text/plain")
            request.end_headers()
            request.wfile.write(message.encode("utf-8"))

        def respond_ok(request, content_type):
            request.close_connection = True
            request.send_response(200)
            request.send_header("Connection", "close")
            request.send_header("Content-Type", content_type)
            request.end_headers()

        def do_GET(request):
            # Parse the URL
            params = {}
            try:
                request.path = request.path.encode("iso-8859-1").decode("utf-8")
                url = urllib.parse.urlparse(request.path)
                for name, value in urllib.parse.parse_qsl(url.query, encoding="utf-8", errors="strict"):
                    params[name] = value
            except:
                return request.respond_error("Cannot parse request URL.")

            # Parse the body of a POST request
            if request.command == "POST":
                if request.headers.get("Transfer-Encoding", "identity").lower() != "identity":
                    return request.respond_error("Only 'identity' Transfer-Encoding of payload is supported for now.")

                try:
                    content_length = int(request.headers["Content-Length"])
                except:
                    return request.respond_error("The Content-Length of payload is required.")

                if content_length > request.server._server_args.max_request_size:
                    return request.respond_error("The payload size is too large.")

                # multipart/form-data
                if request.headers.get("Content-Type", "").startswith("multipart/form-data"):
                    try:
                        parser = email.parser.BytesFeedParser()
                        parser.feed(b"Content-Type: " + request.headers["Content-Type"].encode("ascii") + b"\r\n\r\n")
                        while content_length:
                            parser.feed(request.rfile.read(min(content_length, 4096)))
                            content_length -= min(content_length, 4096)
                        for part in parser.close().get_payload():
                            name = part.get_param("name", header="Content-Disposition")
                            if name:
                                params[name] = part.get_payload(decode=True).decode("utf-8")
                    except:
                        return request.respond_error("Cannot parse the multipart/form-data payload.")
                elif request.headers.get("Content-Type", "").startswith("application/x-www-form-urlencoded"):
                    try:
                        for name, value in urllib.parse.parse_qsl(
                                request.rfile.read(content_length).decode("utf-8"), encoding="utf-8", errors="strict"):
                            params[name] = value
                    except:
                        return request.respond_error("Cannot parse the application/x-www-form-urlencoded payload.")
                else:
                    return request.respond_error("Unsupported payload Content-Type '{}'.".format(request.headers.get("Content-Type", "<none>")))

            # Handle /models
            if url.path == "/models":
                response = {
                    "models": {model.names[0]: ["tokenizer", "tagger", "parser"] for model in request.server._models.models_list},
                    "default_model": request.server._models.default_model,
                }
                request.respond_ok("application/json")
                request.wfile.write(json.dumps(response, indent=1).encode("utf-8"))
            # Handle /process
            elif url.path == "/process":
                if "data" not in params:
                    return request.respond_error("The parameter 'data' is required.")

                model = params.get("model", request.server._models.default_model)
                if model not in request.server._models.models_by_names:
                    return request.respond_error("The request model '{}' does not exist.".format(model))
                model = request.server._models.models_by_names[model]

                # Start by reading and optionally tokenizing the input data.
                if "tokenizer" in params:
                    try:
                        sentences = model.tokenize(params["data"], params["tokenizer"])
                    except:
                        return request.respond_error("An error occured during tokenization of the input.")
                else:
                    try:
                        sentences = model.read(params["data"], params.get("input", "conllu"))
                    except:
                        return request.respond_error("Cannot parse the input in '{}' format.".format(params.get("input", "conllu")))

                # Create the writer
                output_format = params.get("output", "conllu")
                try:
                    writer = model.create_writer(output_format)
                except:
                    return request.respond_error("Unknown output format '{}'.".format(output_format))

                # Process the data
                tag, parse, output_format = "tagger" in params, "parser" in params, params.get("output", "conllu")
                batch, started_responding = [], False
                try:
                    for sentence in itertools.chain(sentences, ["EOF"]):
                        if sentence is "EOF" or len(batch) == request.server._server_args.batch_size:
                            output = model.predict(batch, tag, parse, writer)
                            if not started_responding:
                                # The first batch is ready, we commit to generate output.
                                request.respond_ok("application/json")
                                request.wfile.write(json.dumps({
                                    "model": model.names[0],
                                    "acknowledgements": ["http://ufal.mff.cuni.cz/udpipe#udpipe_acknowledgements", model.acknowledgements],
                                    "results": "",
                                }, indent=1)[:-3].encode("utf-8"))
                                if output_format == "conllu":
                                    request.wfile.write(json.dumps(
                                        "# generator = UDPipe 2, https://lindat.mff.cuni.cz/services/udpipe\n"
                                        "# udpipe_model = {}\n"
                                        "# udpipe_model_licence = CC BY-NC-SA\n".format(model.names[0]))[1:-1].encode("utf-8"))
                                started_responding=True
                            request.wfile.write(json.dumps(output, ensure_ascii=False)[1:-1].encode("utf-8"))
                            batch = []
                        batch.append(sentence)
                    request.wfile.write(b'"\n}\n')
                except:
                    import traceback
                    traceback.print_exc(file=sys.stderr)
                    sys.stderr.flush()

                    if not started_responding:
                        request.respond_error("An internal error occurred during processing.")
                    else:
                        request.wfile.write(b'",\n"An internal error occurred during processing, producing incorrect JSON!"')

            else:
                request.respond_error("No handler for the given URL '{}'".format(url.path))

        def do_POST(request):
            return request.do_GET()

    daemon_threads = False

    def __init__(self, server_args, models):
        super().__init__(("", server_args.port), self.UDServerRequestHandler)

        self._server_args = server_args
        self._models = models

    def server_bind(self):
        import socket
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        super().server_bind()

if __name__ == "__main__":
    import signal
    import threading

    # Parse server arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=int, help="Port to use")
    parser.add_argument("wembedding_server", type=str, help="Address of an WEmbedding server")
    parser.add_argument("default_model", type=str, help="Default model")
    parser.add_argument("models", type=str, nargs="+", help="Models to serve")
    parser.add_argument("--batch_size", default=64, type=int, help="Batch size")
    parser.add_argument("--logfile", default=None, type=str, help="Log path")
    parser.add_argument("--max_request_size", default=4096*1024, type=int, help="Maximum request size")
    parser.add_argument("--threads", default=4, type=int, help="Threads to use")
    args = parser.parse_args()

    # Log stderr to logfile if given
    if args.logfile is not None:
        sys.stderr = open(args.logfile, "w", encoding="utf-8")

    # Load the models
    models = Models(args)

    # Create the WEmbeddings client
    args.wembedding_server = wembeddings.WEmbeddings.ClientNetwork(args.wembedding_server)

    # Create the server
    server = UDServer(args, models)
    server_thread = threading.Thread(target=server.serve_forever, daemon=True)
    server_thread.start()

    print("Started UDPipe 2 server on port {}.".format(args.port), file=sys.stderr)
    print("To stop it gracefully, either send SIGINT (Ctrl+C) or SIGUSR1.", file=sys.stderr, flush=True)

    # Wait until the server should be closed
    signal.sigwait([signal.SIGINT, signal.SIGUSR1])
    print("Initiating shutdown of the UDPipe 2 server.", file=sys.stderr, flush=True)
    server.shutdown()
    print("Stopped handling new requests, processing all current ones.", file=sys.stderr, flush=True)
    server.server_close()
    print("Finished shutdown of the UDPipe 2 server.", file=sys.stderr, flush=True)
