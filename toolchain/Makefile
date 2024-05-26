TOOLCHAIN_PREFIX:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))/cross

toolchain:
	mkdir cross
	mkdir src
	cd src; git clone -b binutils-2_42 --single-branch --depth 1 https://sourceware.org/git/binutils-gdb.git
	cd src; git clone -b releases/gcc-14.1.0 --single-branch --depth 1 https://gcc.gnu.org/git/gcc.git
	mkdir build-binutils
	cd build-binutils; ../src/binutils-gdb/configure --target=i686-elf --prefix=$(TOOLCHAIN_PREFIX) --with-sysroot --disable-nls --disable-gdb --disable-gdbserver --disable-gold --disable-werror; $(MAKE); $(MAKE) install
	mkdir build-gcc
	cd build-gcc; ../src/gcc/configure --target=i686-elf --prefix=$(TOOLCHAIN_PREFIX) --disable-nls --enable-languages=c --without-headers; $(MAKE) all-gcc; $(MAKE) all-target-libgcc; $(MAKE) install-gcc; $(MAKE) install-target-libgcc

clean:
	rm -rf cross
	rm -rf src
	rm -rf build-binutils
	rm -rf build-gcc
