#####################################################
# Makefile for Cephes math library (VMS)            #
#####################################################

# Rolf Niepraschk, 12/97, niepraschk@ptb.de

TOP = [-]
ECHO = WRITE SYS$OUTPUT

INCLUDE $(TOP)Make.conf

CFLAGS = $(CFLAGS0)/INCLUDE=$(TOP)

LIB = libcephes.olb
                   
INCLUDE Make.common
#INCLUDE Make.dep

all : msg $(LIB)($(OBJS))
	@ !

msg : 
        @ $(ECHO) ""
        @ $(ECHO) "Making $(LIB) ..."
        @ $(ECHO) ""
 
install : $(LIB)

tests : dummy

links : dummy

clean :
        IF F$SEARCH("*$(O)",).NES."" THEN $(RM) *$(O);*
        IF F$SEARCH("$(LIB)",).NES."" THEN $(RM) $(LIB);*

distclean : clean
	@ !

devclean : clean
	@ !

dummy :
