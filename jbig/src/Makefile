# Unix makefile for JBIG-KIT
# $Id: Makefile,v 1.14 2004-06-10 22:00:30+01 mgk25 Exp $

# Select an ANSI/ISO C compiler here, GNU gcc is recommended
CC = gcc

# Options for the compiler: A high optimization level is suggested
CCFLAGS = -D__GW32__ -D_LARGEFILE_SOURCE=1 -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 \
		-Wall -O3 -fms-extensions -mms-bitfields  -fno-exceptions -fomit-frame-pointer \
		-march=i386 -mcpu=i686 -idirafter h:/glibc/include
#CCFLAGS = -O -g -W -Wall -ansi -pedantic #-DDEBUG  # developer only

CFLAGS = $(CCFLAGS) -I../libjbig

VERSION=1.6

all: lib pbm
	@echo "Enter 'make test' in order to start some automatic tests."

lib:
	(cd libjbig;  make "CC=$(CC)" "CFLAGS=$(CFLAGS)")

pbm: lib
	(cd pbmtools; make "CC=$(CC)" "CFLAGS=$(CFLAGS)" "VERSION=$(VERSION)")

test: lib pbm
	(cd libjbig;  make "CC=$(CC)" "CFLAGS=$(CFLAGS)" test)
	(cd pbmtools; make "CC=$(CC)" "CFLAGS=$(CFLAGS)" test)

clean:
	rm -f *~ core
	(cd libjbig; make clean)
	(cd pbmtools; make clean)

distribution: clean
	rm -f libjbig/libjbig.a
	(cd ..; tar -c -v --exclude RCS -f jbigkit-$(VERSION).tar jbigkit ; \
	  gzip -9f jbigkit-$(VERSION).tar )
	mv ../jbigkit-$(VERSION).tar.gz $(HOME)/public_html/download/
	cp CHANGES $(HOME)/public_html/jbigkit/
