#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <time.h>
#include <errno.h>
#include "../util/zero_list.h"

#define MAX_BUF 8124

#define ZERO_HTTP_REQUEST_INVALID_METHOD 0
#define ZERO_HTTP_REQUEST_INVALID_REQUEST 1
#define ZERO_HTTP_REQUEST_INVALID_HEADER 2

/* HTTP method */
#define ZERO_HTTP_METHOD_UNKNOWN 0x0001
#define ZERO_HTTP_METHOD_GET  0x0002
#define ZERO_HTTP_METHOD_HEAD 0x0004
#define ZERO_HTTP_METHOD_POST 0x0008

/* HTTP status code */
#define ZERO_HTTP_RETURN_OK 200
#define ZERO_HTTP_RETURN_NOT_MODIFIED 304
#define ZERO_HTTP_RETURN_NOT_FOUND 404

/* RETURN value */
#define ZERO_HTTP_AGAIN EAGAIN /* try again, default 11 */
#define ZERO_HTTP_OK 1

/*
 * 首部
 */
struct zero_http_header_t {
    void *key_start;
    void *key_end;
    void *value_start;
    void *value_end;
    struct list_head list;
};

/*
 * 请求行，包含起始行以及首部
 */
struct zero_http_request_t {
    int fd;
    int epfd;
    void *root;
    int index, start, end;
    char ring_buf[MAX_BUF];
    /* first line */
    void *request_start;
    void *method_end;
    int method;    /* GET, HEAD, POST */
    void *uri_start; 
    void *uri_end;
    int http_major;
    int http_minor;
    void *request_end;
    /* header */
    struct list_head list;
    struct zero_http_header_t *header;
    struct zero_timer_node *timer_node; 
};

/*
 * 响应报文
 */
struct zero_http_return_t {
    int fd;
    int keep_alive;
    /* the modified time of the file */
    time_t m_time;
    /*
     * compare If-modified-since field with mtime \
     to decide whether the file is modified since last time 
    */
    int modified;
    int status;
};

typedef int (*zero_http_header_handler_ptr)(struct zero_http_request_t *request, struct zero_http_return_t *rt, char *data, int len);

/*
 * 处理头部
 */
struct zero_http_header_handler_t {
    char *name;
    zero_http_header_handler_ptr handler_ptr;
};
extern struct zero_http_header_handler_t zero_http_header_handlers[];

/* 处理首部 */
void zero_http_handle_header(struct zero_http_request_t *request, struct zero_http_return_t *rt);
/* 关闭http连接 */
int zero_http_close_conn(struct zero_http_request_t *request);
/* 初始化请求报文 */
int zero_init_request_t(struct zero_http_request_t *request, int fd, int epfd, struct zero_http_conf_t *cf);
/* 释放请求报文 */
int zero_free_request_t(struct zero_http_request_t *request);
/* 初始化响应报文 */
int zero_init_return_t(struct zero_http_return_t *rt, int fd);
/* 释放响应报文 */
int zero_free_return_t(struct zero_http_return_t *rt);
/* 从HTTP状态码中获得短消息 */
const char *get_shortmsg_from_status_code(int status_code);

#endif
