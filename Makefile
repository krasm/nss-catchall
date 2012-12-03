# Makefile for nss-extrausers

CC = gcc
prefix = /usr
exec_prefix = ${prefix}
BITSOFS=
libprefix = ${exec_prefix}/lib$(BITSOFS)
DESTDIR=
OBJSUFFIX=$(BITSOFS).o
OBJECTS=nss_catchall$(OBJSUFFIX)
SHARED_OBJECT = libnss_catchall$(BITSOFS).so.2
INSTALL_NAME = libnss_catchall.so.2
# This only works sometimes, give manually when needed:
BIT_CFLAGS = $(if $(BITSOFS),-m$(BITSOFS))
CFLAGS = $(BIT_CFLAGS) -g -O2 -Wall -Wstrict-prototypes -Wpointer-arith -Wmissing-prototypes
CPPFLAGS =
LIBS = -lc
LDLIBFLAGS = -shared -Wl,-soname,$(INSTALL_NAME)
LDFLAGS = -Wl,-z,defs
VERSION = unreleased

all: $(SHARED_OBJECT)

$(SHARED_OBJECT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDLIBFLAGS) $(LDFLAGS) -o $(SHARED_OBJECT) $(OBJECTS) $(LIBS)

%$(OBJSUFFIX): %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c -o $@ $<

install:
	install -m755 -d $(DESTDIR)$(libprefix)/
	install -m644 $(SHARED_OBJECT) $(DESTDIR)$(libprefix)/$(INSTALL_NAME)

clean:
	rm -f $(OBJECTS)
	rm -f $(SHARED_OBJECT)

distclean: clean

dist: Makefile README s_config.h $(patsubst %$(OBJSUFFIX),%.c,$(OBJECTS))
	mkdir libnss-extrausers-$(VERSION)
	install -m 644 $^ libnss-extrausers-$(VERSION)
	tar -cvvzf libnss-extrausers_$(VERSION).orig.tar.gz libnss-extrausers-$(VERSION)
	rm -r libnss-extrausers-$(VERSION)

.PHONY: all
