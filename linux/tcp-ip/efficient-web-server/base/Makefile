CC = gcc
CFLAGS = -O2 -Wall -I.

OBJS = zero_rio.o

all: $(OBJS)

zero_rio.o: zero_rio.c
	$(CC) $(CFLAGS) -c zero_rio.c

clean:
	rm -f *.o
