INCLUDEDIR = ../include
INCLUDEPNG = /usr/include/libpng12
OBJDIR = ../facade-obj
SRCFILES = $(wildcard ./*.c)
HEADERFILES = $(wildcard $(INCLUDEDIR)/*.h)
CC = gcc
CCFLAGS = -ansi -c -Wall -O2 -I$(INCLUDEDIR)

all : compile

compile : objdir imageio.o imageinfo.o timedate.o geoinfo.o

objdir :
			@if test -e $(OBJDIR); then echo "$(OBJDIR) directory already exists";\
			 else mkdir -p $(OBJDIR); fi

imageio.o : imageio.c $(INCLUDEDIR)/imageio.h
				$(CC) $(CCFLAGS) -I$(INCLUDEPNG) imageio.c -o $(OBJDIR)/imageio.o

imageinfo.o : imageinfo.c $(INCLUDEDIR)/imageinfo.h
				$(CC) $(CCFLAGS) imageinfo.c -o $(OBJDIR)/imageinfo.o

timedate.o : timedate.c $(INCLUDEDIR)/timedate.h
				$(CC) $(CCFLAGS) timedate.c -o $(OBJDIR)/timedate.o

geoinfo.o : geoinfo.c $(INCLUDEDIR)/timedate.h $(INCLUDEDIR)/geoinfo.h
				$(CC) $(CCFLAGS) geoinfo.c -o $(OBJDIR)/geoinfo.o

clean:
		rm -rf *~
		rm -rf $(OBJDIR)
