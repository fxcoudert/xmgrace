#####################################################
# Makefile for grconvert                            #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=..

include $(TOP)/Make.conf

.SUFFIXES : .c $(O)

PROG = grconvert$(EXE)

SRCS = grconvert.c defaults.c readbin.c writeasc.c util.c

OBJS = grconvert$(O) defaults$(O) readbin$(O) writeasc$(O) util$(O)

CFLAGS = $(CFLAGS0) -I$(TOP) -I. $(CPPFLAGS)

LIBS = $(XDR_LIB) $(NOGUI_LIBS)

all : $(PROG)

$(PROG) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

$(OBJS) : grconvert.h

tests : dummy

links : dummy

clean :
	$(RM) $(OBJS) 

distclean :
	$(RM) $(PROG) $(OBJS)
	
devclean :
	$(RM) $(PROG) $(OBJS)

install : $(PROG)
	$(MKINSTALLDIRS) $(DESTDIR)$(GRACE_HOME)/bin
	$(INSTALL_PROGRAM) -s $(PROG) $(DESTDIR)$(GRACE_HOME)/bin/$(PROG)

dummy :

