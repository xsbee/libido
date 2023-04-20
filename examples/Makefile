CFLAGS = -Wall -Wextra
CFLAGS += -I../include
LDFLAGS = -Wl,-rpath,'$$ORIGIN/../' -L../ -lido
LDFLAGS += $(shell pkg-config --cflags --libs libcurl)

EXES = libido-query all-posters

all: libido-query all-posters

libido-query: util.o libido-query.o
	gcc $^ $(LDFLAGS) -o $@

all-posters: all-posters.o
	gcc $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f *.o $(EXES)
