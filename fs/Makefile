CC=gcc
CFLAGS+=-D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE
LDFLAGS+=-ldl

all:dlopen

dlopen:dlopen_demo.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o dlopen