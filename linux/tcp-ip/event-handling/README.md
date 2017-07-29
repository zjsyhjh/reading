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

![reactor](https://github.com/zjsyhjh/reading/blob/master/linux/tcp-ip/event-handling/png/reactor.png?raw=true)

#### Proactor模式

与`Reactor`模式不同，`Proactor`模式将所有IO操作都交给主线程和内核来完成，工作线程仅仅负责业务逻辑。

使用异步IO模型（以`aio_read`和`aio_write`为例）实现的`Proactor`模式的工作流程

- 主线程调用`aio_read`函数向内核注册`socket`上的读完成事件，并告诉内核用户读缓冲区的位置，以及读操作完成时如何通知应用程序（以信号为例）

- 主线程继续处理其他逻辑

- 当`socket`上的数据被读入用户缓冲区后，内核将向应用程序发送一个信号，以通知应用程序数据已经可用

- 应用程序预先定义好的信号处理函数选择一个工作线程来处理客户请求。工作线程处理完客户请求之后，调用`aio_write`函数向内核注册`socket`上的写完成事件，并告诉内核用户写缓冲区的位置，以及写操作完成时如何通知应用程序

- 主线程继续处理其他逻辑

- 当用户缓冲区的数据被写入`socket`之后，内核将向应用程序发送一个信号，以通知应用程序数据已经发送完毕

- 应用程序预先定义好的信号处理函数选择一个工作线程来做善后处理，比如决定是否关闭`socket`

  ​

![proator](https://github.com/zjsyhjh/reading/blob/master/linux/tcp-ip/event-handling/png/proactor.png?raw=true)



#### 高效的半同步/半异步模式

半同步/半异步模式中，同步线程用于处理客户逻辑，异步线程用于处理IO事件，异步线程监听到客户请求后，将其封装成请求对象并插入请求队列中。请求队列将通知某个工作在同步模式的工作线程来读取并处理该请求对象

如下图所示，主线程只负责监听`socket`，连接`socket`由工作线程来管理。当有新的连接到来时，主线程就接受并将新返回的连接`socket`派发给某个工作线程，此后该新`socket`上的任何IO操作都由被选中的工作线程来处理，直到客户关闭连接。主线程向工作线程派发`socket`的最简单的方式，是往它和工作线程之间的管道里写数据。工作线程检测到管道上有数据可读，就分析是否是一个新的客户连接到来。如果是，则把该新`socket`上的读写事件注册到自己的`epoll`内核事件表中

![processpool](https://github.com/zjsyhjh/reading/blob/master/linux/tcp-ip/event-handling/png/processpool.png?raw=true)



