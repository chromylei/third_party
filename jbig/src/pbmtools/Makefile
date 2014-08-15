# Unix makefile for the JBIG-KIT PBM tools
# $Id: Makefile,v 1.12 2003-06-11 18:27:22+01 mgk25 Exp $

# Select an ANSI/ISO C compiler here, e.g. GNU gcc is recommended
CC = gcc

# Options for the compiler
CFLAGS = -g -Wall -ansi -pedantic -I../libjbig
LIBS = -lstdbin -lbinmode -L../libjbig -ljbig

MAJOR=$(shell echo $(VERSION) | sed -e "s/\\..*$$//")
MINOR=$(shell echo $(VERSION) | sed -e "s/^[^\\.]*\\.0*\\([^.]*\\).*$$/\\1/")
LDFLAGS = -Wl,--major-image-version=$(MAJOR) -Wl,--minor-image-version=$(MINOR)

.SUFFIXES: .1 .5 .txt $(SUFFIXES)

# Rule to make compiled resource (Windows)
%-rc.o: %.rc
	windres -i $< -o $@

all: pbmtojbg jbgtopbm pbmtojbg.txt jbgtopbm.txt pbm.txt pgm.txt

pbmtojbg: pbmtojbg.o pbmtojbg-rc.o ../libjbig/libjbig.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o pbmtojbg pbmtojbg.o pbmtojbg-rc.o $(LIBS)

jbgtopbm: jbgtopbm.o jbgtopbm-rc.o ../libjbig/libjbig.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o jbgtopbm jbgtopbm.o jbgtopbm-rc.o $(LIBS)

jbgtopbm.o: jbgtopbm.c ../libjbig/jbig.h
pbmtojbg.o: pbmtojbg.c ../libjbig/jbig.h

../libjbig/libjbig.a: ../libjbig/jbig.c ../libjbig/jbig.h
	cd ../libjbig ; make libjbig.a

test: pbmtojbg jbgtopbm
	make IMG=ccitt1     OPTIONSJ=      OPTIONSP=       dotest1
	make IMG=ccitt2     OPTIONSJ=      OPTIONSP=       dotest1
	make IMG=ccitt3     OPTIONSJ=      OPTIONSP=       dotest1
	make IMG=xvlogo     OPTIONSJ=     "OPTIONSP=-d 3"  dotest1
	make IMG=sandra     OPTIONSP=      OPTIONSJ=       dotest2g
	make IMG=sandra     OPTIONSP=-b    OPTIONSJ=-b     dotest2g
	make IMG=sandra     OPTIONSP=-q    OPTIONSJ=       dotest2g
	make IMG=sandra    "OPTIONSP=-o 0" OPTIONSJ=       dotest2g
	make IMG=sandra    "OPTIONSP=-o 2" OPTIONSJ=       dotest2g
	make IMG=multi      OPTIONSP=      OPTIONSJ=       dotest2g
	make IMG=multi      OPTIONSP=-b    OPTIONSJ=-b     dotest2g
	make IMG=mx        "OPTIONSP=-q -s 3 -m 128"       dotest1
	make IMG=mx        "OPTIONSP=-q -s 3 -m 128"       dotest2b
	make IMG=mx        "OPTIONSP=-q -s 3 -m 128 -p 92" dotest2b
	make IMG=mx        "OPTIONSP=-q -Y -1"             dotest2b
	make IMG=mx        "OPTIONSP=-Y -1"                dotest2b
	rm -f test-*.jbg test-*.pbm test-*.pgm
	./jbgtopbm ../examples/ccitt1.jbg | ./pbmtojbg > test-ccitt1.jbg
	cmp ../examples/ccitt1.jbg test-ccitt1.jbg
	rm -f test-*.jbg test-*.pbm test-*.pgm
	./jbgtopbm < ../examples/ccitt1.jbg | ./pbmtojbg - test-ccitt1.jbg
	cmp ../examples/ccitt1.jbg test-ccitt1.jbg
	rm -f test-*.jbg test-*.pbm test-*.pgm
	./jbgtopbm < ../examples/ccitt1.jbg - test-ccitt1.pbm ; \
	  ./pbmtojbg test-ccitt1.pbm test-ccitt1.jbg
	cmp ../examples/ccitt1.jbg test-ccitt1.jbg
	rm -f test-*.jbg test-*.pbm test-*.pgm
	./jbgtopbm ../examples/ccitt1.jbg test-ccitt1.pbm ; \
	  ./pbmtojbg test-ccitt1.pbm >test-ccitt1.jbg
	cmp ../examples/ccitt1.jbg test-ccitt1.jbg
	rm -f test-*.jbg test-*.pbm test-*.pgm
	@echo
	@echo "The pbmtools have PASSED the functional tests. Good!"
	@echo

dotest1:
	./jbgtopbm $(OPTIONSJ) ../examples/$(IMG).jbg test-$(IMG).pbm
	./pbmtojbg $(OPTIONSP) test-$(IMG).pbm test-$(IMG).jbg
	cmp test-$(IMG).jbg ../examples/$(IMG).jbg

dotest2b:
	./pbmtojbg $(OPTIONSP) test-$(IMG).pbm test-$(IMG).jbg
	./jbgtopbm $(OPTIONSJ) test-$(IMG).jbg test-$(IMG)-2.pbm
	cmp test-$(IMG).pbm test-$(IMG)-2.pbm

dotest2g:
	./pbmtojbg $(OPTIONSP) ../examples/$(IMG).pgm test-$(IMG).jbg
	./jbgtopbm $(OPTIONSJ) test-$(IMG).jbg test-$(IMG).pgm
	cmp test-$(IMG).pgm ../examples/$(IMG).pgm

.1.txt:
	gtroff -man -Tascii $< | grotty -u -b - >$@

.5.txt:
	gtroff -man -Tascii $< | grotty -u -b - >$@

clean:
	rm -f *.o *~ core pbmtojbg jbgtopbm
	rm -f test-*.jbg test-*.pbm test-*.pgm
