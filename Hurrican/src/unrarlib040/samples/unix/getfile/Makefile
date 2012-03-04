# example Makefile for linux
# Johannes Winkelmann, jw@tks6.net

UNRARDIR =	../../../unrarlib
CC      =       gcc
CFLAGS  =       -ansi -pedantic -pipe -I$(UNRARDIR) -Wall -W -O2 -fPIC -DNO_DEBUG
LINK    =       gcc
# HEADERS =	$(UNRARDIR)/unrarlib.h
SOURCES =	getfile.c unrarlib.c
OBJECTS =	getfile.o unrarlib.o
APP	=	getfile



.SUFFIXES: .c
.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all:
	-make symlinks
	-make $(APP)
	-make unsymlinks

symlinks:
	-ln -s -f $(UNRARDIR)/unrarlib.c $(UNRARDIR)/unrarlib.h .

unsymlinks:
	$(RM) -f  unrarlib.c unrarlib.h

$(APP): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(APP) $(OBJECTS)



