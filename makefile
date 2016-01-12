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
.PHONY: examples test
.PHONY: install
.VPATH: ./ ./src ./src/arch/i386 ./src/arch/x86_64

VERSION=0.4.0

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


CFLAGS+=-Wall
CFLAGS+=-std=c11
CFLAGS+=-Wstrict-aliasing
CFLAGS+=-fno-stack-protector  # for Ubuntu gcc patch

ifeq ($(DEBUG),yes)
 CFLAGS+=-DCONCURRENT_DEBUG
 CFLAGS+=-g -ggdb
else
 CFLAGS+=-O2
endif

LDFLAGS+=--version-exports-section="$(VERSION)"


TARGET=libconcurrent.a
INCDIR+=-I.
INCDIR+=-I./include
INCDIR+=-I./src

SOURCE_ARCH=src/arch/$(ARCH)/concurrent_arch.asm
OBJECT_ARCH=src/arch/$(ARCH)/concurrent_arch.o

SOURCE+=src/concurrent.c
OBJECT+=$(subst .c,.o, $(SOURCE))
OBJECT+=$(OBJECT_ARCH)

all: $(TARGET)

examples: $(TARGET)
	make -C examples

test: $(TARGET)
	make -C test

$(TARGET): $(OBJECT)
	$(AR) crv $(TARGET) $(OBJECT)

help:
	@echo "clean help examples test"

install: $(TARGET)
	install -Dm644 libconcurrent.a $(DESTDIR)/usr/lib/libconcurrent.a
	install -Dm644 include/concurrent/concurrent.h $(DESTDIR)/usr/include/concurrent/concurrent.h
	install -Dm644 include/concurrent/shortname.h $(DESTDIR)/usr/include/concurrent/shortname.h

clean:
	@make -C examples clean
	@make -C test clean
	@rm -f $(OBJECT)
	@rm -f $(TARGET)

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

