# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../../src/Makefile.builtem

TOOLS=$(call exe,normalize_form)
all: $(TOOLS)

NORMALIZE_FORM_OBJECTS=normalize_form ../../src/unilib/utf8 ../../src/utils/options
NORMALIZE_FORM_OBJECTS+=$(foreach dir,morphodita/version parsito/version unilib version,../../src/$(dir)/version)
$(call obj,$(NORMALIZE_FORM_OBJECTS)): C_FLAGS+=$(call include_dir,../../src)
$(call exe,normalize_form): $(call obj,$(NORMALIZE_FORM_OBJECTS))
	$(call link_exe,$@,$^,$(call win_subsystem,console))

.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,$(TOOLS)))
