CC = gcc
CFLAGS = -O2 -Wall -I.

OBJS = zero_list.o zero_priority_queue.o

all: $(OBJS)

zero_list.o: zero_list.c
	$(CC) $(CFLAGS) -c zero_list.c

zero_priority_queue.o: zero_priority_queue.c
	$(CC) $(CFLAGS) -c zero_priority_queue.c

clean:
	rm -f *.o
