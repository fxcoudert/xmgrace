#####################################################
# Makefile for Xbae matrix widget on VMS            #
#####################################################

TOP=[--]
ECHO = WRITE SYS$OUTPUT

include $(TOP)Make.conf

CFLAGS=$(CFLAGS0)/INCLUDE=($(TOP)) $(GUI_FLAGS) \
  /DEFINE=(DRAW_RESIZE_SHADOW)/WARNINGS=(DISABLE=LONGEXTERN)

LIB=libXbae.OLB

include Make.common

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

.FIRST
        @ define/nolog xbae 'f$string(f$parse("[-]","","","device")+ \
          f$parse("[-]","","","directory") - "]" + ".xbae]")
