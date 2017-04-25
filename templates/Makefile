#####################################################
# Makefile for templates of Grace                   #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=..

include $(TOP)/Make.conf

DEFAULT_AGR = Default.agr

all : $(DEFAULT_AGR)

clean : dummy

distclean : 
	$(RM) *.orig

devclean : dummy

install : $(DEFAULT_AGR)
	$(MKINSTALLDIRS) $(DESTDIR)$(GRACE_HOME)/templates
	@if test -f $(DESTDIR)$(GRACE_HOME)/templates/$(DEFAULT_AGR); then \
		echo "	$(DESTDIR)$(GRACE_HOME)/templates/$(DEFAULT_AGR) exists"; \
		echo "	Installing only $(DESTDIR)$(GRACE_HOME)/templates/$(DEFAULT_AGR).sample"; \
		$(INSTALL_DATA) $(DEFAULT_AGR) $(DESTDIR)$(GRACE_HOME)/templates/$(DEFAULT_AGR).sample; \
	else \
		$(INSTALL_DATA) $(DEFAULT_AGR) $(DESTDIR)$(GRACE_HOME)/templates/$(DEFAULT_AGR); \
	fi

links : dummy

tests : dummy

dummy :
