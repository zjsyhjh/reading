CC = gcc
CFLAGS = -O2 -Wall -I.


all: zero_epoll.o zero_socket.o

zero_epoll.o: zero_epoll.c
	$(CC) $(CFLAGS) -c zero_epoll.c

zero_socket.o: zero_socket.c
	$(CC) $(CFLAGS) -c zero_socket.c

clean:
	rm -f *.o
