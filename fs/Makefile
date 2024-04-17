CC=gcc
CFLAGS+=-D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE
LDFLAGS+=-lcrypt

chkpasswd:chkpasswd.o
	$(CC) $^ -o $@ $(LDFLAGS)
clean:
	rm -f chkpasswd chkpasswd.o