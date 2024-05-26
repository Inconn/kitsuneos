BASEDIR=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

SUBDIRS=kernel libc

ARCH=i386
TARGET_TRIPLET=i686-elf
TOOLCHAIN_DIR=$(BASEDIR)/toolchain/cross

export BASEDIR
export ARCH
export TARGET_TRIPLET
export TOOLCHAIN_DIR

.PHONY: all clean

all:
	$(MAKE) $(MFLAGS) -C libc
	$(MAKE) $(MFLAGS) -C kernel

clean:
	$(MAKE) $(MFLAGS) -C libc clean
	$(MAKE) $(MFLAGS) -C kernel clean

