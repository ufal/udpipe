#!/usr/bin/env python3
# coding=utf-8
#
# Copyright 2020 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

"""Word embeddings server class."""

import email.parser
import http.server
import json
import socketserver
import sys
import threading
import time
import urllib.error
import urllib.parse
import urllib.request


class FrontendRESTServer(socketserver.TCPServer):
    class Backend():
        def __init__(self, server):
            self._server = server

            with self.request("/models") as response:
                data = json.loads(response.read())

            assert "models" in data and isinstance(data["models"], dict)
            self.models = data["models"]
            self.aliases = [model.split("-") for model in self.models]
            self.aliases = set(["-".join(parts[:None if not i else -i]) for parts in self.aliases for i in range(len(parts))])

            assert "default_model" in data and isinstance(data["default_model"], str)
            self.default_model = data["default_model"]

        def request(self, url, data=None, data_content_type=None):
            return urllib.request.urlopen(urllib.request.Request(
                url="http://{}{}".format(self._server, url),
                data=data,
                headers={} if data is None else {"Content-Type": data_content_type},
            ))


    class FrontendRESTServer(http.server.BaseHTTPRequestHandler):
        protocol_version = "HTTP/1.1"

        format_for_log_table = str.maketrans("\n", "\r", "\r")
        def format_for_log(request, data):
            if len(data) > request.server._args.log_data:
                data = data[:request.server._args.log_data // 2] + " ... " + data[min(-1, -request.server._args.log_data // 2):]
            return data.translate(request.format_for_log_table)

        def respond(request, content_type, code=200):
            request.close_connection = True
            request.send_response(code)
            request.send_header("Connection", "close")
            request.send_header("Content-Type", content_type)
            request.send_header("Access-Control-Allow-Origin", "*")
            request.end_headers()

        def respond_error(request, message, code=400):
            request.respond("text/plain", code)
            request.wfile.write(message.encode("utf-8"))

        def do_GET(request):
            # Parse the model from URL/body
            params, body, body_content_type = {}, None, None
            try:
                encoded_path = request.path.encode("iso-8859-1").decode("utf-8")
                url = urllib.parse.urlparse(encoded_path)
                for name, value in urllib.parse.parse_qsl(url.query, encoding="utf-8", keep_blank_values=True, errors="strict"):
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

                if content_length > request.server._args.max_request_size:
                    return request.respond_error("The payload size is too large.")

                body = request.rfile.read(content_length)
                body_content_type = request.headers.get("Content-Type", "")

                # multipart/form-data
                if request.headers.get("Content-Type", "").startswith("multipart/form-data"):
                    try:
                        parser = email.parser.BytesFeedParser()
                        parser.feed(b"Content-Type: " + request.headers["Content-Type"].encode("ascii") + b"\r\n\r\n")
                        parser.feed(body)
                        for part in parser.close().get_payload():
                            name = part.get_param("name", header="Content-Disposition")
                            if name:
                                params[name] = part.get_payload(decode=True).decode("utf-8")
                    except:
                        return request.respond_error("Cannot parse the multipart/form-data payload.")
                # x-www-form-urlencoded
                elif request.headers.get("Content-Type", "").startswith("application/x-www-form-urlencoded"):
                    try:
                        for name, value in urllib.parse.parse_qsl(body.decode("utf-8"), encoding="utf-8", errors="strict"):
                            params[name] = value
                    except:
                        return request.respond_error("Cannot parse the application/x-www-form-urlencoded payload.")

            # Log if required
            if request.server._args.log_data:
                print(url.path, " ".join(request.headers.get_all("X-Forwarded-For", [])),
                      *["{}:{}".format(key, request.format_for_log(value)) for key, value in params.items() if key != "data"],
                      "data:" + request.format_for_log(params.get("data", "")),
                      sep="\t", file=sys.stderr, flush=True)

            # Handle /models
            if url.path == "/models":
                response = {
                    "models": {name: value for backend in request.server.backends for name, value in backend.models.items()},
                    "default_model": request.server.backends[0].default_model,
                }
                request.respond("application/json")
                request.wfile.write(json.dumps(response, indent=1).encode("utf-8"))
            # Handle everything else
            else:
                # Start by finding appropriate backend
                backend = request.server.backends[0]
                for candidate in request.server.backends:
                    if "model" in params and params["model"] in candidate.aliases:
                        backend = candidate
                        break

                # Forward the request to the backend
                started_responding = False
                try:
                    try:
                        with backend.request(request.path, body, body_content_type) as response:
                            while True:
                                data = response.read(32768)
                                if len(data) == 0: break
                                if not started_responding:
                                    started_responding = True
                                    request.respond(response.getheader("Content-Type", "application/json"), code=response.code)
                                request.wfile.write(data)
                    except urllib.error.HTTPError as error:
                        if not started_responding:
                            started_responding = True
                            request.respond(error.headers.get("Content-Type", "text/plain"), code=error.code)
                            request.wfile.write(error.file.read())
                except:
                    import traceback
                    traceback.print_exc(file=sys.stderr)
                    sys.stderr.flush()

                    if not started_responding:
                        request.respond_error("An internal error occurred during processing.")
                    else:
                        request.wfile.write(b'",\n"An internal error occurred during processing, producing incorrect JSON!"')

        def do_POST(request):
            return request.do_GET()

    def __init__(self, args):
        self._args = args

        # Initialize all backends
        self.backends = [self.Backend(backend) for backend in args.backends]

        # Initialize the server
        self._threads = []
        super().__init__(("", self._args.port), self.FrontendRESTServer)

    def server_bind(self):
        import socket
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        super().server_bind()

    def server_activate(self):
        self.socket.listen(256)

    def process_request_thread(self, request, client_address):
        try:
            self.finish_request(request, client_address)
        except Exception:
            self.handle_error(request, client_address)
        finally:
            self.shutdown_request(request)

    def process_request(self, request, client_address):
        thread = threading.Thread(target=self.process_request_thread, args=(request, client_address), daemon=False)
        self._threads.append(thread)
        thread.start()

    def service_actions(self):
        if len(self._threads) >= self._args.max_concurrency:
            self._threads = [thread for thread in self._threads if thread.is_alive()]

        while len(self._threads) >= self._args.max_concurrency:
            time.sleep(0.1)
            self._threads = [thread for thread in self._threads if thread.is_alive()]

    def server_close(self):
        super().server_close()
        for thread in self._threads:
            thread.join()


if __name__ == "__main__":
    import argparse
    import signal

    # Parse server arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=int, help="Port to use")
    parser.add_argument("backends", type=str, nargs="+", help="Backends to use")
    parser.add_argument("--logfile", default=None, type=str, help="Log path")
    parser.add_argument("--log_data", default=None, type=int, help="Log that much bytes of every request data")
    parser.add_argument("--max_concurrency", default=256, type=int, help="Maximum concurrency")
    parser.add_argument("--max_request_size", default=4096*1024, type=int, help="Maximum request size")
    args = parser.parse_args()

    # Log stderr to logfile if given
    if args.logfile is not None:
        sys.stderr = open(args.logfile, "a", encoding="utf-8")

    # Create the server
    server = FrontendRESTServer(args)
    server_thread = threading.Thread(target=server.serve_forever, daemon=True)
    server_thread.start()

    print("Started Frontend REST server on port {}.".format(args.port), file=sys.stderr)
    print("To stop it gracefully, either send SIGINT (Ctrl+C) or SIGUSR1.", file=sys.stderr, flush=True)

    # Wait until the server should be closed
    signal.pthread_sigmask(signal.SIG_BLOCK, [signal.SIGINT, signal.SIGUSR1])
    signal.sigwait([signal.SIGINT, signal.SIGUSR1])
    print("Initiating shutdown of the Frontend REST server.", file=sys.stderr, flush=True)
    server.shutdown()
    print("Stopped handling new requests, processing all current ones.", file=sys.stderr, flush=True)
    server.server_close()
    print("Finished shutdown of the Frontend REST server.", file=sys.stderr, flush=True)
