INCLUDEDIR = ../include
OBJDIR = ../facade-obj
TESTBINDIR = ../test-bin
IMGDIR = Imgs
SRCFILES = $(wildcard ./*.c)
HEADERFILES = $(wildcard $(INCLUDEDIR)/*.h)
CC = gcc
CCFLAGS = -ansi -Wall -O2 -I$(INCLUDEDIR)
LIBJPEG = jpeg
LIBPNG = png
LIBXML = expat
LIBEXIF = exif
LIBMAT = m
BINFILES = test-imageio test-timedate test-imageinfo test-geoinfo

all : bindir compile test

bindir :
			@if test -e $(TESTBINDIR); then echo "$(TESTBINDIR) directory already exists";\
			 else mkdir -p $(TESTBINDIR); fi; cp -r  $(IMGDIR) $(TESTBINDIR)/$(IMGDIR);

compile : $(BINFILES)

test-imageio : $(OBJDIR)/imageio.o $(INCLUDEDIR)/imageio.h test-imageio.c
				$(CC) $(CCFLAGS)  test-imageio.c $(OBJDIR)/imageio.o -l$(LIBJPEG) -l$(LIBPNG) -o $(TESTBINDIR)/test-imageio

test-timedate : $(OBJDIR)/timedate.o $(INCLUDEDIR)/timedate.h test-timedate.c
				$(CC) $(CCFLAGS) test-timedate.c $(OBJDIR)/timedate.o -l$(LIBMAT) -o $(TESTBINDIR)/test-timedate

test-imageinfo : $(OBJDIR)/imageinfo.o $(OBJDIR)/timedate.o $(INCLUDEDIR)/imageinfo.h test-imageinfo.c
				$(CC) $(CCFLAGS) test-imageinfo.c $(OBJDIR)/imageinfo.o $(OBJDIR)/timedate.o -l$(LIBEXIF) -l$(LIBMAT) -o $(TESTBINDIR)/test-imageinfo

test-geoinfo : $(OBJDIR)/geoinfo.o $(OBJDIR)/timedate.o $(INCLUDEDIR)/timedate.h $(INCLUDEDIR)/geoinfo.h test-geoinfo.c
				$(CC) $(CCFLAGS) test-geoinfo.c $(OBJDIR)/geoinfo.o $(OBJDIR)/timedate.o -l$(LIBXML) -l$(LIBMAT) -o $(TESTBINDIR)/test-geoinfo

test: bindir compile
		cd $(TESTBINDIR);\
		for i in $(BINFILES); do echo "Executing $$i"; ./$$i 2> trash; if [ $$? -eq 1 ]; then    echo "Ok"; else    echo "Oops"; fi; done

clean:
		rm -rf *~
		rm -rf $(TESTBINDIR)
