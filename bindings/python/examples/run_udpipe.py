# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys

from ufal.udpipe import *

# In Python2, wrap sys.stdin and sys.stdout to work with unicode.
if sys.version_info[0] < 3:
    import codecs
    import locale
    encoding = locale.getpreferredencoding()
    sys.stdin = codecs.getreader(encoding)(sys.stdin)
    sys.stdout = codecs.getwriter(encoding)(sys.stdout)

if len(sys.argv) < 4:
    sys.stderr.write('Usage: %s input_format output_format model_file\n' % sys.argv[0])
    sys.exit(1)

sys.stderr.write('Loading model: ')
model = Model.load(sys.argv[3])
if not model:
    sys.stderr.write("Cannot load model from file '%s'\n" % sys.argv[3])
    sys.exit(1)
sys.stderr.write('done\n')

pipeline = Pipeline(model, sys.argv[1], Pipeline.DEFAULT, Pipeline.DEFAULT, sys.argv[2])
error = ProcessingError()

# Read whole input
text = ''.join(sys.stdin.readlines())

# Process data
processed = pipeline.process(text, error)
if error.occurred():
    sys.stderr.write("An error occurred when running run_udpipe: ")
    sys.stderr.write(error.message)
    sys.stderr.write("\n")
    sys.exit(1)
sys.stdout.write(processed)
