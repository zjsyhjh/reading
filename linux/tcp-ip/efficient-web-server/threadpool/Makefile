CC = gcc
CFLAGS = -O2 -Wall -I.

LIBS = -lpthread

all: zero_threadpool.o

zero_threadpool.o: zero_threadpool.c
	$(CC) $(CFLAGS) -c zero_threadpool.c $(LIBS)

clean:
	rm -f *.o
