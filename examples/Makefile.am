
subdir=examples

#include $(top_srcdir)/Makefile
EXTRA_DIST=$(srcdir)/pd.mtx $(srcdir)/vf.mtx make.sh $(srcdir)/make.sh.in $(srcdir)/benchex.sh
CLEANFILES=make.sh
noinst_SCRIPTS=make.sh
EXPSOURCES_RSB=$(srcdir)/hello.c $(srcdir)/transpose.c $(srcdir)/power.c $(srcdir)/autotune.c
EXPSOURCES_NSB=$(srcdir)/hello-spblas.c $(srcdir)/io-spblas.c
EXPSOURCES_FEX=$(srcdir)/fortran.F90 $(srcdir)/fortran_rsb_fi.F90
EXPSOURCES=$(EXPSOURCES_RSB) $(EXPSOURCES_NSB) $(EXPSOURCES_FEX)
EXSTUFF= $(EXTRA_DIST) $(EXPSOURCES) 
LIBRSB_LIB=$(abs_top_builddir)/librsb.la 

if HAVE_C_EXAMPLES
hello_DEPENDENCIES=$(LIBRSB_LIB)
autotune_DEPENDENCIES=$(LIBRSB_LIB)
transpose_DEPENDENCIES=$(LIBRSB_LIB)
power_DEPENDENCIES=$(LIBRSB_LIB)
hello_LDADD=$(default_ldadd)
autotune_LDADD=$(default_ldadd)
transpose_LDADD=$(default_ldadd)
power_LDADD=$(default_ldadd)
EXTRAPROGRAMSC=hello$(EXEEXT) transpose$(EXEEXT) power$(EXEEXT) autotune$(EXEEXT)
if HAVE_SPARSE_BLAS_INTERFACE
io_spblas_DEPENDENCIES=$(LIBRSB_LIB)
io_spblas_LDADD=$(default_ldadd)
hello_spblas_LDADD=$(default_ldadd)
hello_spblas_DEPENDENCIES=$(LIBRSB_LIB)
EXTRAPROGRAMSSPBLAS=hello-spblas$(EXEEXT) io-spblas$(EXEEXT)
else
EXTRAPROGRAMSSPBLAS=
endif
else
EXTRAPROGRAMSSPBLAS=
EXTRAPROGRAMSC=
endif

# NOTE: see main Makefile's default_ldadd 
default_ldadd=-L$(top_builddir) $(LIBRSB_LIB)
AM_CFLAGS= -I$(top_builddir) -I$(top_srcdir)
AM_FCFLAGS=-I$(top_builddir) -I$(top_srcdir)
tests:	$(noinst_PROGRAMS) all
	if test $(abs_top_builddir) != $(abs_top_srcdir) ; then cp $(abs_top_srcdir)/pd.mtx $(abs_top_srcdir)/vf.mtx .. ; fi
	for ii in $(noinst_PROGRAMS) ; do echo ./$$ii ; if  ./$$ii ; then true ; else exit -1 ;fi ; done

if HAVE_FORTRAN_EXAMPLES
if HAVE_SPARSE_BLAS_INTERFACE
fortran_DEPENDENCIES=$(LIBRSB_LIB)
fortran_LDADD=$(default_ldadd)
fortran_LINK=$(FCLINK)
fortran_SOURCES=fortran.F90
fortran_rsb_fi_DEPENDENCIES=$(LIBRSB_LIB)
fortran_rsb_fi_LDADD=$(default_ldadd)
fortran_rsb_fi_LINK=$(FCLINK)
fortran_rsb_fi_SOURCES=fortran_rsb_fi.F90
EXTRAPROGRAMSFORTRAN=fortran fortran_rsb_fi
else
EXTRAPROGRAMSFORTRAN=
endif
else
EXTRAPROGRAMSFORTRAN=
endif

noinst_PROGRAMS=$(EXTRAPROGRAMSC) $(EXTRAPROGRAMSSPBLAS) $(EXTRAPROGRAMSFORTRAN)

all: $(EXTRA_DIST)

pd.mtx: $(top_srcdir)/pd.mtx
	cp $< $@ 

vf.mtx: $(top_srcdir)/vf.mtx
	cp $< $@ 

install-data-local:
	$(mkdir_p) "$(DESTDIR)$(docdir)"
	$(mkdir_p) "$(DESTDIR)$(docdir)/examples/"
	$(INSTALL_DATA) $(EXSTUFF) "$(DESTDIR)$(docdir)/examples/"
	chmod +x "$(DESTDIR)$(docdir)/examples/make.sh"

uninstall-local:
	for f in $(EXSTUFF) ; do if test -f "$(DESTDIR)$(docdir)/examples/"$$f ; then  rm "$(DESTDIR)$(docdir)/examples/"$$f ; fi ; done
	if test -d "$(DESTDIR)$(docdir)/examples" ; then rmdir "$(DESTDIR)$(docdir)/examples" || true ; fi
	if test -d "$(DESTDIR)$(docdir)" ; then rmdir "$(DESTDIR)$(docdir)" || true ; fi

