# UDPipe 2

This is the UDPipe 2, a POS tagger, lemmatizer and dependency parser.

Compared to UDPipe 1:
- UDPipe 2 is Python-only and tested only in Linux,
- UDPipe 2 is meant as a research tool, not as a user-friendly UDPipe 1 replacement,
- UDPipe 2 achieves much better performance, but requires a GPU for reasonable
  performance,
- UDPipe 2 does not perform tokenization by itself – it uses UDPipe 1 for that.

UDPipe 2 is available as a REST service running at https://lindat.mff.cuni.cz/services/udpipe.
If you like, you can use the [udpipe2_client.py](https://github.com/ufal/udpipe/blob/udpipe-2/udpipe2_client.py)
script to interact with it.

However, if you prefer to run UDPipe 2 locally, you have come to the right place.

## Running Inference with Existing Models

To run UDPipe 2, you need to first download a model from the
[list of UDPipe 2 models](https://ufal.mff.cuni.cz/udpipe/2/models).
Then you can run UDPipe 2 as a local REST server, and use the
[udpipe2_client.py](https://github.com/ufal/udpipe/blob/udpipe-2/udpipe2_client.py)
script to interact with it (in the same way as with the official service).

To run the server, use the [udpipe2_server.py](https://github.com/ufal/udpipe/blob/udpipe-2/udpipe2_server.py)
script.
- Install the [requirements.txt](https://github.com/ufal/udpipe/blob/udpipe-2/requirements.txt). While only
  TF 1 is supported for model training (ancient, I know), you can use also TF 2 for inference.
- The script has the following required options:
  - `port`: the port to listen on. We use `SO_REUSEPORT` to allow multiple processes
    to run concurrently, supporting seamless upgrades;
  - `default_model`: model name to use when no model is specified in the
    request;
  - `models`: each model is then a quadruple of the following parameters (each
    published model contains a file `MODEL.txt` with these parameters):
    - `model names`: any number of model names separated by `:`; furthermore, any
      hyphen-separated prefix of any model name can be also used as a name
      (e.g., `czech-pdt-ud-2.10-220711:cs_pdt-ud-2.10-220711:cs:ces:cze`);
    - `model path`: path to the model directory;
    - `treebank name`: because multiple treebanks can be handled by a single
      model, we need to specify a treebank name to use (this also specifies
      which tokenizer to use from the model directory);
    - `acknowledgements`: a URL to the model's acknowledgements.
- The script has the following optional parameters:
  - `--batch_size`: batch size to use (default 32);
  - `--logfile`: if specified, log to this file instead of standard error;
  - `--max_request_size`: maximum request size, in bytes (default 4MB);
  - `--preload_models`: list of models to preload (or `all`) immediately after
    start (default none);
  - `--threads`: number of threads to use (default is to use all physical cores);
  - `--wembedding_server`: for deployment purposes, it might be useful to
    compute the contextualized embeddings (mBERT, RobeCzech) not in the UDPipe 2
    service, but in a specialized service – see https://github.com/ufal/wembedding_service
    for documentation of the wembeddings service (default is to compute the
    embeddings directly in the UDPipe 2 service).

The service can be stopped by a `SIGINT` (Ctrl+C) signal or by a `SIGUSR1` signal.
Once such a signal is received, the service stops accepting new requests, but
waits until all existing connections are handled and closed.

The models are loaded on-demand, but they are never freed. If a GPU is
available, then all computation is performed on it (and an OOM might occur if
too many models are loaded). If you would like to run BERT on a GPU and the
remaining computation on a CPU, you could use GPU-enabled wembeddings service
plus a CPU-only UDPipe 2 service.

## Training New Models

You can train UDPipe 2 models, but we provide no support for the model training,
and you will probably need to read the source code to find out what various
training options do.

To train a new UDPipe model, you need to perform three steps, assuming
you have the data in CoNLL-U format:
1. First, you need to compute the contextualized embeddings for your data,
   using the `wembedding_service/compute_wembeddings.py` script.

   The official UDPipe models use the
   [scripts/compute_wembeddings.sh](https://github.com/ufal/udpipe/blob/udpipe-2/scripts/compute_embeddings.sh)
   script, where you can see how to name the outputs and which BERT-like models
   are used for which treebanks.

2. Then, you need to train the UDPipe 2 models themselves, using the
   [udpipe2.py](https://github.com/ufal/udpipe/blob/udpipe-2/udpipe2.py) script.
   You can generally use the default hyperparameters, you only need to specify
   the path of the trained model (as the first argument) and then paths to your
   data using `--train`, `--dev`, and `--test` options.

   The official UDPipe 2 models are trained using the
   [scripts/train.sh](https://github.com/ufal/udpipe/blob/udpipe-2/scripts/train.sh)
   script; you can see they tweak `--batch_size` and `--rnn_cell_dim` a bit
   depending on the trained treebank size.

3. Because UDPipe 2 does not include tokenization functionality, you need to
   [train a UDPipe 1 tokenizer](https://ufal.mff.cuni.cz/udpipe/1/users-manual#model_training_tokenizer).
   The tokenizer should then be put in the trained UDPipe 2 model directory
   under the name `VARIANT.tokenizer`, where `VARIANT` is the variant you specify
   on the command line of the
   [udpipe2_server.py](https://github.com/ufal/udpipe/blob/udpipe-2/udpipe2_server.py).
