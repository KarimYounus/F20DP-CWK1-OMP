CC=gcc-13
CFLAGS=-fopenmp -std=c17

all: build

build: SumTot.c
	$(CC) $(CFLAGS) -o sum SumTot.c

compile: build
	./sum 0 100

debug: CFLAGS += -g
debug: build

.PHONY: clean all debug

clean:
	rm -f sum
