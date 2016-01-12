#
#  libconcurrent
#  Copyright (C) 2010-2011 MIURA Shirow (sharow)
#

.SUFFIXES: .c .o .h .exe
.PHONY: all clean

CFLAGS+=-Wall
CFLAGS+=-std=c11

ifeq ($(DEBUG),yes)
 CFLAGS+=-g -ggdb
else
 CFLAGS+=-O2
endif

EXT= #
ifeq ($(shell uname -o),Cygwin)
 EXT=.exe
endif

INCDIR+=-I./
INCDIR+=-I../include

SOURCE+=concurrent_sort1.c
SOURCE+=coroutine1.c
SOURCE+=coroutine2.c
SOURCE+=accumlator.c
SOURCE+=many_context1.c
SOURCE+=float1.c
SOURCE+=float2.c
SOURCE+=time_slice1.c
OBJECT=$(subst .c,.o, $(SOURCE))
EXE=$(subst .c,.exe, $(SOURCE))

all: $(EXE)

clean:
	rm -f $(EXE)
	rm -f $(subst .c,$(EXT),$(SOURCE))
	rm -f *~

# suffix rule
.c.exe:
	$(CC) $(CFLAGS) $(INCDIR) $< -o $(subst .c,$(EXT),$<) ../libconcurrent.a
