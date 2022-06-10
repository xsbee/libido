CC = cc
CFLAGS = -Wall -Wextra
CFLAGS += -I../include
LDFLAGS = -L../ -l:libido.a
LDFLAGS += $(shell pkg-config --libs libcurl json-c)

EXES = libido-query

libido-query: util.o libido-query.o
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f *.o $(EXES)
