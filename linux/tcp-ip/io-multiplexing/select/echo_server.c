#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
#define BACKLOG_SIZE 5
void error_handler(const char *str) {
	fputs(str, stderr);
	fputc('\n', stderr);
	exit(1);
}
/*
  * gcc echo_server.c -o server
  * ./server 9090
  */
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1) {
		error_handler("socket() error");
	}
	struct sockaddr_in serv_addr, clnt_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		error_handler("bind() error");
	}
	if (listen(serv_sock, BACKLOG_SIZE) == -1) {
		error_handler("listen() error");
	}

	fd_set reads, cpy_reads;
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	int fd_max = serv_sock;

	while (1) {
		cpy_reads = reads;
		struct timeval timeout = {5, 5000};

		int fd_nums = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
		if (fd_nums == -1) {
			break;
		}
		if (fd_nums == 0) {
			continue;
		}
		for (int i = 0; i < fd_max + 1; i++) {
			if (FD_ISSET(i, &cpy_reads)) {
				if (i == serv_sock) {
					//connect request
					socklen_t addr_sz = sizeof(clnt_addr);
					int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
					FD_SET(clnt_sock, &reads);
					fd_max = (fd_max > clnt_sock ? fd_max : clnt_sock);
					printf("connected client : %d\n", clnt_sock);
				} else {
					//read message
					char buf[BUF_SIZE];
					int str_len = read(i, buf, BUF_SIZE - 1);
					if (str_len == 0) {
						//close request
						FD_CLR(i, &reads);
						close(i);
						printf("closed client : %d\n", i);
					} else {
						// echo
						buf[str_len] = '\0';
						printf("Message from client-%d: %s", i, buf);
						write(i, buf, str_len);
					}
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}