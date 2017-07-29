#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "processpool.h"
#define BACKLOG_SIZE 5

class cgi_conn {
public:
	cgi_conn(){}
	~cgi_conn(){}

	void init(int epollfd, int sockfd, const sockaddr_in& clnt_addr) {
		m_epollfd = epollfd;
		m_sockfd = sockfd;
		m_addr = clnt_addr;
		memset(m_buf, 0, BUF_SIZE);
		m_read_index = 0;
	}

	void process() {
		while (1) {
			int index = m_read_index;
			int ret = recv(m_sockfd, m_buf + index, BUF_SIZE - 1 - index, 0);
			if (ret < 0) {
				if (errno != EAGAIN) {
					removefd(m_epollfd, m_sockfd);
				}
				break;
			} else if (ret == 0) {
				removefd(m_epollfd, m_sockfd);
				break;
			} else {
				m_read_index += ret;
				printf("User content is : %s\n", m_buf);
				for ( ; index < m_read_index; index++) {
					if ((index >= 1) && (m_buf[index - 1] == '\r') && (m_buf[index] == '\n')) {
						break;
					}
				}
				if (index == m_read_index) {
					continue;
				}
				m_buf[index - 1] = '\0';
				char *file_name = m_buf;
				if (access(file_name, F_OK) == -1) {
					removefd(m_epollfd, m_sockfd);
					break;
				}

				ret = fork();
				if (ret == -1) {
					removefd(m_epollfd, m_sockfd);
					break;
				} else if (ret > 0) {
					removefd(m_epollfd, m_sockfd);
					break;
				} else {
					close(STDOUT_FILENO);
					dup(m_sockfd);
					execl(m_buf, m_buf, 0);
					exit(0);
				}	
			}
		}
	}
private:
	static int m_epollfd;
	int m_sockfd;
	sockaddr_in m_addr;
	char m_buf[BUF_SIZE];
	int m_read_index;
};


int cgi_conn::m_epollfd = -1;

/*
 * 
 */
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <port> \n", basename(argv[0]));
		exit(1);
	}
	int port =  atoi(argv[1]);

	int listenfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(listenfd >= 0);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	int ret = bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
	assert(ret != -1);

	ret = listen(listenfd, BACKLOG_SIZE);
	assert(ret != -1);

	processpool<cgi_conn > *pool = processpool<cgi_conn>::create(listenfd);
	if (pool) {
		pool->run();
		delete pool;
	}
	close(listenfd);
	return 0;
}