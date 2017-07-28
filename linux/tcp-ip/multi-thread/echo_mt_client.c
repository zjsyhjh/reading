#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define NAME_SIZE 30
char name[NAME_SIZE] = "[DEFAULT]";

void error_handler(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void *send_msg(void *arg) {
	int sock = *((int*)arg);
	char name_msg[NAME_SIZE + BUF_SIZE];
	while (1) {
		char msg[BUF_SIZE];
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
			close(sock);
			exit(0);
		}
		sprintf(name_msg, "%s %s", name, msg);
		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}


void *recv_msg(void *arg) {
	int sock = *((int*)arg);
	char name_msg[NAME_SIZE + BUF_SIZE];
	while (1) {
		int recv_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
		if (recv_len == -1) {
			return (void*)-1;
		}
		name_msg[recv_len] = '\0';
		fputs(name_msg, stdout);
	}
	return NULL;
}

/*
 * gcc echo_mt_client.c -o client -lpthread
 */
int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}
	sprintf(name, "[%s]", argv[3]);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		error_handler("connect() error");
	}

	pthread_t send_thread, recv_thread;
	pthread_create(&send_thread, NULL, send_msg, (void *)&sock);
	pthread_create(&recv_thread, NULL, recv_msg, (void *)&sock);
	void *thread_return;
	pthread_join(send_thread, &thread_return);
	pthread_join(recv_thread, &thread_return);
	close(sock);
	return 0;
}