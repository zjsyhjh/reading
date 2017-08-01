/* $begin zero_epoll.h */
#ifndef __ZERO_EPOLL_H__
#define __ZERO_EPOLL_H__

#include <sys/epoll.h>

#define MAX_EVENTS 1024

int zero_epoll_create(int flags);
void zero_epoll_add(int epfd, int fd, struct epoll_event *event);
void zero_epoll_mod(int epfd, int fd, struct epoll_event *event);
void zero_epoll_del(int epfd, int fd, struct epoll_event *event);
int zero_epoll_wait(int epfd, struct epoll_event *events, int max_events, int timeout);


#endif
/* $end zero_epoll.h */