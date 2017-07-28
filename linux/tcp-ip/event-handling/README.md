## 两种高效的事件处理模式：Reactor和Proactor

#### Reactor模式

`Reactor`是这样一种模式，它要求主线程（IO处理单元）只负责监听文件描述符上是否有事件发生，有的话就立刻将该事件通知工作线程（逻辑单元）。除此之外，主线程不做任何其他实质性的工作。读写数据、接受新的连接，以及处理客户请求均在工作线程中完成

使用同步IO模型（以`epoll_wait`为例）实现的`Reactor`模式的工作流程如下

- 主线程往`epoll`内核事件表中注册`socket`上的读就绪事件
- 主线程调用`epoll_wait`等待`socket`上有数据可读
- 当`socket`上有数据可读时，`epoll_wait`通知主线程，主线程则将`socket`可读事件放入请求队列
- 睡眠在请求队列上的某个工作线程被唤醒，它从`socket`读取数据，并处理客户请求，然后往`epoll`内核事件表中注册该`socket`上的写就绪事件
- 主线程调用`epoll_wait`等待`socket`可写
- 当`socket`可写时，`epoll_wait`通知主线程。主线程将`socket`可写事件放入请求队列
- 睡眠在请求队列上的某个工作线程被唤醒，它往`socket`上写入服务器处理客户请求的结果

![reactor](https://github.com/zjsyhjh/reading/blob/master/linux/tcp-ip/event-handling/png/reactor.png)

#### Proactor模式

与`Reactor`模式不同，`Proactor`模式将所有IO操作都交给主线程和内核来完成，工作线程仅仅负责业务逻辑。

使用异步IO模型（以`aio_read`和`aio_write`为例）实现的`Proactor`模式的工作流程

- ​