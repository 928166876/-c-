CFLAGS+=-pthread
LDFLAGS+=-pthread

all:mytbf

mytbf:mytbf.o main.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)
clean:
	rm -rf *.o mytbf