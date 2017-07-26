## IO复用之EPOLL

#### 基于select的IO复用技术速度慢的原因

- 调用`select`函数后常见的针对所有文件描述符的循环语句
- 每次调用`select`函数时都需要向该函数传递监视对象信息（主要原因）
  - 解决方案（`epoll`的特点）：仅向操作系统传递1次监视对象，监视范围或内容发生变化时只通知发生变化的事项

#### 实现epoll时必要的函数和结构体

- 无需编写以监视状态变化为目的的针对所有文件描述符的循环语句
- 调用对应于`select`函数的`epoll_wait`函数时无需每次传递监视对象信息

`epoll`服务器端实现中需要用到的3个函数

- `epoll_create`：创建保存`epoll`文件描述符的空间
  - `select`方式中为了保存监视对象文件描述符，直接声明了`fd_set`变量，但`epoll`方式下操作系统负责保存监视对象文件描述符，因此需要向操作系统请求创建保存文件描述符的空间
- `epoll_ctl`：向空间注册并注销文件描述符
  - 为了添加和删除监视对象文件描述符，`select`方式中需要`FD_SET`、`FD_CLR`函数，但在`epoll`方式中，通过`epoll_ctl`函数请求操作系统完成
- `epoll_wait`：与`select`函数类似，等待文件描述符发生变化
  - `select`方式下调用`select`函数等待文件描述符的变化，而`epoll`中调用`epoll_wait`函数，并通过结构体`epoll_event`将发生变化的（发生事件的）文件描述符单独集中到一起

```c
#include <sys/epoll.h>

/*
 * 成功时返回epoll文件描述符，失败时返回-1
 * 通过参数size的值来决定epoll例程的大小，但该值只是向操作系统提建议，并不能真正决定
 */
int epoll_create(int size);

/*
 * 成功时返回0，失败时返回-1
 * epfd - 用于注册监视对象的epoll例程的文件描述符
 * op - 用于指定监视对象的添加、删除或是更改等操作，如EPOLL_CTL_ADD, EPOLL_CTL_DEL, EPOLL_CTL_MOD
 * fd - 需要注册的监视对象文件描述符 
 * event - 监视对象的事件类型
 */
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
/*
 * epoll_event结构体用于保存发生事件的文件描述符集合
 * 声明足够大的epoll_event结构体数组，传递给epoll_wait函数时，只有发生变化的文件描述符信息被填入数组
 */
struct epoll_event {
  __uint32_t events;
  epoll_data_t data;
};
typedef union epoll_data {
  void *ptr;
  int fd;
  __uint32_t u32;
  __uint64_t u64;
} epoll_data_t;

/* 
 * 示例
 * 将sockfd注册到epoll例程epfd中，并在需要读取数据的情况下产生相应事件
 * events中可以保存的常量及所指的事件类型
 * EPOLLIN - 需要读取数据的情况
 * EPOLLOUT - 输出缓冲为空，可以立即发生数据的情况
 * EPOLLET -  以边缘触发的方式得到事件通知 
 * EPOLLRDHUP - 断开连接或半关闭的情况，这在边缘触发方式下非常有用
 */
struct epoll_event event;
event.events = EPOLLIN; //发生需要读取数据的事件时
event.data.fd = sockfd;
epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

/*
 * 成功时返回发生事件的文件描述符数，失败时返回-1
 * epfd - 表示事件发生监视范围的epoll例程的文件描述符
 * events - 保存发生事件的文件描述符集合的结构体地址值
 * maxevents - 第二个参数中可以保存的最大事件数
 * timeout - 以1/1000秒为单位的等待时间，传递-1时，一直等待直到发生事件 
 */
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

/*
 * 示例
 */
int event_cnt;
struct epoll_event *ep_events;
ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE); //EPOLL_SIZE是宏常量
event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
```
####  水平或条件触发（Level Trigger）和边缘触发（Edge Trigger）

条件触发和边缘触发的区别在于发生事件的时间点（epoll默认以条件触发方式工作）

- 条件触发方式中，只要输入缓冲有数据，就会以事件方式不断注册
- 边缘触发方式中，输入缓冲收到数据时仅注册1次该事件。即使输入缓冲中还留有数据，也不会再进行注册

边缘触发的服务器端实现中必知的两点

- 通过`errno`变量验证错误原因
- 为了完成非阻塞（`Non-blocking`）I/O，更改套接字特性

```c
#include <fcntl.h>

/*
 * 成功时返回cmd参数相关值，失败时返回-1
 * filedes - 属性更改目标的文件描述符 
 * cmd - 表示函数调用的目的
 */
int fcntl(int filedes, int cmd, ...);
/* 
 * 示例：将文件（套接字）改为非阻塞模式
 * 通过第一条语句获取之前设置的属性信息
 * 通过第二条语句在此基础上添加非阻塞O_NONBLOCK标志
 * 调用read & write函数时，无论是否存在数据，都会形成非阻塞文件（或套接字）
 */
int flag = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flag|O_NONBLOCK);
```

