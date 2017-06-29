INCLUDEDIR = ../include
OBJDIR = ../facade-obj
BINDIR = ../bin
IMGDIR = TestImgs
SRCFILES = $(wildcard ./*.c)
HEADERFILES = $(wildcard $(INCLUDEDIR)/*.h)
CC = gcc
CCFLAGS = -ansi -Wall -O2 -I$(INCLUDEDIR)
LIBJPEG = jpeg
LIBPNG = png
LIBXML = expat
LIBEXIF = exif
LIBMAT = m
BINFILES = cloudcover

all : bindir compile

bindir :
			@if test -e $(BINDIR); then echo "$(BINDIR) directory already exists";\
			 else mkdir -p $(BINDIR); fi; cp -r  $(IMGDIR) $(BINDIR)/$(IMGDIR);

compile : $(BINFILES)

cloudcover : $(OBJDIR)/imageio.o $(OBJDIR)/timedate.o $(OBJDIR)/imageinfo.o $(OBJDIR)/geoinfo.o\
		       $(INCLUDEDIR)/imageio.h $(INCLUDEDIR)/timedate.h $(INCLUDEDIR)/imageinfo.h $(INCLUDEDIR)/geoinfo.h $(INCLUDEDIR)/cloudcover.h\
				 cloudcover.c
				 $(CC) $(CCFLAGS)  cloudcover.c $(OBJDIR)/imageio.o $(OBJDIR)/timedate.o $(OBJDIR)/imageinfo.o $(OBJDIR)/geoinfo.o\
	                -l$(LIBJPEG) -l$(LIBPNG) -l$(LIBXML) -l$(LIBEXIF) -l$(LIBMAT) -o $(BINDIR)/cloudcover

clean:
		rm -rf *~
		rm -rf $(BINDIR)
