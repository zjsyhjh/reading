#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handler(const char *str) {
	fputs(str, stderr);
	fputc('\n', stderr);
	exit(1);
}

/*
 * gcc echo_client.c -o client
 * ./client 127.0.0.1 9090
 */
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
	printf("connected...");

	while (1) {
		fputs("Input message(Q to quit): ", stdout);
		char message[BUF_SIZE];
		fgets(message, BUF_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
			break;
		}

		write(sock, message, strlen(message));
		int str_len = read(sock, message, BUF_SIZE);
		message[str_len] = '\0';
		printf("Message from server: %s", message);
	}
	close(sock);
	return 0;
}