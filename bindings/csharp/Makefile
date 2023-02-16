# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../common/Makefile.common

BINDING_MODULE=$(call dynlib,$(if $(filter win-%,$(PLATFORM)),udpipe_csharp,libudpipe_csharp))
ifneq ($(filter win-gcc-%,$(PLATFORM)),)
  BINDING_LD_FLAGS+=-Wl,--kill-at
endif

all: $(BINDING_MODULE)

$(call dynobj,udpipe_csharp):C_FLAGS+=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,udpipe_csharp) $(BINDING_UDPIPE_OBJECTS)
	$(call link_dynlib,$@,$^ $(BINDING_C_FLAGS), $(BINDING_LD_FLAGS))

udpipe_csharp.cpp: udpipe_csharp.i ../common/udpipe.i
	@$(call mkdir,Ufal/UDPipe)
	swig $(SWIG_FLAGS) -csharp -namespace Ufal.UDPipe -outdir $(call platform_name,Ufal/UDPipe) -o $@ $<

.PHONY: clean
clean:
	@$(call rm,.build Ufal $(call all_dynlib,udpipe_csharp libudpipe_csharp) udpipe_csharp.cpp $(wildcard examples/*.class))
