
all:
       
EXTRA_DIST=Doxyfile
if WANT_BUILD_DOC
dox: html/index.html
EXTRA_DIST+=man/* html/*
BUILT_DOC=html man
if HAVE_HELP2MAN
man1_MANS=         man/rsbench.1 man/librsb-config.1
else
man1_MANS=
endif
man3_MANS=man/man3/rsb*

if HAVE_HELP2MAN
man/librsb-config.1: ../librsb-config
	$(HELP2MAN) --name="provide configuration information for librsb" --no-info $< > $@ 
man/rsbench.1: ../rsbench
	$(HELP2MAN) --name="benchmark and test for librsb" --no-info $< > $@ 
endif

$(man3_MANS): html/index.html

html/index.html: ../rsb.h
	$(MAKE) makedox

makedox:
	DOXYGEN_PROJECT_NUMBER=$(VERSION) $(DOXYGEN) Doxyfile || echo "are you sure you have doxygen installed ?"
	mkdir -p man/man3_
	mv man/man3/* man/man3_/
	if find man/man3_/rsb_doc* ; then \
	mv man/man3_/rsb_doc* man/man3/ ; \
	sed -i s/_doc_/::/g man/man3/* ; \
	for f in man/man3/rsb_* ; do mv $$f `echo $$f | sed s/_doc_/::/g` ; done ; \
	for f in man/man3/rsb* ; do $(AWK) -f ../scripts/rsbmandesc.awk $$f > $$f.tmp ; mv $$f.tmp $$f ; done ; \
	for f in man/man3/rsb* ; do sed -i 's/^\(Generated.*$$\)/librsb was written by Michele Martone; this documentation has been generated by Doxygen./g' $$f ; done ; \
	mv  -v man/man3/rsb::rsb.3               man/man3/rsb.h.3 ; \
	sed -i s/rsb::rsb/rsb.h/g                man/man3/rsb.h.3 ; \
	mv  -v man/man3/rsb::sparse_blas.3       man/man3/rsb-spblas.h.3 ; \
	sed -i s/rsb::sparse_blas/rsb-spblas.h/g man/man3/rsb-spblas.h.3 ; \
	mv  -v man/man3/rsb::examples.3          man/man3/rsb-examples.3 ; \
	sed -i s/rsb::examples/rsb-examples/g  man/man3/rsb-examples.3 ; \
	sed -i 's/\\fP\([a-z]\)/\\fP \1/g'     man/man3/rsb*.3 ; \
	for f in man/man3/rsb* ; do ../scripts/rsbmanseealso.sh man/man3/rsb* >>  $$f ; done ; \
	rm -fR man/man3_ ; fi

else
makedox:
	echo "Documentation building has been disabled at configure time --- skipping."

install-data-local:
	$(mkdir_p) "$(DESTDIR)$(docdir)"
	$(mkdir_p) "$(DESTDIR)$(docdir)/html/"
	if test -f ./html/index.html ; then $(INSTALL_DATA)  ./html/* "$(DESTDIR)$(docdir)/html/" ; fi

uninstall-local:
	cd ./html ; for f in * ; do if test -f "$(DESTDIR)$(docdir)/html/"$$f ; then  rm "$(DESTDIR)$(docdir)/html/"$$f ; fi ; done
	if test -d "$(DESTDIR)$(docdir)/html" ; then rmdir "$(DESTDIR)$(docdir)/html" || true ; fi
	if test -d "$(DESTDIR)$(docdir)" ; then rmdir "$(DESTDIR)$(docdir)" || true ; fi
dox:
EXTRA_DIST+=
BUILT_DOC=
man1_MANS=
$(man1_MANS):
man3_MANS=
$(man3_MANS):
endif

cleanall:
	rm -rf $(BUILT_DOC)


