#
# Makefile.in for UAE tools
#


CC        = gcc
LDFLAGS   = 
CFLAGS    = -g -O2 -Wall -W -Wno-unused -D__inline__=inline
LIBRARIES = 

INSTALL         = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA    = ${INSTALL} -m 644
prefix          = /usr/local
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libdir          = ${exec_prefix}/lib
sysconfdir      = ${prefix}/etc

VPATH = .

.SUFFIXES: .o .c .h .m .i .S

INCLUDES=-I. -I./../include/ -I.

all: gencpu gencomp build68k genblitter genlinetoscr

install:


clean:
	-rm -f *.o
	-rm cpudefs.c
	-rm -f gencpu gencomp genblitter build68k genlinetoscr


distclean: clean
	-rm -f config.cache config.log config.status
	-rm -f Makefile sysconfig.h
	-rm -f *.c

genblitter: genblitter.o blitops.o writelog.o
	$(CC) $(LDFLAGS) -o genblitter genblitter.o blitops.o writelog.o
build68k: build68k.o writelog.o
	$(CC) $(LDFLAGS) -o build68k build68k.o writelog.o
gencpu: gencpu.o readcpu.o cpudefs.o missing.o writelog.o
	$(CC) $(LDFLAGS) -o gencpu gencpu.o readcpu.o cpudefs.o missing.o writelog.o
gencomp: gencomp.o readcpu.o cpudefs.o missing.o
	$(CC) $(LDFLAGS) -o gencomp gencomp.o readcpu.o missing.o cpudefs.o writelog.o
genlinetoscr: genlinetoscr.o
	$(CC) $(LDFLAGS) -o genlinetoscr genlinetoscr.o

custom.o: blit.h
drawing.o: linetoscr.c

cpudefs.c: build68k ./../table68k
	./build68k <./../table68k >cpudefs.c

.c.o:
	$(CC) $(INCLUDES) -c $(INCDIRS) $(CFLAGS) $(X_CFLAGS) $(DEBUGFLAGS) $(CFLAGS_$@) $< -o $@
.c.s:
	$(CC) $(INCLUDES) -S $(INCDIRS) $(CFLAGS) $(X_CFLAGS) $(DEBUGFLAGS) $< -o $@
.c.i:
	$(CC) $(INCLUDES) -E $(INCDIRS) $(CFLAGS) $(X_CFLAGS) $(DEBUGFLAGS) $< > $@

