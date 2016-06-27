#!/bin/sh

# Prepare API documentation and examples
make -C ../../doc manual_bindings_api.txt >/dev/null

cat <<"EOF"
=encoding utf-8

=head1 NAME

Ufal::UDPipe - bindings to UDPipe library L<http://ufal.mff.cuni.cz/udpipe>.

=head1 SYNOPSIS

  use Ufal::UDPipe;
  use open qw(:std :utf8);

  my $model_file = '...';
  my $model = Ufal::UDPipe::Model::load($model_file)
     or die "Cannot load model from file '$model_file'\n";

  my $tokenizer = $model->newTokenizer($Ufal::UDPipe::Model::DEFAULT);
  my $conllu_output = Ufal::UDPipe::OutputFormat::newOutputFormat("conllu");
  my $sentence = Ufal::UDPipe::Sentence->new();

  $tokenizer->setText(join('', <>));
  while ($tokenizer->nextSentence($sentence)) {
    $model->tag($sentence, $Ufal::UDPipe::Model::DEFAULT);
    $model->parse($sentence, $Ufal::UDPipe::Model::DEFAULT);

    my $output = $conllu_output->writeSentence($sentence);
    print $output;
  }

=head1 REQUIREMENTS

To compile the module, a C++11 compiler is needed, either C<g++> 4.7 or newer,
alternatively C<clang> 3.2 or newer or C<Visual C++> 2015.

=head1 DESCRIPTION

C<Ufal::UDPipe> is a Perl binding to UDPipe library L<http://ufal.mff.cuni.cz/udpipe>.

The binding is a straightforward conversion of the C++ bindings API.
Vectors do not have native Perl interface, see L<Ufal::UDPipe::Words>
source for reference. Static methods and enumerations are available only
through the module, not through object instance.

=head2 Wrapped C++ API

The C++ API being wrapped follows. For a API reference of the original
C++ API, see L<http://ufal.mff.cuni.cz/udpipe/api-reference>.

EOF
tail -n+4 ../../doc/manual_bindings_api.txt | sed 's/^/  /'
cat <<EOF

=head1 Examples

=head2 run_udpipe

A simple pipeline loading data from the specified input format
(vertical, horizontal or conllu), tagging, parsing
and writing to the specified output format.

EOF
sed '1,/^$/d' ../../bindings/perl/examples/run_udpipe.pl | sed 's/^/  /'
cat <<EOF

=head2 no_segmentation

The example in synopsis does both segmentation to sentences and tokenization
(plus tagging and parsing). Sometimes we have the input already segmented
one sentence per line. This is an adapted example which respects the input
sentence segmentation (but still does tokenization and allows untokenized
plain-text input, unlike the C<run_udpipe> example above).

EOF
sed '1,/^$/d' ../../bindings/perl/examples/no_segmentation.pl | sed 's/^/  /'
cat <<EOF

=head1 AUTHORS

Milan Straka <straka@ufal.mff.cuni.cz>

=head1 COPYRIGHT AND LICENCE

Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
Mathematics and Physics, Charles University in Prague, Czech Republic.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

=cut
EOF
