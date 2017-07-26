#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 4
#define BACKLOG_SIZE 5
#define EPOLL_SIZE 50

void set_nonblocking_mode(int fd) {
	int flag = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}

void error_handler(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

/*
 * Edge Trigger
 * gcc echo_ET_server.c -o server
 * ./server 9090
 */
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

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

	int epfd = epoll_create(EPOLL_SIZE);
	struct epoll_event *ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
	struct epoll_event event;

	set_nonblocking_mode(serv_sock);
	event.events = EPOLLIN;
	event.data.fd = serv_sock;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event) == -1) {
		error_handler("epoll_ctl() error");
	}

	while (1) {
		int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if (event_cnt == -1) {
			error_handler("epoll_wait() error");
		}

		puts("return epoll_wait()");
		for (int i = 0; i < event_cnt; i++) {
			if (ep_events[i].data.fd == serv_sock) {
				// connect request
				socklen_t addr_sz = sizeof(clnt_addr);
				int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
				//Edge Trigger
				set_nonblocking_mode(clnt_sock);
				event.events = EPOLLIN | EPOLLET;
				event.data.fd = clnt_sock;
				if (epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event) == -1) {
					error_handler("epoll_ctl() error");
				}
				printf("connect client: %d\n", clnt_sock);
			} else {
				//read message
				while (1) {
					char buf[BUF_SIZE];
					int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE - 1);
					if (str_len == 0) {
						//close request
						if (epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL) == -1) {
							error_handler("epoll_ctl() error");
						}
						close(ep_events[i].data.fd);
						printf("closed client: %d\n", ep_events[i].data.fd);
					} else if (str_len < 0) {
						if (errno == EAGAIN) {
							//no message to read
							break;
						} else {
							error_handler("read() error");
						}
					} else {
						//echo server
						buf[str_len] = '\0';
						printf("Message from client-%d: %s\n", ep_events[i].data.fd, buf);
						write(ep_events[i].data.fd, buf, str_len);
					}
				}
				
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}