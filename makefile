#
#  libconcurrent
#  Copyright (C) 2010-2016 sharow
#

CC=$(PREFIX)gcc
LD=$(PREFIX)ld
NASM=nasm
UNAME=uname

.SUFFIXES: .c .a .o .h .asm
.PHONY: clean help
.PHONY: examples example sample test
.PHONY: install uninstall
.VPATH: ./ ./src ./src/arch/i386 ./src/arch/x86_64

VERSION=0.4.0

ifeq ($(ARCH),)
 ARCH=$(shell $(UNAME) -m)
endif

ifeq ($(SYSTEM),)
 SYSTEM=$(shell $(UNAME) -o)
endif

NASM_FLAGS=
ifeq ($(findstring true, $(eq $(SYSTEM),Cygwin), $(eq $(SYSTEM),MSys)), true)
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

# FreeBSD has its own uname -m style
ifeq ($(SYSTEM),FreeBSD)
 CC=cc
 LD=ld
 ifeq ($(ARCH),amd64)
  ARCH=x86_64
  ARCH_BITS=64
 else ifeq ($(ARCH),i386)
  ARCH=i686
  ARCH_BITS=32
 else ifeq ($(ARCH),arm) # arm not tested, just a guess
  ARCH_BITS=32
  AS=as
  ASFLAGS=
 endif
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


example: examples
sample: examples
examples: $(TARGET)
	$(MAKE) -C examples

test: $(TARGET)
	$(MAKE) -C test

$(TARGET): $(OBJECT)
	$(AR) crv $(TARGET) $(OBJECT)

help:
	@echo "make [clean|help|examples|test|install|uninstall]"

install: $(TARGET)
	install -Dm644 libconcurrent.a $(DESTDIR)/usr/lib/libconcurrent.a
	install -Dm644 include/concurrent/concurrent.h $(DESTDIR)/usr/include/concurrent/concurrent.h
	install -Dm644 include/concurrent/shortname.h $(DESTDIR)/usr/include/concurrent/shortname.h

uninstall:
	rm $(DESTDIR)/usr/lib/libconcurrent.a
	rm $(DESTDIR)/usr/include/concurrent/concurrent.h
	rm $(DESTDIR)/usr/include/concurrent/shortname.h
	rmdir $(DESTDIR)/usr/include/concurrent

clean:
	@$(MAKE) -C examples clean
	@$(MAKE) -C test clean
	@rm -f $(OBJECT)
	@rm -f $(TARGET)

# suffix rule
.c.o:
	$(CC) $(CFLAGS) $(DEFS) $(INCDIR) -c $< -o $@

.asm.o:
	$(AS) $(ASFLAGS) $< -o $@

depend:
	$(RM) -f depend.inc
	$(CC) $(CFLAGS) $(INCDIR) -MM $(SOURCE) > depend.inc

concurrent_arch.o: concurrent_arch.asm

include depend.inc

