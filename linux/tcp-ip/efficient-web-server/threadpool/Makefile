CC = gcc
CFLAGS = -O2 -Wall -I.

LIBS = -lpthread

all: zero_threadpool.o zero_task.o

zero_threadpool.o: zero_threadpool.c
	$(CC) $(CFLAGS) -c zero_threadpool.c $(LIBS)

zero_task.o: zero_task.c
	$(CC) $(CFLAGS) -c zero_task.c
	
clean:
	rm -f *.o
