UNRARDIR =	../../../unrarlib
CC      =       gcc
CFLAGS  =       -ansi -pedantic -pipe -I$(UNRARDIR) -Wall -W -O2 -fPIC -DNO_DEBUG
LINK    =       gcc
SOURCES =	listarchive.c unrarlib.c
OBJECTS =	listarchive.o unrarlib.o
APP	=	listarchive

.SUFFIXES: .c
.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<


all:
	-make symlinks
	-make $(APP)
	-make unsymlinks

symlinks:
	-ln -s -f $(UNRARDIR)/unrarlib.c $(UNRARLIB)/unrarlib.h .

unsymlinks:
	$(RM) -f  unrarlib.c unrarlib.h

$(APP): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(APP) $(OBJECTS) unrarlib.c unrarlib.h



