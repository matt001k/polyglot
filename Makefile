###############################################################################
# Generic Variables
###############################################################################
CMAKE := cmake
MAKE := make

BUILD_DIR := build

ROOT_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR := $(dir $(ROOT_MAKEFILE))

###############################################################################
# OS Specific Variables
###############################################################################
ifeq ($(OS),Windows_NT)
	SYSTEM = Building on a Windows System
	GEN_NAME = "MinGW Makefiles"
	MKDIR := mkdir
	RM := rd /s /q
	DELIM := \\
else
	UNAME := $(shell uname -s)
	SYSTEM = Building on a Linux System
	GEN_NAME = "Unix Makefiles"
	MKDIR := mkdir
	RM := rm -fR
	DELIM := /
endif

###############################################################################
# Build Commands
###############################################################################
all:init
	@cd $(BUILD_DIR) && $(MAKE) --no-print-directory

init:$(BUILD_DIR)
ifeq ("$(wildcard $(BUILD_DIR)/CMakeCache.txt)","")
	@cd $(BUILD_DIR) && $(CMAKE) -G $(GEN_NAME) $(ROOT_DIR)
endif

$(BUILD_DIR):
	@echo $(SYSTEM)
ifeq ("$(wildcard $(BUILD_DIR))","")
	@$(MKDIR) $(BUILD_DIR)
endif

flash: flash_boot flash_app

flash_boot:all
	python scripts/flash.py ./build/ahriman_boot.bin 0x0000

flash_app:all
	python scripts/flash.py ./build/ahriman_app.bin 0x8000

clean:
	@$(RM) $(BUILD_DIR)
