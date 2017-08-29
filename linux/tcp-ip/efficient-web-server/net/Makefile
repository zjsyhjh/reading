CC = gcc
CFLAGS = -O2 -Wall -I.


all: zero_epoll.o

zero_epoll.o: zero_epoll.c
	$(CC) $(CFLAGS) -c zero_epoll.c

clean:
	rm -f *.o
