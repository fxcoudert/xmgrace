#####################################################
# Makefile for libgrace_np                          #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=..

include $(TOP)/Make.conf

.SUFFIXES : .c .f $(O)

LIB = libgrace_np.a

CFLAGS=$(CFLAGS0) -I$(TOP) -I. $(CPPFLAGS)

LIBS = $(NOGUI_LIBS)

LIBOBJS = grace_np$(O) gracef_np$(O) gracef_np_$(O)
TESTS=test_np$(EXE) testf_np$(EXE)

all : $(LIB)

tests : $(TESTS)
	-@PATH=../src:$$PATH; export PATH; GRACE_HOME=..; export GRACE_HOME; \
	for i in $(TESTS); do echo Running $$i; ./$$i; done

$(LIB) : $(LIBOBJS) 
	$(RM) $@
	$(AR) cr $@ $(LIBOBJS)
	$(RANLIB) $@

$(LIBOBJS) : $(TOP)/config.h grace_np.h

gracef_np_$(O) : gracef_np.c
	$(CC) $(CFLAGS) -DNEED_F77_UNDERSCORE -c -o $@ gracef_np.c

test_np$(EXE) : test_np$(O)
	$(CC) $(CFLAGS) -o $@ $? $(LIB)

testf_np$(EXE) : testf_np$(O)
	-$(FC) $(FFLAGS) -o $@ $? $(LIB)

test_np$(O) : $(LIB)
testf_np$(O) : $(LIB)

install : $(LIB)
	$(MKINSTALLDIRS) $(DESTDIR)$(GRACE_HOME)/lib
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(GRACE_HOME)/lib/$(LIB)
	$(MKINSTALLDIRS) $(DESTDIR)$(GRACE_HOME)/include
	$(INSTALL_DATA) grace_np.h $(DESTDIR)$(GRACE_HOME)/include/grace_np.h

$(DESTDIR)$(GRACE_HOME)/lib/$(LIB) : $(LIB)
	$(MAKE) install

links : $(DESTDIR)$(GRACE_HOME)/lib/$(LIB)
	$(MKINSTALLDIRS) $(DESTDIR)$(PREFIX)/lib
	cd $(DESTDIR)$(PREFIX)/lib; $(RM) $(LIB); $(LN_S) $(GRACE_HOME)/lib/$(LIB) $(LIB)
	$(MKINSTALLDIRS) $(DESTDIR)$(PREFIX)/include
	cd $(DESTDIR)$(PREFIX)/include; $(RM) grace_np.h; $(LN_S) $(GRACE_HOME)/include/grace_np.h grace_np.h
	
clean :
	$(RM) *.o sample.agr $(TESTS)

distclean :
	$(RM) *.o sample.agr $(LIB) $(TESTS)
	
devclean :
	$(RM) *.o sample.agr $(LIB) $(TESTS)

