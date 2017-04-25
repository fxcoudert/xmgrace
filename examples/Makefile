#####################################################
# Makefile for examples of Grace                    #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=..

include $(TOP)/Make.conf

DOTEST = dotest$(BAT)

EXAMPLES = altaxis.par logtest.par times.agr \
	au.agr manygraphs.agr terr.agr tinset.agr \
	axes.agr mlo.dat tlog.agr log2.agr log2log.agr logit.agr reciprocal.agr \
	bar.agr explain.agr test.com tmc.c \
	fills.agr props.agr test.dat \
	chartebar.agr charts.agr graphs.par regions.par test1.par tstack.agr \
	brw.dat hilo.agr test2.agr arrows.agr txyr.agr \
	co2.agr log.dat stackedb.agr tfonts.agr typeset.agr xyz.agr \
	co2.par logistic.agr logistic.fit symslines.agr tforms.par polar.agr \
	motif.agr spectrum.agr pie.agr txttrans.agr boxplot.agr vmap.agr \
	xysize.agr xycolor.agr

all : dummy

clean : dummy

distclean : dummy

devclean : dummy

install : $(EXAMPLES) $(DOTEST)
	$(MKINSTALLDIRS) $(DESTDIR)$(GRACE_HOME)/examples
	for i in $(EXAMPLES); do $(INSTALL_DATA) $$i $(DESTDIR)$(GRACE_HOME)/examples; done
	$(INSTALL_PROGRAM) $(DOTEST) $(DESTDIR)$(GRACE_HOME)/examples/$(DOTEST)

links : dummy

tests : $(EXAMPLES) $(DOTEST)
	./$(DOTEST)

dummy :
