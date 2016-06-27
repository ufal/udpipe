# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

use strict;
use warnings;
use Ufal::UDPipe;
use open qw(:std :utf8);

@ARGV == 1 or die "Usage: $0 model_file < in.txt > out.conllu\n";
my $model_file = shift @ARGV;
my $model = Ufal::UDPipe::Model::load($model_file)
  or die "Cannot load model from file '$model_file'\n";

my $tokenizer = $model->newTokenizer($Ufal::UDPipe::Model::DEFAULT);
my $conllu_output = Ufal::UDPipe::OutputFormat::newOutputFormat("conllu");
my $sentence = Ufal::UDPipe::Sentence->new();

while (<>){

  # Tokenize the current line (sentence) and save the result in $sentence.
  $tokenizer->setText($_);
  my $is_another = $tokenizer->nextSentence($sentence);

  # If UDPipe (falsely) detected more sentences on this line,
  # let's merge them into the first $sentence.
  if ($is_another) {
      my $words = $sentence->{words};
      my $multiwords = $sentence->{multiwordTokens};
      my $plus_index = $words->size - 1;
      my $sentence2 = Ufal::UDPipe::Sentence->new();
      while ($tokenizer->nextSentence($sentence2)) {
          # Copy words from $sentence2 to the first $sentence.
          # Skip $words2->get(0) which is always the technical root.
          my $words2 = $sentence2->{words};
          for my $i (1 .. $words2->size - 1) {
              my $word2 = $words2->get($i);
              $word2->{id} += $plus_index;
              $words->push($word2);
          }
          # Copy multiword tokens from $sentence2 to the first $sentence.
          my $multiwords2 = $sentence2->{multiwordTokens};
          for my $i (0 .. $multiwords2->size - 1) {
              my $multiword2 = $multiwords2->get($i);
              $multiword2->{idLast}  += $plus_index;
              $multiword2->{idFirst} += $plus_index;
              $multiwords->push($multiword2);
          }
      }
  }

  # Tag, parse and print the current line (saved in $sentence).
  $model->tag($sentence, $Ufal::UDPipe::Model::DEFAULT);
  $model->parse($sentence, $Ufal::UDPipe::Model::DEFAULT);
  print $conllu_output->writeSentence($sentence);
}
