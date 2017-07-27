#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define BACKLOG_SIZE 5

void error_handler(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void read_process(int sockfd) {
	while (1) {
		char buf[BUF_SIZE];
		memset(buf, 0, sizeof(buf));
		int recv_len = read(sockfd, buf, sizeof(buf));
		if (recv_len == -1) {
			error_handler("read() error");
		} else if (recv_len == 0) {
			return;
		} else {
			buf[recv_len] = '\0';
			printf("Message from server: %s", buf);
		}
	}
}

void write_process(int sockfd) {
	while (1) {
		char message[BUF_SIZE];
		fgets(message, BUF_SIZE, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
			shutdown(sockfd, SHUT_WR);
			return;
		}
		write(sockfd, message, strlen(message));
	}
}


int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		error_handler("socket() error");
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		error_handler("connect() error");
	}
	printf("connected...\n");

	pid_t pid = fork();
	if (pid == -1) {
		error_handler("fork() error");
	} else if (pid == 0) {
		//child process
		write_process(sock);
	} else {
		//parent process
		read_process(sock);
	}
	close(sock);
	return 0;
}