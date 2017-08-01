#include <stdlib.h>

#include "zero_epoll.h"
#include "../debug/debug.h"

struct epoll_event *events;

int zero_epoll_create(int flags) {
	int epfd;
	if ((epfd = epoll_create1(flags)) == -1) {
		unix_error("zero_epoll_create() error");
	}
	events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * MAX_EVENTS);
	return epfd;
}

void zero_epoll_add(int epfd, int fd, struct epoll_event *event) {
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, events) == -1) {
		unix_error("zero_epoll_add() error");
	}
	return;
}

void zero_epoll_mod(int epfd, int fd, struct epoll_event *event) {
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, events) == -1) {
		unix_error("zero_epoll_mod() error");
	}
	return;
}

void zero_epoll_del(int epfd, int fd, struct epoll_event *event) {
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, events) == -1) {
		unix_error("zero_epoll_del() error");
	}
	return;
}

int zero_epoll_wait(int epfd, struct epoll_event *events, int max_events, int timeout) {
	int cnt;
	if ((cnt = epoll_wait(epfd, events, max_events, timeout)) == -1) {
		unix_error("zero_epoll_wait() error");
	}
	return cnt;
}
