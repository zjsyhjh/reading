CC = gcc
CFLAGS = -O2 -Wall -I.

all: http_parse.o http_request.o http.o

http_parse.o: http_parse.c
	$(CC) $(CFLAGS) -c http_parse.c

http_request.o: http_request.c
	$(CC) $(CFLAGS) -c http_parse.c

http.o: http.c
	$(CC) $(CFLAGS) -c http.c
	
clean:
	rm -f *.o
