## IO复用之SELECT

#### Select函数的调用方法和顺序

- 步骤一
  - 设置文件描述符
  - 指定监视范围及设置超时
- 步骤二
  - 调用`select`函数
- 步骤三
  - 查看调用结果

##### 1.1 设定文件描述符

​	利用`select`函数可以同时监视多个文件描述符，首先需要将要监视的文件描述符集中到一起，使用`fd_set`数组变量执行此操作，该数组是存0和1的位数组，如果数组中某位置为1，则表示该文件描述符是监视对象。针对`fd_set`变量中注册或更改值的操作都由下列宏完成：

- `FD_ZERO(fd_set *fdset)`：将`fd_set`变量中的所有位初始化为0
- `FD_SET(int fd, fd_set *fdset)`：在参数`fdset`指向的变量中注册文件描述符fd的信息
- `FD_CLR(int fd, fd_set *fdset)`：从参数`fdset`指向的变量中清除文件描述符fd的信息
- `FD_ISSET(int fd, fd_set *fdset)`：若参数 `fdset`指向的变量中包含文件描述符fd的信息，则返回”真“，该函数的主要作用是用于验证`select`函数的调用结果

##### 1.2 设置监视范围及超时

```c
#include <sys/select.h>
#include <sys/time.h>

/*
 * 成功返回大于0的值（发生事件的文件描述符数），失败时返回-1
 * maxfd - 监视对象文件描述符数量
 * readset - 将所有关注“是否存在待读取数据”的文件描述符注册到fd_set型变量，并传递其地址值
 * writeset - 将所有关注“是否可传输无阻塞数据”的文件描述符注册到fd_set型变量，并传递其地址值
 * exceptset - 将所有关注“是否发生异常”的文件描述符注册到fd_set型变量，并传递其地址值
 * timeout - 调用select函数后，为防止陷入无限阻塞的状态，传递超时信息
 */
int select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout);

//timeval结构体
struct timeval {
  long tv_sec; //seconds
  long tv_usec; //microseconds
}
```

