.PHONY: all app deps

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
$(error Unable to detect platform. Please open a ticket with the output of uname -a.)
endif
endif
export PLATFORM
endif

# Configuration.

DEPS_DIR ?= $(CURDIR)/deps
export DEPS_DIR

CONVEX_DIR ?= $(CURDIR)/convex
export CONVEX_DIR

# Core targets.

all:: deps app

ifneq ($(wildcard $(CONVEX_DIR)),)

deps:: 

else

deps:: $(CONVEX_DIR)

endif

$(CONVEX_DIR):
	$(verbose) git submodule update --init --recursive
	$(verbose) (test -d "$(CURDIR)/ChibiOS_2.6.2" || unzip -q "$(DEPS_DIR)/jpearman/convex/ChibiOS_2.6.2.zip" -d "$(CURDIR)")
	$(verbose) ln -svf "deps/jpearman/convex/convex" "$(CONVEX_DIR)"

app::
	$(MAKE) -C app
