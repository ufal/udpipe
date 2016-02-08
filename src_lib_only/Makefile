# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../src/Makefile.builtem
include ../src/Makefile.lib

MERGE=$(call exe,merge_sources/merge_sources)

udpipe.cpp: $(MERGE) force
	@$(call echo,>$@,// This file is part of UDPipe <http://github.com/ufal/udpipe/>.)
	@$(call echo,>>$@,//)
	@$(call echo,>>$@,// This file is a bundle of all sources and headers of UDPipe library.)
	@$(call echo,>>$@,// Comments and copyrights of all individual files are kept.)
	$(call platform_name,$(MERGE) ufal::udpipe ../src/ $(patsubst %,%.cpp,$(UDPIPE_O))) >>$@

$(MERGE):$(call exe,%):$$(call obj,%)
	$(call link_exe,$@,$^,$(call win_subsystem,console))

.PHONY: force
force:

.PHONY: clean
clean:
	@$(call rm,.build udpipe.cpp $(call all_exe,$(MERGE)))
