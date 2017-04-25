#####################################################
# Makefile for T1lib (bundled with Grace)           #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=..

include $(TOP)/Make.conf

.SUFFIXES : .c $(O)

ALLCFLAGS = $(CFLAGS0) -I../.. $(CPPFLAGS) -DGLOBAL_CONFIG_DIR="\".\"" \
	-DT1_AA_TYPE16=$(T1_AA_TYPE16) -DT1_AA_TYPE32=$(T1_AA_TYPE32)
SUBMAKE   = $(MAKE) 'CC=$(CC)' 'CFLAGS=$(ALLCFLAGS)'


TYPE1_OBJS = \
	type1/arith$(O) \
	type1/curves$(O) \
	type1/fontfcn$(O) \
	type1/hints$(O) \
	type1/lines$(O) \
	type1/objects$(O) \
	type1/paths$(O) \
	type1/regions$(O) \
	type1/scanfont$(O) \
	type1/spaces$(O) \
	type1/t1io$(O) \
	type1/t1snap$(O) \
	type1/t1stub$(O) \
	type1/token$(O) \
	type1/type1$(O) \
	type1/util$(O) 


T1LIB_OBJS = \
	t1lib/t1finfo$(O) \
	t1lib/t1base$(O) \
	t1lib/t1delete$(O) \
	t1lib/t1enc$(O) \
	t1lib/t1env$(O) \
	t1lib/t1load$(O) \
	t1lib/t1set$(O) \
	t1lib/t1trans$(O) \
	t1lib/t1aaset$(O) \
	t1lib/t1afmtool$(O) \
	t1lib/t1outline$(O) \
	t1lib/t1subset$(O) \
	t1lib/parseAFM$(O) 


TYPE1_SRCS = \
	type1/arith.c \
	type1/curves.c \
	type1/fontfcn.c \
	type1/hints.c \
	type1/lines.c \
	type1/objects.c \
	type1/paths.c \
	type1/regions.c \
	type1/scanfont.c \
	type1/spaces.c \
	type1/t1io.c \
	type1/t1snap.c \
	type1/t1stub.c \
	type1/token.c \
	type1/type1.c \
	type1/util.c 


T1LIB_SRCS = \
	t1lib/t1finfo.c \
	t1lib/t1base.c \
	t1lib/t1delete.c \
	t1lib/t1enc.c \
	t1lib/t1env.c \
	t1lib/t1load.c \
	t1lib/t1set.c \
	t1lib/t1trans.c \
	t1lib/t1aaset.c \
	t1lib/t1afmtool.c \
	t1lib/t1outline.c \
	t1lib/parseAFM.c 


LIBSUBDIRS = \
	type1 \
	t1lib


all : libt1.a


$(TYPE1_OBJS) : $(TYPE1_SRCS)
	(cd type1; $(SUBMAKE))

$(T1LIB_OBJS) : $(T1LIB_SRCS)
	(cd t1lib; $(SUBMAKE))

libt1.a : $(TYPE1_OBJS) $(T1LIB_OBJS)
	$(RM) $@
	$(AR) cr $@ $(TYPE1_OBJS) $(T1LIB_OBJS)
	$(RANLIB) $@


.PHONY : clean


clean : dummy
	for i in $(LIBSUBDIRS); do \
	  (cd $$i; $(MAKE) clean) || exit 1; \
	done
	$(RM) libt1.a *~  

distclean : dummy
	for i in $(LIBSUBDIRS); do \
	  (cd $$i; $(MAKE) clean) || exit 1; \
	done
	$(RM) libt1.a *~  

install : dummy

links : dummy

tests : dummy

dummy :

