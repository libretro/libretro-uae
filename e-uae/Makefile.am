ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = m4 amiga docs CHANGES README

SUBDIRS = src

dist_noinst_SCRIPTS = bootstrap.sh

dist-hook:
	rm -rf `find $(distdir) -name CVS`
	rm -rf `find $(distdir) -name *.bak`
	rm -rf `find $(distdir) -name *~`
	rm -rf `find $(distdir) -name *.orig`
