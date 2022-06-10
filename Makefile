CC = cc

CFLAGS = -Wall -Wextra -fPIC
CFLAGS += $(shell pkg-config --cflags json-c libcurl)
LDFLAGS = $(shell pkg-config --libs json-c libcurl)

SUBDIRS = out

.SILENT: subdirs
.PHONY: subdirs clean
all: build build-examples

subdirs:
	mkdir -p $(SUBDIRS)

out/%.o: src/%.c subdirs
	gcc -c $(CFLAGS) -Iinclude/ -o $@ $<

libido.so: out/libido.o
	gcc -shared -Wl,-soname,$@ $(LDFLAGS) $< -o $@

libido.a: out/libido.o
	ar rcs $@ $<

build: libido.so libido.a

clean:
	rm -f out/*.o *.so *.a
	rm -rf $(SUBDIRS)
