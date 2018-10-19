GUILD = @GUILD@
GUILE_CFLAGS = @GUILE_CFLAGS@
GUILE_LIBS = @GUILE_LIBS@

GUILE_EFFECTIVE_VERSION = @GUILE_EFFECTIVE_VERSION@
GUILE_SITE = @GUILE_SITE@
GUILE_SITE_CCACHE = @GUILE_SITE_CCACHE@
GUILE_EXTENSION = @GUILE_EXTENSION@

plugindir = $(GUILE_SITE_CCACHE)
plugin_LTLIBRARIES = guile-databuf.la

guile_databuf_la_SOURCES = databuf.c
guile_databuf_la_CFLAGS = $(GUILE_CFLAGS)
guile_databuf_la_LDFLAGS=-module
guile_databuf_la_LIBADD = $(GUILE_LIBS) -ldatabuf

all-local: databuf.go

databuf.go: databuf.scm
	$(GUILD) compile -o $@ $<

clean-local:
	rm -f databuf.go

install-exec-local:
	$(INSTALL) -D databuf.scm $(GUILE_SITE)/databuf.scm
	$(INSTALL) -D databuf.go $(GUILE_SITE_CCACHE)/databuf.go

EXTRA_DIST = databuf.scm

