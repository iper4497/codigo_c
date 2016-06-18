PREFIX=/opt/lnxi
PERLPATH=/usr/bin/perl
VERSION="1.6"
DATE="1 February 2001"

SHAREDIR=$(PREFIX)/share/mkelfImage
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/man/man1

DIRS=$(SHAREDIR) $(BINDIR) $(MANDIR)

MANS=mkelfImage.1

FILES=mkelfImage $(MANS) 

all: $(FILES)

clean:
	rm -f $(FILES)

install: $(DIRS) $(FILES)
	cp -r elf32-i386/ $(SHAREDIR)
	find $(SHAREDIR) -type d | xargs chmod a+x
	find $(SHAREDIR) -type f | xargs chmod 444
	cp mkelfImage $(BINDIR)
	cp $(MANS) $(MANDIR)

$(DIRS):
	mkdir -p $@

%.1 : %.pl Makefile
	pod2man --date=$(DATE) --release=$(VERSION) $*.pl > $@

mkelfImage:  mkelfImage.pl Makefile
	echo 's|^$$params{MYDATA}=".";$$|$$params{MYDATA}="$(SHAREDIR)";|' > sedfile
	echo 's|^#!/usr/bin/perl|#!$(PERLPATH)|' >> sedfile
	sed -f sedfile mkelfImage.pl > $@
	chmod a+x $@
	rm -f sedfile

