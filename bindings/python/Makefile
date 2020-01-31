# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../common/Makefile.common

BINDING_MODULE=$(if $(filter osx-%,$(PLATFORM)),ufal/_udpipe.so,$(call dynlib,ufal/_udpipe))
BINDING_C_FLAGS+=$(if $(PYTHON_INCLUDE),,$(error PYTHON_INCLUDE must be set to the directory containing Python.h))
BINDING_C_FLAGS+=$(call include_dir,$(PYTHON_INCLUDE))
BINDING_C_FLAGS+=$(if $(filter %-gcc %-clang,$(PLATFORM)),-w)

all: $(BINDING_MODULE)

$(call dynobj,udpipe_python):C_FLAGS+=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,udpipe_python) $(BINDING_UDPIPE_OBJECTS)
	$(call link_dynlib,$@,$^ $(BINDING_C_FLAGS), $(BINDING_LD_FLAGS))

udpipe_python.cpp: udpipe_python.i ../common/udpipe.i
	@$(call mkdir,ufal)
	@$(call echo,>ufal/__init__.py)
	swig $(SWIG_FLAGS) -python -outdir ufal -o $@ -builtin $<

.PHONY: clean
clean:
	@$(call rm,.build ufal $(call all_dynlib,ufal_udpipe) udpipe_python.cpp)
