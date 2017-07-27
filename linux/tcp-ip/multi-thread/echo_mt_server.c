#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define BACKLOG_SIZE 5
#define MAX_CLNT 256

int clnt_cnt;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

void error_handler(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
/*
 * send to all clients
 */
void send_msg(char *msg, int len) {
	pthread_mutex_lock(&mutx);
	for (int i = 0; i < clnt_cnt; i++) {
		write(clnt_socks[i], msg, len);
	}
	pthread_mutex_unlock(&mutx);
}

void *handler_clnt(void *arg) {
	int clnt_sock = *((int*)arg);
	char msg[BUF_SIZE];
	while (1) {
		int recv_len = read(clnt_sock, msg, sizeof(msg));
		if (recv_len == -1) {
			error_handler("read() error");
		} else if (recv_len == 0) {
			break;
		}
		send_msg(msg, recv_len);
	}
	pthread_mutex_lock(&mutx);
	//remove disconnected client
	for (int i = 0; i < clnt_cnt; i++) {
		if (clnt_sock == clnt_socks[i]) {
			while (i++<clnt_cnt-1) clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}

/*
 * gcc echo_mt_server.c -o server -lpthread
 */
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL);
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

	clnt_cnt = 0;
	while (1) {
		struct sockaddr_in clnt_addr;
		socklen_t clnt_addr_sz = sizeof(clnt_addr);
		int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_t t_id;
		pthread_create(&t_id, NULL, handler_clnt, (void *)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP : %s\n", inet_ntoa(clnt_addr.sin_addr));
	}
	close(serv_sock);
	return 0;
}


