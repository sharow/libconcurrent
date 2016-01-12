.SUFFIXES: .c .o .h
.PHONY: all clean

CFLAGS+=-Wall
CFLAGS+=-std=c11

ifeq ($(DEBUG),yes)
 CFLAGS+=-g -ggdb
else
 CFLAGS+=-O2
endif

EXT=out
ifeq ($(shell uname -o),Cygwin)
 EXT=exe
endif

INCDIR+=-I./
INCDIR+=-I../include

SOURCE+=stack_torture1.c
EXE=$(subst .c,.$(EXT), $(SOURCE))

all: $(EXE)
	@for X in $(EXE); do \
		echo run $$X; \
		exec ./$$X; \
	done

clean:
	rm -f $(EXE)
	rm -f *~

# suffix rule
.c.$(EXT):
	$(CC) $(CFLAGS) $(INCDIR) $< -o $(subst .c,.$(EXT),$<) ../libconcurrent.a
