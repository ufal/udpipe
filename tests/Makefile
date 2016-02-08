# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../src/Makefile.builtem

TESTS=$(call exe,udpipe_bundle)
all: $(TESTS)

.PHONY: force
../src_lib_only/udpipe.cpp: force
	$(MAKE) -C ../src_lib_only udpipe.cpp

$(call obj,udpipe_bundle): C_FLAGS+=$(call include_dir,../src_lib_only)
$(call exe,udpipe_bundle): $(call obj,udpipe_bundle ../src_lib_only/udpipe)
	$(call link_exe,$@,$^,$(call win_subsystem,console))

.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,$(TESTS)))
