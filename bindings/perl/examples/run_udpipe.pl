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
use open qw(:std :utf8);

use Ufal::UDPipe;

@ARGV >= 3 or die "Usage: $0 input_format output_format model_file\n";
my $input = shift @ARGV;
my $output = shift @ARGV;
my $model_file = shift @ARGV;

print STDERR "Loading model: ";
my $model = Ufal::UDPipe::Model::load($model_file);
$model or die "Cannot load model from file '$model_file'\n";
print STDERR "done\n";

my $pipeline = Ufal::UDPipe::Pipeline->new(
                 $model, $input, $Ufal::UDPipe::Pipeline::DEFAULT,
                 $Ufal::UDPipe::Pipeline::DEFAULT, $output);
my $error = Ufal::UDPipe::ProcessingError->new();

for (my $not_eof = 1; $not_eof; ) {
  my $text = '';

  # Read block
  while (1) {
    my $line = <>;
    last unless ($not_eof = defined $line);
    $text .= $line;
    chomp($line);
    last unless length $line;
  }

  # Process data
  my $processed = $pipeline->process($text, $error);
  $error->occurred() and die "An error occurred in run_udpipe: " . $error->{message};
  print $processed;
}
