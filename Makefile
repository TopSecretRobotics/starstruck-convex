.PHONY: all app clean deps flash lsusb

# Verbosity.

V ?= 0

verbose_0 = @
verbose_2 = set -x;
verbose = $(verbose_$(V))

# Platform detection.

ifeq ($(PLATFORM),)
ifeq ($(OS),Windows_NT)
PLATFORM = windows
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
PLATFORM = linux
else ifeq ($(UNAME_S),Darwin)
PLATFORM = darwin
else
$(error Unable to detect platform. Please open a ticket with the output of uname -a)
endif
endif
export PLATFORM
endif

ifeq ($(PLATFORM),darwin)
CORTEXFLASH ?= $(CURDIR)/tools/cortexflash.darwin
else ifeq ($(PLATFORM),linux)
CORTEXFLASH ?= $(CURDIR)/tools/cortexflash.linux
else ifeq ($(PLATFORM),windows)
CORTEXFLASH ?= $(CURDIR)/tools/cortexflash.exe
endif

# Configuration.

DEPS_DIR ?= $(CURDIR)/deps
export DEPS_DIR

CONVEX_DIR ?= $(CURDIR)/convex
export CONVEX_DIR

# Core targets.

all:: app

clean::
	$(MAKE) -C src clean

deps::

app::
	$(MAKE) -C src

VEX_DEVICE ?= $(word 1, $(shell (pros lsusb | grep -i vex | head -n 1)))

ifeq ($(VEX_DEVICE),)

flash::
	$(error No device found. Connect USB device or specify with VEX_DEVICE environment variable)

else

flash::
	-$(verbose) $(CORTEXFLASH) -X -w $(CURDIR)/bin/output.hex -v -g 0x0 $(VEX_DEVICE)

endif

lsusb::
	$(verbose) pros lsusb
