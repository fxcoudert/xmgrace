#####################################################
# Makefile for T1lib (bundled with GRACE)           #
#####################################################

TOP = [-]
ECHO = WRITE SYS$OUTPUT
CD = SET DEFAULT

include $(TOP)Make.conf

TYPE1_OBJS = \
	[.type1]arith$(O) \
	[.type1]curves$(O) \
	[.type1]fontfcn$(O) \
	[.type1]hints$(O) \
	[.type1]lines$(O) \
	[.type1]objects$(O) \
	[.type1]paths$(O) \
	[.type1]regions$(O) \
	[.type1]scanfont$(O) \
	[.type1]spaces$(O) \
	[.type1]t1io$(O) \
	[.type1]t1snap$(O) \
	[.type1]t1stub$(O) \
	[.type1]token$(O) \
	[.type1]type1$(O) \
	[.type1]util$(O) 

T1LIB_OBJS = \
	[.t1lib]t1finfo$(O) \
	[.t1lib]t1base$(O) \
	[.t1lib]t1delete$(O) \
	[.t1lib]t1enc$(O) \
	[.t1lib]t1env$(O) \
	[.t1lib]t1load$(O) \
	[.t1lib]t1set$(O) \
	[.t1lib]t1trans$(O) \
	[.t1lib]t1aaset$(O) \
	[.t1lib]t1afmtool$(O) \
	[.t1lib]t1outline$(O) \
	[.t1lib]parseAFM$(O) 

TYPE1_SRCS = \
	[.type1]arith.c \
	[.type1]curves.c \
	[.type1]fontfcn.c \
	[.type1]hints.c \
	[.type1]lines.c \
	[.type1]objects.c \
	[.type1]paths.c \
	[.type1]regions.c \
	[.type1]scanfont.c \
	[.type1]spaces.c \
	[.type1]t1io.c \
	[.type1]t1snap.c \
	[.type1]t1stub.c \
	[.type1]token.c \
	[.type1]type1.c \
	[.type1]util.c 

T1LIB_SRCS = \
	[.t1lib]t1finfo.c \
	[.t1lib]t1base.c \
	[.t1lib]t1delete.c \
	[.t1lib]t1enc.c \
	[.t1lib]t1env.c \
	[.t1lib]t1load.c \
	[.t1lib]t1set.c \
	[.t1lib]t1trans.c \
	[.t1lib]t1aaset.c \
	[.t1lib]t1afmtool.c \
	[.t1lib]t1outline.c \
	[.t1lib]parseAFM.c 

TYPE1_HEADERS = \
	[.type1]Xstuff.h \
	[.type1]arith.h \
	[.type1]blues.h \
	[.type1]cluts.h \
	[.type1]curves.h \
	[.type1]digit.h \
	[.type1]ffilest.h \
	[.type1]font.h \
	[.type1]fontfcn.h \
	[.type1]fontfile.h \
	[.type1]fontmisc.h \
	[.type1]fonts.h \
	[.type1]fontstruct.h \
	[.type1]fontxlfd.h \
	[.type1]fsmasks.h \
	[.type1]hdigit.h \
	[.type1]hints.h \
	[.type1]lines.h \
	[.type1]objects.h \
	[.type1]paths.h \
	[.type1]paths_rmz.h \
	[.type1]pictures.h \
	[.type1]regions.h \
	[.type1]spaces.h \
	[.type1]spaces_rmz.h \
	[.type1]strokes.h \
	[.type1]t1hdigit.h \
	[.type1]t1imager.h \
	[.type1]t1intf.h \
	[.type1]t1stdio.h \
	[.type1]token.h \
	[.type1]tokst.h \
	[.type1]trig.h \
	[.type1]types.h \
	[.type1]util.h

T1LIB_HEADERS = \
	[.t1lib]parseAFM.h \
	[.t1lib]t1afmtool.h \
	[.t1lib]t1aaset.h \
	[.t1lib]t1base.h \
	[.t1lib]t1delete.h \
	[.t1lib]t1enc.h \
	[.t1lib]t1env.h \
	[.t1lib]t1extern.h \
	[.t1lib]t1finfo.h \
	[.t1lib]t1global.h \
	[.t1lib]t1lib.h \
	[.t1lib]t1load.h \
	[.t1lib]t1misc.h \
	[.t1lib]t1set.h \
	[.t1lib]t1trans.h \
	[.t1lib]t1types.h

MAIN_TARGET = libt1lib.olb

DUMMYSUBDIRS=XXXX

all : msg $(DUMMYSUBDIRS) $(MAIN_TARGET)($(TYPE1_OBJS) $(T1LIB_OBJS))
	@ !

msg :
        @ $(ECHO) ""
        @ $(ECHO) "Making $(MAIN_TARGET) ..."
        @ $(ECHO) ""

$(DUMMYSUBDIRS) : dummy
	@ $(CD) [.TYPE1]
	@ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	@ $(CD) [-]
	@ $(CD) [.T1LIB]
	@ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	@ $(CD) [-]

libt1lib.olb : $(TYPE1_OBJS) $(T1LIB_OBJS)

clean : dummy
	@ $(CD) [.TYPE1]
	@ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	@ $(CD) [-]
	@ $(CD) [.T1LIB]
	@ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	@ $(CD) [-]

distclean : dummy
	@ $(CD) [.TYPE1]
	@ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	@ $(CD) [-]
	@ $(CD) [.T1LIB]
	@ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	@ $(CD) [-]
	$(RM) libt1lib.olb  

install : dummy

links : dummy

tests : dummy

dummy :
	@ !
