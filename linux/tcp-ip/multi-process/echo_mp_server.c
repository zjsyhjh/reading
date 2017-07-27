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

void read_childprc(int sig) {
	int status;
	pid_t pid;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		printf("removed proc id : %d\n", pid);
	}
}

void do_service(int conn) {
	while (1) {
		char buf[BUF_SIZE];
		memset(buf, 0, sizeof(buf));
		int recv_len = read(conn, buf, sizeof(buf));
		if (recv_len == -1) {
			error_handler("read() error");
		} else if (recv_len == 0) {
			//client close
			printf("client-%d disconnected...", conn);
			close(conn);
		} else {
			//echo
			fputs(buf, stdout);
			write(conn, buf, recv_len);
		}
	}
}


int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	signal(SIGCHLD, read_childprc);

	int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
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

	while (1) {
		struct sockaddr_in clnt_addr;
		socklen_t addr_sz = sizeof(clnt_addr);
		int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
		if (clnt_sock == -1) {
			error_handler("accept() error");
		}

		pid_t pid = fork();
		if (pid == -1) {
			error_handler("fork() error");
		}
		if (pid == 0) {
			//child process
			close(serv_sock);
			do_service(clnt_sock);
			exit(EXIT_SUCCESS);
		} else {
			//parent process
			close(clnt_sock);
		}
	}
	close(serv_sock);
	return 0;
}