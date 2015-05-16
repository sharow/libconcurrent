#
#  libconcurrent
#  Copyright (C) 2010-2014 MIURA Shirow (sharow)
#

CC=$(PREFIX)gcc
LD=$(PREFIX)ld
NASM=nasm
UNAME=uname

.SUFFIXES: .c .a .o .h .asm
.PHONY: clean help
.PHONY: win32_lib
.PHONY: sample test
.PHONY: install
.VPATH: ./ ./source ./source/arch/i386 ./source/arch/x86_64

VERSION=0.3.0

ifeq ($(ARCH),)
 ARCH=$(shell $(UNAME) -m)
endif

ifeq ($(SYSTEM),)
 SYSTEM=$(shell $(UNAME) -o)
endif

NASM_FLAGS=
ifeq ($(SYSTEM),Cygwin)
 NASM_FLAGS+=-f win$(ARCH_BITS)
 ifeq ($(ARCH),i686)
  NASM_FLAGS+=--prefix _
 endif
else ifeq ($(SYSTEM),Msys)
 NASM_FLAGS+=-f win$(ARCH_BITS)
 ifeq ($(ARCH),i686)
  NASM_FLAGS+=--prefix _
 endif
else ifeq ($(SYSTEM),GNU/Linux)
 NASM_FLAGS+=-f elf$(ARCH_BITS)
else
 NASM_FLAGS+=-f elf$(ARCH_BITS)
endif

AS=$(NASM)
ASFLAGS=$(NASM_FLAGS)

ifeq ($(ARCH),i686)
 ARCH_BITS=32
else ifeq ($(ARCH),x86_64)
 ARCH_BITS=64
else ifeq ($(ARCH),armv6l)
 ARCH=arm
 ARCH_BITS=32
 AS=as
 ASFLAGS=
else ifeq ($(ARCH),armv7l)
 ARCH=arm
 ARCH_BITS=32
 AS=as
 ASFLAGS=
else
 ARCH_BITS=32
endif


ifeq ($(DESTDIR),)
  DESTDIR=./pkg
endif

CFLAGS+=-DCONCURRENT_HAS_BUILD_CONFIG # ./concurrent_build_config.h
CFLAGS+=-DCONCURRENT_ARCH_NAME='"$(ARCH)"'
CFLAGS+=-Wall
CFLAGS+=-std=c99
CFLAGS+=-Wstrict-aliasing

ifeq ($(DEBUG),yes)
 CFLAGS+=-DCONCURRENT_DEBUG
 CFLAGS+=-g -ggdb
else
 CFLAGS+=-O2
endif

LDFLAGS+=--version-exports-section="$(VERSION)"


TARGET_STATIC=libconcurrent.a
INCDIR+=-I.
INCDIR+=-I./include
INCDIR+=-I./source

SOURCE_ARCH=source/arch/$(ARCH)/concurrent_arch.asm
OBJECT_ARCH=source/arch/$(ARCH)/concurrent_arch.o

SOURCE+=source/concurrent.c
OBJECT=$(subst .c,.o, $(SOURCE))
OBJECT+=$(OBJECT_ARCH)

all: $(TARGET_STATIC)

win32_lib: $(TARGET_STATIC)

sample: $(TARGET_STATIC)
	make -C sample

test: $(TARGET_STATIC)
	make -C test

$(TARGET_STATIC): $(OBJECT)
	$(AR) crv $(TARGET_STATIC) $(OBJECT)

help:
	@echo "clean help win32_lib sample"

install: $(TARGET_STATIC)
	install -Dm644 libconcurrent.a $(DESTDIR)/usr/lib/libconcurrent.a
	install -Dm644 include/concurrent/concurrent.h $(DESTDIR)/usr/include/concurrent/concurrent.h
	install -Dm644 include/concurrent/short_lower_case_api.h $(DESTDIR)/usr/include/concurrent/short_lower_case_api.h

clean:
	@make -C sample clean
	@make -C test clean
	@rm -f $(OBJECT)
	@rm -f $(TARGET_STATIC)

# suffix rule
.c.o:
	$(CC) $(CFLAGS) $(DEFS) $(INCDIR) -c $< -o $@

.asm.o:
	$(AS) $(ASFLAGS) $< -o $@

#depend
depend:
	$(RM) -f depend.inc
	$(CC) $(CFLAGS) $(INCDIR) -MM $(SOURCE) > depend.inc

concurrent_arch.o: concurrent_arch.asm

include depend.inc

