#!/usr/bin/env python3

# This file is part of UDPipe 2 <http://github.com/ufal/udpipe>.
#
# Copyright 2020 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import argparse
import os
import sys
import time
import warnings
os.environ.setdefault("TF_USE_LEGACY_KERAS", "1")  # Use legacy Keras 2, not Keras 3

import numpy as np
import tensorflow as tf
import ufal.chu_liu_edmonds

import udpipe2_dataset
import udpipe2_eval

__version__ = "2.1.1-dev"


# Use tf.compat.v1 if running with TF2. Only prediction is supported
# in this case, because we use tf.compat.opt.LazyAdamOptimizer, which
# is not available in TF2.
if not tf.__version__.startswith("1"):
    tf = tf.compat.v1

# Disable TF warnings
tf.logging.set_verbosity(tf.logging.ERROR)

# Ignore warnings containing "is deprecated"
warnings.filterwarnings("ignore", message=".*is deprecated")

class UDPipe2:
    METRICS = ["UPOS", "XPOS", "UFeats", "AllTags", "Lemmas", "UAS", "LAS", "CLAS", "MLAS", "BLEX"]

    def __init__(self, threads, seed=42):
        self.morphodita = None

        # Create an empty graph and a session
        graph = tf.Graph()
        graph.seed = seed
        self.session = tf.Session(graph=graph, config=tf.ConfigProto(inter_op_parallelism_threads=threads,
                                                                     intra_op_parallelism_threads=threads,
                                                                     allow_soft_placement=True))

    def construct(self, args, train, devs, tests, predict_only):
        num_words = len(train.factors[train.FORMS].words)
        num_chars = len(train.factors[train.FORMS].alphabet)
        num_tags = {tag: len(train.factors[train.FACTORS_MAP[tag]].words) for tag in args.tags}
        num_deprels = len(train.factors[train.DEPREL].words)

        with self.session.graph.as_default():
            # Inputs
            self.sentence_lens = tf.placeholder(tf.int32, [None])
            self.word_ids = tf.placeholder(tf.int32, [None, None])
            self.charseqs = tf.placeholder(tf.int32, [None, None])
            self.charseq_lens = tf.placeholder(tf.int32, [None])
            self.charseq_ids = tf.placeholder(tf.int32, [None, None])
            if train.variants > 1: self.variants = tf.placeholder(tf.int32, [None])
            if train.embeddings_size: self.embeddings = tf.placeholder(tf.float32, [None, None, train.embeddings_size])
            self.tags = dict((tag, tf.placeholder(tf.int32, [None, None])) for tag in args.tags)
            self.heads = tf.placeholder(tf.int32, [None, None])
            self.deprels = tf.placeholder(tf.int32, [None, None])
            self.is_training = tf.placeholder(tf.bool, [])
            self.learning_rate = tf.placeholder(tf.float32, [])

            # RNN Cell
            if args.rnn_cell == "LSTM":
                rnn_cell = tf.nn.rnn_cell.LSTMCell
            elif args.rnn_cell == "GRU":
                rnn_cell = tf.nn.rnn_cell.GRUCell
            else:
                raise ValueError("Unknown rnn_cell {}".format(args.rnn_cell))

            # Word embeddings
            inputs = []
            if args.we_dim:
                word_embeddings = tf.get_variable("word_embeddings", shape=[num_words, args.we_dim], dtype=tf.float32)
                inputs.append(tf.nn.embedding_lookup(word_embeddings, self.word_ids))

            # Character-level embeddings
            character_embeddings = tf.get_variable("character_embeddings", shape=[num_chars, args.cle_dim], dtype=tf.float32)
            characters_embedded = tf.nn.embedding_lookup(character_embeddings, self.charseqs)
            characters_embedded = tf.layers.dropout(characters_embedded, rate=args.dropout, training=self.is_training)
            _, (state_fwd, state_bwd) = tf.nn.bidirectional_dynamic_rnn(
                tf.nn.rnn_cell.GRUCell(args.cle_dim), tf.nn.rnn_cell.GRUCell(args.cle_dim),
                characters_embedded, sequence_length=self.charseq_lens, dtype=tf.float32)
            cle = tf.concat([state_fwd, state_bwd], axis=1)
            cle_inputs = tf.nn.embedding_lookup(cle, self.charseq_ids)
            # If CLE dim is half WE dim, we add them together, which gives
            # better results; otherwise we concatenate CLE and WE.
            if 2 * args.cle_dim == args.we_dim:
                inputs[-1] += cle_inputs
            else:
                inputs.append(cle_inputs)

            # Variants
            if train.variants > 1:
                variant_embeddings = tf.get_variable("variant_embeddings", shape=[train.variants, args.variant_dim], dtype=tf.float32)
                variant_embeddings = tf.nn.embedding_lookup(variant_embeddings, self.variants)
                variant_embeddings = tf.expand_dims(variant_embeddings, axis=1)
                variant_embeddings = tf.tile(variant_embeddings, [1, tf.shape(self.word_ids)[1], 1])
                inputs.append(variant_embeddings)

            # Contextualized embeddings
            if train.embeddings_size:
                inputs.append(self.embeddings)

            # All inputs done
            inputs = tf.concat(inputs, axis=2)

            # Shared RNN layers
            hidden_layer = tf.layers.dropout(inputs, rate=args.dropout, training=self.is_training)
            for i in range(args.rnn_layers):
                (hidden_layer_fwd, hidden_layer_bwd), _ = tf.nn.bidirectional_dynamic_rnn(
                    rnn_cell(args.rnn_cell_dim), rnn_cell(args.rnn_cell_dim),
                    hidden_layer, sequence_length=self.sentence_lens + 1, dtype=tf.float32,
                    scope="word-level-rnn-{}".format(i))
                previous = hidden_layer
                hidden_layer = tf.layers.dropout(hidden_layer_fwd + hidden_layer_bwd, rate=args.dropout, training=self.is_training)
                if i: hidden_layer += previous

            # Tagger
            loss = 0
            weights = tf.sequence_mask(self.sentence_lens, dtype=tf.float32)
            weights_sum = tf.reduce_sum(weights)
            self.predictions, self.predictions_logits = {}, {}
            tag_hidden_layer = hidden_layer[:, 1:]
            for i in range(args.rnn_layers_tagger):
                (hidden_layer_fwd, hidden_layer_bwd), _ = tf.nn.bidirectional_dynamic_rnn(
                    rnn_cell(args.rnn_cell_dim), rnn_cell(args.rnn_cell_dim),
                    tag_hidden_layer, sequence_length=self.sentence_lens, dtype=tf.float32,
                    scope="word-level-rnn-tag-{}".format(i))
                previous = tag_hidden_layer
                tag_hidden_layer = tf.layers.dropout(hidden_layer_fwd + hidden_layer_bwd, rate=args.dropout, training=self.is_training)
                if i: tag_hidden_layer += previous
            for tag in args.tags:
                tag_layer = tag_hidden_layer
                for _ in range(args.tag_layers):
                    tag_layer += tf.layers.dropout(tf.layers.dense(tag_layer, args.rnn_cell_dim, activation=tf.nn.tanh), rate=args.dropout, training=self.is_training)
                if tag == "LEMMAS": tag_layer = tf.concat([tag_layer, cle_inputs[:, 1:]], axis=2)
                output_layer = tf.layers.dense(tag_layer, num_tags[tag])
                self.predictions[tag] = tf.argmax(output_layer, axis=2, output_type=tf.int32)
                self.predictions_logits[tag] = output_layer

                if args.label_smoothing:
                    gold_labels = tf.one_hot(self.tags[tag], num_tags[tag]) * (1 - args.label_smoothing) + args.label_smoothing / num_tags[tag]
                    loss += tf.losses.softmax_cross_entropy(gold_labels, output_layer, weights=weights)
                else:
                    loss += tf.losses.sparse_softmax_cross_entropy(self.tags[tag], output_layer, weights=weights)

            # Parsing
            if args.parse:
                max_words = tf.reduce_max(self.sentence_lens)

                if args.rnn_layers == 0:
                    parser_inputs = [inputs]
                    for tag in ["UPOS", "XPOS", "FEATS"]:
                        parser_inputs.append(tf.nn.embedding_lookup(tf.get_variable(tag + "_embeddings", shape=[num_tags[tag], 128], dtype=tf.float32),
                                                                    tf.pad(self.predictions[tag], ((0, 0),(1, 0)), constant_values=2)))
                    parser_inputs = tf.concat(parser_inputs, axis=2)
                    hidden_layer = tf.layers.dropout(parser_inputs, rate=args.dropout, training=self.is_training)

                for i in range(args.rnn_layers_parser):
                    (hidden_layer_fwd, hidden_layer_bwd), _ = tf.nn.bidirectional_dynamic_rnn(
                        rnn_cell(args.rnn_cell_dim), rnn_cell(args.rnn_cell_dim),
                        hidden_layer, sequence_length=self.sentence_lens + 1, dtype=tf.float32,
                        scope="word-level-rnn-parser-{}".format(i))
                    previous = hidden_layer
                    hidden_layer = tf.layers.dropout(hidden_layer_fwd + hidden_layer_bwd, rate=args.dropout, training=self.is_training)
                    if i: hidden_layer += previous

                # Heads
                head_deps = hidden_layer[:, 1:]
                for _ in range(args.parser_layers):
                    head_deps += tf.layers.dropout(tf.layers.dense(head_deps, args.rnn_cell_dim, activation=tf.nn.tanh), rate=args.dropout, training=self.is_training)
                head_roots = hidden_layer
                for _ in range(args.parser_layers):
                    head_roots += tf.layers.dropout(tf.layers.dense(head_roots, args.rnn_cell_dim, activation=tf.nn.tanh), rate=args.dropout, training=self.is_training)

                head_deps_bias = tf.get_variable("head_deps_bias", [args.rnn_cell_dim], dtype=tf.float32, initializer=tf.zeros_initializer)
                head_roots_bias = tf.get_variable("head_roots_bias", [args.rnn_cell_dim], dtype=tf.float32, initializer=tf.zeros_initializer)
                head_biaffine = tf.get_variable("head_biaffine", [args.rnn_cell_dim, args.rnn_cell_dim], dtype=tf.float32, initializer=tf.zeros_initializer)

                heads = tf.reshape(tf.matmul(tf.reshape(head_deps, [-1, args.rnn_cell_dim]) + head_deps_bias, head_biaffine),
                                   [tf.shape(hidden_layer)[0], -1, args.rnn_cell_dim])
                heads = tf.matmul(heads, head_roots + head_roots_bias, transpose_b=True)
                self.heads_logs = tf.nn.log_softmax(heads)
                if args.label_smoothing:
                    gold_labels = tf.one_hot(self.heads, max_words + 1) * (1 - args.label_smoothing)
                    gold_labels += args.label_smoothing / tf.to_float(max_words + 1)
                    loss += tf.losses.softmax_cross_entropy(gold_labels, heads, weights=weights)
                else:
                    loss += tf.losses.sparse_softmax_cross_entropy(self.heads, heads, weights=weights)

                # Deprels
                self.deprel_hidden_layer = tf.identity(hidden_layer)
                self.deprel_heads = tf.identity(self.heads)

                deprel_deps = tf.layers.dropout(tf.layers.dense(self.deprel_hidden_layer[:, 1:], args.parser_deprel_dim, activation=tf.nn.tanh), rate=args.dropout, training=self.is_training)
                for _ in range(args.parser_layers - 1):
                    deprel_deps += tf.layers.dropout(tf.layers.dense(deprel_deps, args.parser_deprel_dim, activation=tf.nn.tanh), rate=args.dropout, training=self.is_training)

                deprel_indices = tf.stack([
                    tf.tile(tf.expand_dims(tf.range(tf.shape(self.deprel_heads)[0]), axis=1), multiples=[1, tf.shape(self.deprel_heads)[1]]),
                    self.deprel_heads], axis=2)
                deprel_roots = tf.gather_nd(self.deprel_hidden_layer, deprel_indices, )
                deprel_roots = tf.layers.dropout(tf.layers.dense(deprel_roots, args.parser_deprel_dim, activation=tf.nn.tanh), rate=args.dropout, training=self.is_training)
                for _ in range(args.parser_layers - 1):
                    deprel_roots += tf.layers.dropout(tf.layers.dense(deprel_roots, args.parser_deprel_dim, activation=tf.nn.tanh), rate=args.dropout, training=self.is_training)

                deprel_deps_bias = tf.get_variable("deprel_deps_bias", [args.parser_deprel_dim], dtype=tf.float32, initializer=tf.zeros_initializer)
                deprel_roots_bias = tf.get_variable("deprel_roots_bias", [args.parser_deprel_dim], dtype=tf.float32, initializer=tf.zeros_initializer)
                deprel_biaffine = tf.get_variable("deprel_biaffine", [args.parser_deprel_dim, num_deprels * args.parser_deprel_dim], dtype=tf.float32, initializer=tf.zeros_initializer)

                deprels = tf.reshape(tf.matmul(tf.reshape(deprel_deps, [-1, args.parser_deprel_dim]) + deprel_deps_bias, deprel_biaffine),
                                     [tf.shape(self.deprel_hidden_layer)[0], -1, num_deprels, args.parser_deprel_dim])
                deprels = tf.squeeze(tf.matmul(deprels, tf.expand_dims(deprel_roots + deprel_roots_bias, axis=3)), axis=3)
                self.predictions_deprel = tf.argmax(deprels, axis=2, output_type=tf.int32)
                if args.label_smoothing:
                    gold_labels = tf.one_hot(self.deprels, num_deprels) * (1 - args.label_smoothing)
                    gold_labels += args.label_smoothing / num_deprels
                    loss += tf.losses.softmax_cross_entropy(gold_labels, deprels, weights=weights)
                else:
                    loss += tf.losses.sparse_softmax_cross_entropy(self.deprels, deprels, weights=weights)

            # Pretrain saver
            self.saver = tf.train.Saver(max_to_keep=1)
            if predict_only: return

            # Training
            self.global_step = tf.train.create_global_step()
            if args.clip_gradient:
                optimizer = tf.contrib.opt.LazyAdamOptimizer(learning_rate=self.learning_rate, beta2=args.beta_2)
                gradients, variables = zip(*optimizer.compute_gradients(loss))
                gradients, gradient_norm = tf.clip_by_global_norm(gradients, args.clip_gradient)
                self.training = optimizer.apply_gradients(zip(gradients, variables), global_step=self.global_step)
            else:
                self.training = tf.contrib.opt.LazyAdamOptimizer(learning_rate=self.learning_rate, beta2=args.beta_2).minimize(loss, global_step=self.global_step)

            # Summaries
            summary_writer = tf.contrib.summary.create_file_writer(args.model, flush_millis=10 * 1000)
            with summary_writer.as_default(), tf.contrib.summary.record_summaries_every_n_global_steps(100):
                self.training_summaries = [
                    tf.contrib.summary.scalar("train/loss", loss),
                    tf.contrib.summary.scalar("train/lr", self.learning_rate)]
                if args.clip_gradient:
                    self.training_summaries.append(tf.contrib.summary.scalar("train/gradient_norm", gradient_norm))
                for tag in args.tags:
                    self.training_summaries.append(tf.contrib.summary.scalar(
                        "train/{}".format(tag),
                        tf.reduce_sum(tf.cast(tf.equal(self.tags[tag], self.predictions[tag]), tf.float32) * weights) /
                        weights_sum))
                if args.parse:
                    heads_acc = tf.reduce_sum(tf.cast(tf.equal(self.heads, tf.argmax(heads, axis=-1, output_type=tf.int32)),
                                                      tf.float32) * weights) / weights_sum
                    self.training_summaries.extend([tf.contrib.summary.scalar("train/heads_acc", heads_acc)])
                    deprels_acc = tf.reduce_sum(tf.cast(tf.equal(self.deprels, tf.argmax(deprels, axis=-1, output_type=tf.int32)),
                                                        tf.float32) * weights) / weights_sum
                    self.training_summaries.extend([tf.contrib.summary.scalar("train/deprels_acc", deprels_acc)])

            # Evaluation summaries
            self.summary_writers = {"": summary_writer}
            for dataset in devs + tests:
                if dataset.label not in self.summary_writers:
                    self.summary_writers[dataset.label] = tf.contrib.summary.create_file_writer(
                        os.path.join(args.model, dataset.label), flush_millis=10 * 1000)

            self.event = tf.placeholder(tf.string, [])
            self.event_summaries = {}
            for name, writer in self.summary_writers.items():
                with writer.as_default():
                    self.event_summaries[name] = tf.contrib.summary.import_event(self.event)
            self.summary_writers_close = [writer.close() for writer in self.summary_writers.values()]

            # Initialize variables
            self.session.run(tf.global_variables_initializer())
            for summary_writer in self.summary_writers.values():
                with summary_writer.as_default():
                    tf.contrib.summary.initialize(session=self.session)

    def load(self, path, morphodita_dictionary=None):
        # We use the following version instead of calling `self.saver.restore`,
        # because it works even TF 2 is in Eager mode.
        self.session.run(self.saver.saver_def.restore_op_name,
                         {self.saver.saver_def.filename_tensor_name: os.path.join(path, "weights")})

        # Try loading also consistent feats table.
        consistent_feats_table = os.path.join(path, "consistent_feats.table")
        if os.path.exists(consistent_feats_table):
            import gzip
            with gzip.open(consistent_feats_table, "rb") as consistent_feats_table_file:
                consistent_feats_table = np.load(consistent_feats_table_file)

            with self.session.graph.as_default():
                consistent_feats_table = tf.convert_to_tensor(consistent_feats_table, dtype=tf.float32)
                self.predictions["FEATS"] = tf.argmax(
                    tf.nn.softmax(self.predictions_logits["FEATS"], axis=2) * tf.gather(consistent_feats_table, self.predictions["UPOS"]),
                    axis=2, output_type=tf.int32)

        # Load MorphoDiTa if requested
        if morphodita_dictionary:
            import ufal.morphodita
            self.morphodita = ufal.morphodita.Morpho.load(os.path.join(path, morphodita_dictionary))
            assert "XPOS" in self.tags and "LEMMAS" in self.tags, "MorphoDiTa dictionary operates on XPOS and LEMMAS, which are not present."

    def close_writers(self):
        self.session.run(self.summary_writers_close)

    def train_epoch(self, train, learning_rate, args):
        batches, at_least_one_epoch = 0, False
        while batches < args.min_epoch_batches:
            while not train.epoch_finished():
                sentence_lens, word_ids, charseq_ids, charseqs, charseq_lens = train.next_batch(args.batch_size)
                if args.word_dropout:
                    mask = np.random.binomial(n=1, p=args.word_dropout, size=word_ids[train.FORMS].shape)
                    word_ids[train.FORMS] = (1 - mask) * word_ids[train.FORMS] + mask * train.factors[train.FORMS].words_map["<unk>"]
                if args.char_dropout:
                    mask = np.random.binomial(n=1, p=args.char_dropout, size=charseqs[train.FORMS].shape)
                    charseqs[train.FORMS] = (1 - mask) * charseqs[train.FORMS] + mask * train.factors[train.FORMS].alphabet_map["<unk>"]

                feeds = {self.is_training: True, self.learning_rate: learning_rate, self.sentence_lens: sentence_lens,
                         self.charseqs: charseqs[train.FORMS], self.charseq_lens: charseq_lens[train.FORMS],
                         self.word_ids: word_ids[train.FORMS], self.charseq_ids: charseq_ids[train.FORMS]}
                if train.variants > 1:
                    feeds[self.variants] = word_ids[train.VARIANT]
                if train.embeddings_size:
                    if args.word_dropout:
                        mask = np.random.binomial(n=1, p=args.word_dropout, size=[*word_ids[train.EMBEDDINGS].shape[:2], 1])
                        word_ids[train.EMBEDDINGS] *= (1 - mask)
                    feeds[self.embeddings] = word_ids[train.EMBEDDINGS]
                for tag in args.tags: feeds[self.tags[tag]] = word_ids[train.FACTORS_MAP[tag]]
                if args.parse:
                    feeds[self.heads] = word_ids[train.HEAD]
                    feeds[self.deprels] = word_ids[train.DEPREL]
                self.session.run([self.training, self.training_summaries], feeds)
                batches += 1
                if at_least_one_epoch: break
            at_least_one_epoch = True

    def predict(self, dataset, evaluating, args):
        import io
        conllu, sentences = io.StringIO(), 0

        while not dataset.epoch_finished():
            sentence_lens, word_ids, charseq_ids, charseqs, charseq_lens = dataset.next_batch(args.batch_size)

            feeds = {self.is_training: False, self.sentence_lens: sentence_lens,
                     self.charseqs: charseqs[dataset.FORMS], self.charseq_lens: charseq_lens[dataset.FORMS],
                     self.word_ids: word_ids[dataset.FORMS], self.charseq_ids: charseq_ids[dataset.FORMS]}
            if dataset.variants > 1:
                feeds[self.variants] = word_ids[dataset.VARIANT]
            if dataset.embeddings_size:
                feeds[self.embeddings] = word_ids[dataset.EMBEDDINGS]
            if evaluating:
                for tag in args.tags: feeds[self.tags[tag]] = word_ids[dataset.FACTORS_MAP[tag]]
                if args.parse:
                    feeds[self.heads] = word_ids[dataset.HEAD]
                    feeds[self.deprels] = word_ids[dataset.DEPREL]

            targets = [self.predictions]
            if self.morphodita: targets.extend([self.predictions_logits["XPOS"], self.predictions_logits["LEMMAS"]])
            if args.parse: targets.extend([self.heads_logs, self.deprel_hidden_layer])
            predictions, *other_values = self.session.run(targets, feeds)
            if self.morphodita: xpos_logits, lemma_logits, *other_values = other_values
            if args.parse: prior_heads, deprel_hidden_layer, *other_values = other_values

            if args.parse:
                heads = np.zeros(prior_heads.shape[:2], dtype=np.int32)
                for i in range(len(sentence_lens)):
                    padded_heads = np.pad(prior_heads[i][:sentence_lens[i], :sentence_lens[i] + 1].astype(np.float64),
                                          ((1, 0), (0, 0)), mode="constant")
                    if args.single_root:
                        padded_heads[:, 0] = np.nan
                        padded_heads[1 + np.argmax(prior_heads[i][:sentence_lens[i], 0]), 0] = 0
                    chosen_heads, _ = ufal.chu_liu_edmonds.chu_liu_edmonds(padded_heads)
                    heads[i, :sentence_lens[i]] = chosen_heads[1:]
                deprels = self.session.run(self.predictions_deprel,
                                           {self.is_training: False, self.deprel_hidden_layer: deprel_hidden_layer, self.deprel_heads: heads})

            for i in range(len(sentence_lens)):
                overrides = [None] * dataset.FACTORS
                for tag in args.tags: overrides[dataset.FACTORS_MAP[tag]] = predictions[tag][i]
                if self.morphodita:
                    self.disambiguate_with_morphodita(
                        dataset.factors[dataset.FORMS].strings[sentences][1:], dataset, xpos_logits[i], lemma_logits[i], overrides)
                if args.parse:
                    overrides[dataset.HEAD] = heads[i]
                    overrides[dataset.DEPREL] = deprels[i]
                dataset.write_sentence(conllu, sentences, overrides)
                sentences += 1

        return conllu.getvalue()

    def disambiguate_with_morphodita(self, forms, dataset, tag_logits, lemma_logits, overrides):
        import ufal.morphodita
        tags_map = dataset.factors[dataset.XPOS].words_map
        lemma_rules_map = dataset.factors[dataset.LEMMAS].words_map
        overrides[dataset.XPOS] = overrides[dataset.XPOS].tolist()
        overrides[dataset.LEMMAS] = overrides[dataset.LEMMAS].tolist()

        analyses = ufal.morphodita.TaggedLemmas()
        for i in range(len(forms)):
            if self.morphodita.analyze(forms[i], self.morphodita.NO_GUESSER, analyses) < 0:
                continue

            if len(analyses) == 1:
                overrides[dataset.XPOS][i] = analyses[0].tag
                overrides[dataset.LEMMAS][i] = analyses[0].lemma
                continue

            lemmas = {}
            for analysis in analyses:
                tag_id = tags_map.get(analysis.tag, None)
                if tag_id is None:
                    continue
                stripped_lemma = self.morphodita.lemmaId(analysis.lemma)
                stripped_lemma_info = lemmas.get(stripped_lemma, None)
                if stripped_lemma_info is None:
                    lemmas[stripped_lemma] = analysis.lemma, tag_id
                else:
                    full_lemma, best_tag_id = stripped_lemma_info
                    if tag_logits[i, tag_id] > tag_logits[i, best_tag_id]:
                        lemmas[stripped_lemma] = full_lemma, tag_id

            if len(lemmas) == 1:
                lemma, best_tag_id = next(iter(lemmas.values()))
                overrides[dataset.XPOS][i] = best_tag_id
                overrides[dataset.LEMMAS][i] = lemma
            elif len(lemmas) > 1:
                best_tag_id, best_unknownlemma_tag_id = None, None
                for stripped_lemma, (full_lemma, tag_id) in lemmas.items():
                    lemma_rule = dataset._gen_lemma_rule(forms[i], stripped_lemma, dataset._lr_allow_copy)
                    lemma_rule_id = lemma_rules_map.get(lemma_rule, None)
                    if lemma_rule_id is None:
                        if best_unknownlemma_tag_id is None or tag_logits[i, tag_id] > tag_logits[i, best_unknownlemma_tag_id]:
                            best_unknownlemma_tag_id, best_unknownlemma = tag_id, full_lemma
                    else:
                        lemmatag_logits = lemma_logits[i, lemma_rule_id] + tag_logits[i, tag_id]
                        if best_tag_id is None or lemmatag_logits > best_lemmatag_logits:
                            best_tag_id, best_lemmatag_logits, best_lemma = tag_id, lemmatag_logits, full_lemma
                if best_tag_id is None or (best_unknownlemma_tag_id is not None and tag_logits[i, best_unknownlemma_tag_id] > tag_logits[i, best_tag_id] + 0.5):
                    best_tag_id, best_lemma = best_unknownlemma_tag_id, best_unknownlemma
                overrides[dataset.XPOS][i] = best_tag_id
                overrides[dataset.LEMMAS][i] = best_lemma

    def evaluate(self, dataset_name, dataset, args):
        import io

        conllu = self.predict(dataset.data, True, args)
        metrics = udpipe2_eval.evaluate(dataset.gold, udpipe2_eval.load_conllu(io.StringIO(conllu), args.single_root))
        event = [tf.Summary.Value(tag="{}/{}".format(dataset_name, metric), simple_value=metrics[metric].f1) for metric in self.METRICS]
        event = tf.Event(summary=tf.Summary(value=event), step=self.session.run(self.global_step), wall_time=time.time()).SerializeToString()
        self.session.run(self.event_summaries[dataset.label], {self.event: event})

        if args.parse:
            return (metrics["LAS"].f1 + metrics["MLAS"].f1 + metrics["BLEX"].f1) / 3., metrics
        else:
            return metrics["AllTags"].f1, metrics

    @staticmethod
    def argument_parser():
        parser = argparse.ArgumentParser()
        parser.add_argument("model", type=str, help="Model path")
        parser.add_argument("--batch_size", default=32, type=int, help="Batch size.")
        parser.add_argument("--beta_2", default=0.99, type=float, help="Adam beta 2")
        parser.add_argument("--char_dropout", default=0, type=float, help="Character dropout")
        parser.add_argument("--cle_dim", default=256, type=int, help="Character-level embedding dimension.")
        parser.add_argument("--clip_gradient", default=2.0, type=float, help="Gradient clipping.")
        parser.add_argument("--dev", default=[], nargs="+", type=str, help="Dev files.")
        parser.add_argument("--dropout", default=0.5, type=float, help="Dropout")
        parser.add_argument("--epochs", default="40:1e-3,20:1e-4", type=str, help="Epochs and learning rates.")
        parser.add_argument("--exp", default=None, type=str, help="Experiment name.")
        parser.add_argument("--label_smoothing", default=0.03, type=float, help="Label smoothing.")
        parser.add_argument("--max_sentence_len", default=120, type=int, help="Max sentence length.")
        parser.add_argument("--morphodita", default=None, type=str, help="MorphoDiTa dictionary used for PDT-C prediction.")
        parser.add_argument("--min_epoch_batches", default=300, type=int, help="Minimum number of batches per epoch.")
        parser.add_argument("--parse", default=1, type=int, help="Parse.")
        parser.add_argument("--parser_layers", default=1, type=int, help="Parser layers.")
        parser.add_argument("--parser_deprel_dim", default=128, type=int, help="Parser deprel dim.")
        parser.add_argument("--predict", default=False, const=True, nargs="?", type=str, help="Only predict.")
        parser.add_argument("--predict_input", default=None, type=str, help="Input to prediction.")
        parser.add_argument("--predict_output", default=None, type=str, help="Output to prediction.")
        parser.add_argument("--rnn_cell", default="LSTM", type=str, help="RNN cell type.")
        parser.add_argument("--rnn_cell_dim", default=512, type=int, help="RNN cell dimension.")
        parser.add_argument("--rnn_layers", default=2, type=int, help="RNN layers.")
        parser.add_argument("--rnn_layers_parser", default=1, type=int, help="Parser RNN layers.")
        parser.add_argument("--rnn_layers_tagger", default=0, type=int, help="Tagger RNN layers.")
        parser.add_argument("--seed", default=42, type=int, help="Initial random seed.")
        parser.add_argument("--single_root", default=1, type=int, help="Single root allowed only.")
        parser.add_argument("--tags", default="UPOS,XPOS,FEATS,LEMMAS", type=str, help="Tags.")
        parser.add_argument("--tag_layers", default=1, type=int, help="Additional tag layers.")
        parser.add_argument("--test", default=[], nargs="+", type=str, help="Test files.")
        parser.add_argument("--train", default=None, type=str, help="Trainig file.")
        parser.add_argument("--threads", default=4, type=int, help="Maximum number of threads to use.")
        parser.add_argument("--variant_dim", default=128, type=int, help="Variant embedding dimension.")
        parser.add_argument("--we_dim", default=512, type=int, help="Word embedding dimension.")
        parser.add_argument("--wembedding_model", default="bert-base-multilingual-uncased-last4", type=str, help="WEmbedding model.")
        parser.add_argument("--word_dropout", default=0.2, type=float, help="Word dropout")
        return parser

    @staticmethod
    def postprocess_arguments(args):
        # Add option defaults if missing in `args`
        args = UDPipe2.argument_parser().parse_args([args.model], namespace=args)

        # Manual args post-processing
        args.tags = args.tags.split(",")
        args.epochs = [(int(epochs), float(lr)) for epochs, lr in (epochs_lr.split(":") for epochs_lr in args.epochs.split(","))]


