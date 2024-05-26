CC:=$(TOOLCHAIN_DIR)/bin/$(TARGET_TRIPLET)-gcc
AR:=$(TOOLCHAIN_DIR)/bin/$(TARGET_TRIPLET)-ar

LIBK_CFLAGS=$(CFLAGS) -D__is_libk

CFLAGS?=-O2 -g
LDFLAGS?=
LIBS?=

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -D__is_libc -Iinclude -I$(BASEDIR)/kernel/include
LIBK_CFLAGS:=$(CFLAGS) -D__is_libk

FREEOBJS=\
	 stdio/printf.o\
	 stdio/putchar.o\
	 stdio/puts.o\
	 stdlib/abort.o\
	 string/memcmp.o\
	 string/memcpy.o\
	 string/memmove.o\
	 string/memset.o\
	 string/strlen.o\

OBJS=\
     $(FREEOBJS)\

LIBK_OBJS=$(FREEOBJS:.o=.libk.o)

BINARIES=libk.a

.SUFFIXES: .o .libk.o .c .s
.PHONY: all .c.o .s.o .c.libk.o .s.libk.o clean

all: $(BINARIES)

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu99 $(CFLAGS)

.s.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS)

.c.libk.o:
	$(CC) -MD -c $< -o $@ -std=gnu99 $(LIBK_CFLAGS)

.s.libk.o:
	$(CC) -MD -c $< -o $@ $(LIBK_CFLAGS)

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) $(LIBK_OBJS.o=.d) *.d */*.d */*/*.d

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
