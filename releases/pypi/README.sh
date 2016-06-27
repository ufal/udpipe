#!/bin/sh

# Prepare API documentation and examples
make -C ../../../doc manual_bindings_api.txt >/dev/null

cat <<"EOF"
ufal.udpipe
============

The ``ufal.udpipe`` is a Python binding to UDPipe library <http://ufal.mff.cuni.cz/udpipe>.

The bindings is a straightforward conversion of the ``C++`` bindings API.
In Python 2, strings can be both ``unicode`` and UTF-8 encoded ``str``, and the
library always produces ``unicode``. In Python 3, strings must be only ``str``.


Wrapped C++ API
---------------

The C++ API being wrapped follows. For a API reference of the original
C++ API, see <http://ufal.mff.cuni.cz/udpipe/api-reference>.

::

EOF
tail -n+4 ../../doc/manual_bindings_api.txt | sed 's/^/  /'
cat <<EOF


Examples
========

run_udpipe
--------------

Simple pipeline loading data (tokenizing on request), tagging, parsing and
writing to specified output format::


EOF
sed '1,/^$/d' ../../bindings/python/examples/run_udpipe.py | sed 's/^/  /'
cat <<EOF


AUTHORS
=======

Milan Straka <straka@ufal.mff.cuni.cz>


COPYRIGHT AND LICENCE
=====================

Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
Mathematics and Physics, Charles University in Prague, Czech Republic.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