if __name__ == "__main__":
    import collections
    import glob
    import json

    # Parse arguments
    parser = UDPipe2.argument_parser()
    args = parser.parse_args()

    # Fix random seed
    np.random.seed(args.seed)

    if not args.predict:
        # Create model dir and save the passed options
        os.makedirs(args.model, exist_ok=True)
        with open(os.path.join(args.model, "options.json"), mode="w") as options_file:
            json.dump(vars(args), options_file, sort_keys=True)
    else:
        # Load saved options from the model
        with open(os.path.join(args.model, "options.json"), mode="r") as options_file:
            args = argparse.Namespace(**json.load(options_file))
        parser.parse_args(namespace=args)

    # Postprocess args
    UDPipe2.postprocess_arguments(args)

    # Load the data
    devs, tests = [], []
    EvaluationDataset = collections.namedtuple("EvaluationDataset", ["label", "data", "gold"])
    if not args.predict:
        train = udpipe2_dataset.UDPipe2Dataset(path=args.train, max_sentence_len=args.max_sentence_len, shuffle_batches=True,
                                               embeddings=glob.glob("{}*.npz".format(args.train)))
        train.save_mappings(os.path.join(args.model, "mappings.pickle"))
        for sources, target in [(args.dev, devs), (args.test, tests)]:
            for source in sources:
                label, path = ("", source) if ":" not in source else source.split(":", maxsplit=1)
                target.append(EvaluationDataset(
                    label,
                    udpipe2_dataset.UDPipe2Dataset(path=path, train=train, shuffle_batches=False,
                                                   embeddings=glob.glob("{}*.npz".format(path))),
                    udpipe2_eval.load_conllu_file(path, args.single_root)
                ))
    else:
        train = udpipe2_dataset.UDPipe2Dataset.load_mappings(os.path.join(args.model, "mappings.pickle"))
        test = udpipe2_dataset.UDPipe2Dataset(path=args.predict_input, train=train, shuffle_batches=False,
                                              embeddings=glob.glob("{}*.npz".format(args.predict_input)))

    # Construct the network
    network = UDPipe2(threads=args.threads, seed=args.seed)
    network.construct(args, train, devs, tests, predict_only=args.predict)

    if args.predict:
        def predict_mode(*modes):
            return isinstance(args.predict, str) and any(mode in args.predict.lower() for mode in modes)
        network.load(args.model, None if predict_mode("no_tagging", "no_morphodita") else args.morphodita)
        args.tags = [] if predict_mode("no_tagging") else args.tags
        args.parse = 0 if predict_mode("no_parsing") else args.parse
        conllu = network.predict(test, False, args)
        with open(args.predict_output, "w", encoding="utf-8") as output_file:
            print(conllu, end="", file=output_file)
    else:
        log_files = [open(os.path.join(args.model, "log"), "w", encoding="utf-8"), sys.stderr]
        for log_file in log_files:
            for tag in args.tags + ["DEPREL"]:
                print("{}: {}".format(tag, len(train.factors[train.FACTORS_MAP[tag]].words)), file=log_file, flush=True)
            print("VARIANT: {}".format(train.variants), file=log_file, flush=True)
            print("Parsing with args:", *["{}: {}".format(key, value) for key, value in sorted(vars(args).items())],
                  sep="\n", file=log_file, flush=True)

        for i, (epochs, learning_rate) in enumerate(args.epochs):
            for epoch in range(epochs):
                network.train_epoch(train, learning_rate, args)

                for dev in devs:
                    dev_accuracy, metrics = network.evaluate("dev", dev, args)
                    metrics_log = ", ".join(("{}: {:.2f}".format(metric, 100 * metrics[metric].f1) for metric in UDPipe2.METRICS))
                    for log_file in log_files:
                        print("Dev {} epoch {}, lr {}, {}".format(dev.label, epoch + 1, learning_rate, metrics_log), file=log_file, flush=True)

        for test in tests:
            test_accuracy, metrics = network.evaluate("test", test, args)
            metrics_log = ", ".join(("{}: {:.2f}".format(metric, 100 * metrics[metric].f1) for metric in UDPipe2.METRICS))
            for log_file in log_files:
                print("Test {} epoch {}, lr {}, {}".format(test.label, epoch + 1, learning_rate, metrics_log), file=log_file, flush=True)
        network.close_writers()

        network.saver.save(network.session, os.path.join(args.model, "weights"), write_meta_graph=False)
