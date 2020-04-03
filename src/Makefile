# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include Makefile.builtem
include Makefile.include
include rest_server/microrestd/Makefile.include

EXECUTABLES = $(call exe,udpipe)
SERVER = $(call exe,rest_server/udpipe_server)
TOOLS =
LIBRARIES = $(call lib,libudpipe)

.PHONY: all exe server tools lib full
all: exe
exe: $(EXECUTABLES)
server: $(SERVER)
tools: $(TOOLS)
lib: $(LIBRARIES)
full: exe server tools lib

# libraries
$(call lib,libudpipe): $(call obj,$(UDPIPE_OBJECTS))
	$(call link_lib,$@,$^)

C_FLAGS += $(call include_dir,.)

ISARM := 0
ifneq ($(filter linux-gcc linux-clang,$(PLATFORM)),)
  UNAME_A := $(shell uname -a)
  ifneq ($(filter arm%,$(UNAME_A)),)
    ISARM := 1
  endif
endif
ifeq ($(ISARM),1)
  LD_FLAGS += -latomic
endif

# executables
$(call exe,rest_server/udpipe_server): LD_FLAGS+=$(call use_library,$(if $(filter win-%,$(PLATFORM)),$(MICRORESTD_LIBRARIES_WIN),$(MICRORESTD_LIBRARIES_POSIX)))
$(call exe,rest_server/udpipe_server): $(call obj,rest_server/udpipe_service $(addprefix rest_server/microrestd/,$(MICRORESTD_OBJECTS) $(MICRORESTD_PUGIXML_OBJECTS)))
$(EXECUTABLES) $(SERVER) $(TOOLS):$(call exe,%): $$(call obj,% utils/options $(UDPIPE_OBJECTS))
	$(call link_exe,$@,$^,$(call win_subsystem,console))

# cleaning
.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,$(EXECUTABLES) $(SERVER) $(TOOLS)) $(call all_lib,$(LIBRARIES)))

# dump library sources
.PHONY: lib_sources
lib_sources:
	@echo $(patsubst %,%.cpp,$(UDPIPE_OBJECTS))
