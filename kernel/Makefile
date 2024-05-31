CC:=$(TOOLCHAIN_DIR)/bin/$(TARGET_TRIPLET)-gcc

CFLAGS?=-O2 -g
LDFLAGS?=
LIBS?=

CFLAGS:=$(CFLAGS) -std=gnu99 -ffreestanding -Wall -Wextra -D__is_kernel -Iinclude -I$(BASEDIR)/libc/include
LIBS:=$(LIBS) -L$(BASEDIR)/libc -nostdlib -lk -lgcc

ARCHDIR=arch/$(ARCH)

include $(ARCHDIR)/make.config

KERNEL_OBJS:=\
	     $(KERNEL_ARCH_OBJS)\
	     kernel/kernel.o\
	     kernel/buddy.o\
	     kernel/dumb_alloc.o\
	     kernel/test_user_fn.o\

CRTI_OBJ=$(ARCHDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=$(ARCHDIR)/crtn.o

OBJS=\
	$(ARCHDIR)/crti.o\
	$(ARCHDIR)/crtbegin.o\
	$(KERNEL_OBJS)\
	$(ARCHDIR)/crtend.o\
	$(ARCHDIR)/crtn.o\

LINK_LIST=\
	$(LDFLAGS)\
	$(ARCHDIR)/crti.o\
        $(ARCHDIR)/crtbegin.o\
	$(KERNEL_OBJS)\
	$(LIBS)\
	$(ARCHDIR)/crtend.o\
	$(ARCHDIR)/crtn.o\

.SUFFIXES: .o .c. s
.PHONY: all .c.o .s.o clean

all: kitsuneos.bin

kitsuneos.bin: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o kitsuneos.bin $(CFLAGS) $(LINK_LIST)

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu99 $(CFLAGS)

.s.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS)

clean:
	rm -f kitsuneos.bin
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

-include $(OBJS:=.o=.d)
