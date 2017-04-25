#####################################################
# Makefile for Xbae widget (bundled with Grace)     #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=../..

include $(TOP)/Make.conf

.SUFFIXES : .c $(O)

CFLAGS=$(CFLAGS0) -I$(TOP) -I. $(XBAE_INC) $(CPPFLAGS) $(GUI_FLAGS) \
	-DHAVE_CONFIG_H -DDRAW_RESIZE_SHADOW

LIB=libXbae.a

all : $(LIB)

include Make.common


$(LIB) : $(OBJS)
	$(AR) cr $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

install : $(LIB)

tests : dummy

links : dummy

clean :
	$(RM) $(LIB) $(OBJS)

distclean :
	$(RM) $(LIB) $(OBJS) *.orig

devclean :
	$(RM) $(LIB) $(OBJS) *.orig

dummy :
