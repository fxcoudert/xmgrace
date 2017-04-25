#####################################################
# Makefile for documentation of Grace               #
#####################################################
# You should not change anything here.              #
#####################################################

TOP=..

include $(TOP)/Make.conf

.SUFFIXES : .sgml .dvi .html .pdf .ps .tex

# NB: the language and papersize options should be made `configure'able
LANGUAGE    = english
PAPERSIZE   = letter

LINUXDOC    = linuxdoc --language=$(LANGUAGE) --papersize=$(PAPERSIZE)

SGML2HTML   = $(LINUXDOC) --backend=html  --split=0 --toc=2
SGML2LATEX  = $(LINUXDOC) --backend=latex --output=tex
SGML2DVI    = $(LINUXDOC) --backend=latex --output=dvi
SGML2PDF    = $(LINUXDOC) --backend=latex --output=pdf
DVI2PS      = dvips -t $(PAPERSIZE)
LATEX       = latex
PDFLATEX    = pdflatex

.sgml.html :
	$(SGML2HTML) $<

.sgml.dvi :
	$(SGML2DVI) $<
	
.sgml.pdf :
	$(SGML2PDF) $<
	
.sgml.tex :
	$(SGML2LATEX) $<

.tex.dvi :
	$(LATEX) $<
	$(LATEX) $<

.dvi.ps :
	$(DVI2PS) $< -o $@

.tex.pdf :
	$(PDFLATEX) $<
	$(PDFLATEX) $<

SGMLS    = FAQ.sgml Tutorial.sgml UsersGuide.sgml 
HTMLS    = CHANGES.html GPL.html philosophical-gnu-sm.jpg nohelp.html

HTMLDOCS = FAQ.html Tutorial.html UsersGuide.html

DVIDOCS  = FAQ.dvi Tutorial.dvi UsersGuide.dvi

PSDOCS   = FAQ.ps Tutorial.ps UsersGuide.ps

PDFDOCS  = FAQ.pdf Tutorial.pdf UsersGuide.pdf

TEXDOCS  = FAQ.tex Tutorial.tex UsersGuide.tex

TUTDATA  = 2.1.dat 3.dat 7.1.2.agr 7.1.3.agr 8.2.dat \
           10.1.dat 10a.dat 10c.dat 10b.dat shiftdata.sh mygraph.png

MANPAGES = grace.1 gracebat.1 xmgrace.1 grconvert.1 convcal.1

all : html pdf

html : $(HTMLDOCS)

dvi  : $(DVIDOCS)

pdf  : $(PDFDOCS)

ps   : $(PSDOCS)

tex  : $(TEXDOCS)

clean :
	$(RM) $(TEXDOCS) *.aux *.log *.toc *.out

distclean : clean
	$(RM) *.orig

devclean : distclean
	$(RM) $(DVIDOCS) $(PSDOCS) $(PDFDOCS) $(HTMLDOCS)

install : $(HTMLS) $(HTMLDOCS) $(PDFDOCS) $(TUTDATA) $(MANPAGES)
	$(MKINSTALLDIRS) $(DESTDIR)$(GRACE_HOME)/doc
	for i in $?; do $(INSTALL_DATA) $$i $(DESTDIR)$(GRACE_HOME)/doc; done

$(DESTDIR)$(GRACE_HOME)/doc/grace.1 : $(MANPAGES)
	$(MAKE) install

links : $(DESTDIR)$(GRACE_HOME)/doc/grace.1
	$(MKINSTALLDIRS) $(DESTDIR)$(PREFIX)/man
	$(MKINSTALLDIRS) $(DESTDIR)$(PREFIX)/man/man1
	cd $(DESTDIR)$(PREFIX)/man/man1; \
	for i in $(MANPAGES); do $(RM) $$i; $(LN_S) $(GRACE_HOME)/doc/$$i $$i; done

tests : dummy

dummy :
