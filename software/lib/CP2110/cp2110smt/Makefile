###########################################
# Makefile for cp2110smtsmt
# Requires libusb-1.0
# Requires libuuid
# Requires libcp210xmanufacturing, libslabhidtouart
#   libslabhidtosmbus, libslabhiddevice, libslab_usb_spi
#
# Silicon Labs
# Modified July 11th, 2016
###########################################

CPU_ARCH?=$(shell arch)
BUILD ?=../build
BUILDPATH ?=$(BUILD)/bin/$(CPU_ARCH)
OBJPATH ?=$(BUILD)/obj/$(CPU_ARCH)

# required packages
PKG_CONFIG_DEPENDENCIES = libusb-1.0

CSRCS =
CPPSRCS  = cp2110smt.cpp
CPPSRCS += smt.cpp 
CPPSRCS += util.cpp
CPPSRCS += OsDep.cpp

COBJS=$(CSRCS:%.c=%.o)
CPPOBJS=$(CPPSRCS:%.cpp=%.o)

OBJFILES = $(COBJS) $(CPPOBJS)

OBJFULLPATHS=$(addprefix $(OBJPATH)/, $(OBJFILES))

EXENAME = cp2110smt
EXEFULLPATH = $(BUILDPATH)/$(EXENAME)

LIBS	  += -lcp210xmanufacturing
LIBS	  += -lslabhidtouart
LIBS	  += -lslabhidtosmbus
LIBS	  += -lslabhiddevice
LIBS	  += -lslab_usb_spi
LIBS	  += -luuid
LIBS      += -lpthread
LIBS      += $(foreach deplib,$(PKG_CONFIG_DEPENDENCIES), `pkg-config --libs $(deplib)`)


CC       ?= gcc
CFLAGS   ?= -Wall -fPIC -g $(INCLUDES)

CXX      ?= g++
CXXFLAGS ?= -Wall -fPIC -g $(INCLUDES)

all: $(OBJPATH) $(BUILDPATH) $(EXEFULLPATH)

$(OBJPATH):
	mkdir -p $@

$(BUILDPATH):
	mkdir -p $@

$(OBJPATH)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(EXEFULLPATH): $(OBJFULLPATHS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) $(LIBPATHS) -o $@
	

clean:
	rm -rf $(BUILD)


