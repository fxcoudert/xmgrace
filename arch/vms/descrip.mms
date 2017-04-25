#####################################################
# Top-level Makefile for GRACE   (VMS)              #
#####################################################

# Rolf Niepraschk, 11/97, niepraschk@ptb.de

INCLUDE Make.conf

CD = SET DEFAULT
TOP = [-]
ECHO = WRITE SYS$OUTPUT

ALL : $(SUBDIRS)
         @ !

.LAST
	 @ $(ECHO) ""
	 @ $(ECHO) "Done."

CEPHES :
	 @ $(CD) [.CEPHES]
	 @ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	 @ $(CD) $(TOP)

T1LIB :
	 @ $(CD) [.T1LIB]
	 @ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	 @ $(CD) $(TOP)

XBAE :
	 @ $(CD) [.XBAE.XBAE]
	 @ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	 @ $(CD) [--]

SRC :
	 @ $(CD) [.SRC]
	 @ $(MMS) $(MMSQUALIFIERS) $(MMSTARGETS)
	 @ $(CD) $(TOP)

clean : $(SUBDIRS)
	@ !

distclean : $(SUBDIRS)
	@ !
