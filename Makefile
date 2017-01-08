.PHONY: all app clean deps flash lsusb shell windocker

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

ifeq ($(PLATFORM),windows)

VEX_DEVICE ?= $(word 1, $(shell (pros lsusb | grep -i com | head -n 1)))

else

VEX_DEVICE ?= $(word 1, $(shell (pros lsusb | grep -i vex | head -n 1)))

endif

ifeq ($(VEX_DEVICE),)

flash::
	$(error No device found. Connect USB device or specify with VEX_DEVICE environment variable)

shell::
	$(error No device found. Connect USB device or specify with VEX_DEVICE environment variable)

else

flash::
	-$(verbose) "$(CORTEXFLASH)" -X -w "$(CURDIR)/bin/output.hex" -v -g 0x0 $(VEX_DEVICE)

ifeq ($(PLATFORM),windows)

shell::
	$(verbose) putty -serial $(VEX_DEVICE) -sercfg 115200

# $(error You will need to use PuTTY to connect to device $(VEX_DEVICE) at speed 115200)

else

shell::
	$(verbose) screen $(VEX_DEVICE) 115200

endif

endif

lsusb::
	$(verbose) pros lsusb

ifeq ($(PLATFORM),windows)

windocker::
	docker-compose build
	docker rm -f convex_project_data || true
	docker create -v /"$(shell pwd)":/build/project --name convex_project_data convex_project
	winpty docker run -it --rm --volumes-from convex_project_data convex_project

else

windocker::
	$(error This only works on Windows.)

endif
