# build: `docker build -t udpipe/server .`
# run:   `docker run --rm -it -p 8080:8080 udpipe/server`
#
# see also https://github.com/samisalkosuo/udpipe-rest-server-docker
#
FROM ubuntu:18.04

ENV MODEL_FILE_NAME test.model
ENV MODEL_NAME test
ENV MODEL_DESC test description

RUN apt-get -q update && \
    apt-get -q install -y curl gcc g++ build-essential && \
    g++ --version

ADD src /udpipe/src
ADD releases/test_data/${MODEL_FILE_NAME} /models/
WORKDIR /udpipe/src/rest_server

RUN cd /udpipe/src && BITS=64 MODE=release make -j4 server

EXPOSE 8080
CMD ./udpipe_server "8080" "${MODEL_NAME}" "${MODEL_NAME}" "/models/${MODEL_FILE_NAME}" "${MODEL_DESC}"
