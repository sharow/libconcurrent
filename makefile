#
#  libconcurrent
#  Copyright (C) 2010-2013 MIURA Shirow (sharow)
#

CC=$(PREFIX)gcc
LD=$(PREFIX)ld
NASM=nasm
UNAME=uname

.SUFFIXES: .c .a .o .h .asm
.PHONY: clean help
.PHONY: win32_dll win32_lib
.PHONY: gen_arch_objs
.PHONY: release_win32_gcc release_arch_objs release_source
.PHONY: sample
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

ifeq ($(ARCH),x86_64)
LDFLAGS_SHARED+=-entry=DllMainCRTStartup@12
else
LDFLAGS_SHARED+=-entry=_DllMainCRTStartup@12
endif
LDFLAGS_SHARED+=-shared
LDFLAGS_SHARED+=--enable-auto-image-base
LDFLAGS_SHARED+=--dll
LDFLAGS_SHARED+=--no-seh
LDFLAGS_SHARED+=--subsystem=windows
LDFLAGS_SHARED+=--out-implib=libconcurrent.dll.a
LDFLAGS_SHARED+=--output-def=libconcurrent.def

TARGET_STATIC=libconcurrent.a
TARGET_DYNAMIC=libconcurrent.dll
INCDIR+=-I./
INCDIR+=-I./include
INCDIR+=-I./source

SOURCE_ARCH=source/arch/$(ARCH)/concurrent_arch.asm
OBJECT_ARCH=source/arch/$(ARCH)/concurrent_arch.o

SOURCE+=source/concurrent.c
OBJECT=$(subst .c,.o, $(SOURCE))
OBJECT+=$(OBJECT_ARCH)

all: $(TARGET_STATIC)

win32_dll: $(TARGET_DYNAMIC)

win32_lib: $(TARGET_STATIC)

sample: $(TARGET_STATIC)
	make -C sample

$(TARGET_STATIC): $(OBJECT)
	$(AR) crv $(TARGET_STATIC) $(OBJECT)

$(TARGET_DYNAMIC): $(OBJECT)
	$(CC) -c $(CFLAGS) win32_dll_main_crt_startup.c
	$(LD) -s $(LDFLAGS) $(LDFLAGS_SHARED) $(OBJECT) -o $(TARGET_DYNAMIC) win32_dll_main_crt_startup.o

help:
	@echo "clean help win32_dll win32_lib release_win32_gcc sample"

install: $(TARGET_STATIC)
	install -Dm644 -s libconcurrent.a $(DESTDIR)/usr/lib/libconcurrent.a
	install -Dm644 include/concurrent/concurrent.h $(DESTDIR)/usr/include/concurrent/concurrent.h
	install -Dm644 include/concurrent/short_lower_case_api.h $(DESTDIR)/usr/include/concurrent/short_lower_case_api.h

clean:
	@make -C sample clean
	@rm -f $(OBJECT)
	@rm -f *~
	@rm -f makefile~
	@rm -f source/*~
	@rm -f source/arch/i386/*~
	@rm -f include/concurrent/*~
	@rm -f win32_dll_main_crt_startup.o
	@rm -f $(TARGET_DYNAMIC) $(TARGET_STATIC)
	@rm -f libconcurrent.def libconcurrent.dll.a
	@rm -rf arch_objs

release_win32_gcc: clean win32_lib win32_dll
	mkdir -p lib
	cp -f libconcurrent.a lib/
	mkdir -p dll
	cp -f libconcurrent.dll dll/
	cp -f libconcurrent.dll.a dll/
	cp -f libconcurrent.def dll/
	cp -f ./concurrent_build_config.h dll/
	tar -czf libconcurrent-win32-gcc-$(VERSION).tar.gz LICENSE include lib dll
	rm -rf ./lib ./dll

ARCH_SOURCE_i686=source/arch/i686/concurrent_arch.asm
ARCH_SOURCE_x86_64=source/arch/x86_64/concurrent_arch.asm

release_arch_objs: gen_arch_objs
	tar -czf libconcurrent-arch_objs-$(VERSION).tar.gz arch_objs LICENSE

release_source:
	hg archive -t tgz libconcurrent-$(VERSION).tar.gz -p libconcurrent-$(VERSION)

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

