#####################################################
# Makefile for auxiliary stuff of Grace             #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=..

include $(TOP)/Make.conf

AUXILIARIES = README convcal.c magic.grace
PROGRAMS = convcal$(EXE)
SCRIPTS = fdf2fit

all : convcal$(EXE)

clean : dummy

distclean : clean
	$(RM) convcal$(EXE)

devclean : distclean

convcal$(EXE) : convcal.c
	$(CC) $(CFLAGS0) $(CPPFLAGS) -o $@ convcal.c $(NOGUI_LIBS)

install : $(AUXILIARIES) $(PROGRAMS) $(SCRIPTS)
	$(MKINSTALLDIRS) $(DESTDIR)$(GRACE_HOME)/auxiliary
	for i in $(AUXILIARIES); do $(INSTALL_DATA) $$i $(DESTDIR)$(GRACE_HOME)/auxiliary; done
	for i in $(PROGRAMS); do $(INSTALL_PROGRAM) -s $$i $(DESTDIR)$(GRACE_HOME)/bin; done
	for i in $(SCRIPTS); do $(INSTALL_PROGRAM) $$i $(DESTDIR)$(GRACE_HOME)/bin; done

tests : dummy

links : dummy

dummy :
