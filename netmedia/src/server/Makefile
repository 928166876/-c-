CFLAGS+=-I../include/ -pthread
LDFLAGS+=-pthread


all:server
server:server.o thr_list.o thr_channel.o medialib.o mytbf.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)
clean:
	rm -rf *.o server