#####################################################
# Makefile for grconvert -- VMS version (R.N.) 5/98 #
#####################################################

# not tested (RN)


TOP = [-]
SRCDIR = [-.SRC]
VMSDIR = [-.ARCH.VMS]
#RM = DELETE/log

include $(TOP)Make.conf

PROG = grconvert$(EXE)

SRCS = grconvert.c defaults.c readbin.c writeasc.c util.c

OBJS = grconvert$(O) defaults$(O) readbin$(O) writeasc$(O) util$(O)

.IFDEF FLOAT
.ELSE
FLOAT = D_FLOAT
.ENDIF
.IFDEF MULTINET
CFLAGS = /INCLUDE=($(TOP),$(SRCDIR),$(VMSDIR))/DEFINE=(GRCONVERT=1,MULTINET=1) \
  /FLOAT=$(FLOAT)/PREFIX=ALL
LIBS = ,multinet_common_root:[multinet.library]rpc.olb/LIBRARY
.ELSE
CFLAGS = /INCLUDE=($(TOP),$(SRCDIR),$(VMSDIR))/DEFINE=(GRCONVERT=1) \
  /FLOAT=$(FLOAT)/PREFIX=ALL
LIBS = ,$(VMSDIR)xdr.opt/OPTION
.ENDIF


all : $(PROG)
	@ !

$(PROG) : $(OBJS)
	$(LINK) /EXECUTABLE=$@ $+ $(LIBS)


clean :
	$(RM) $(OBJS) 

distclean : clean
	$(RM) $(PROG) 
	

