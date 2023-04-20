CC = cc
PREFIX = /usr/local
CFLAGS = -Wall -Wextra -fPIC
CFLAGS += $(shell pkg-config --cflags json-c libcurl)
LDFLAGS = $(shell pkg-config --libs json-c libcurl)

SUBDIRS = out

.SILENT: subdirs
.PHONY: subdirs clean
all: build

subdirs:
	mkdir -p $(SUBDIRS)

out/%.o: src/%.c subdirs
	gcc -c $(CFLAGS) -Iinclude/ -o $@ $<

LNAME = libido.so
SONAME = $(LNAME).1
REALNAME = $(SONAME).0.0
LIBDIR = $(PREFIX)/lib

libido.so: out/libido.o
	gcc -shared -Wl,-soname,$@.1 $^ $(LDFLAGS) -o $(REALNAME)
	- ln -sf $(REALNAME) $(LNAME)
	- ln -sf $(REALNAME) $(SONAME)

build: libido.so

install:
	install --strip $(REALNAME) $(LIBDIR)
	ln -sf $(LIBDIR)/$(REALNAME) $(LIBDIR)/$(SONAME)
	ln -sf $(LIBDIR)/$(REALNAME) $(LIBDIR)/$(LNAME)
	ldconfig $(LIBDIR)

clean:
	rm -f out/*.o *.so* *.a
	rm -rf $(SUBDIRS)
