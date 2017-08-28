#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "../util/zero_list.h"

#define MAX_BUF 8124

#define ZERO_HTTP_REQUEST_INVALID_METHOD 0
#define ZERO_HTTP_REQUEST_INVALID_REQUEST 1
#define ZERO_HTTP_REQUEST_INVALID_HEADER 2

#define ZERO_HTTP_METHOD_UNKNOWN 0x0001
#define ZERO_HTTP_METHOD_GET  0x0002
#define ZERO_HTTP_METHOD_HEAD 0x0004
#define ZERO_HTTP_METHOD_POST 0x0008

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



#endif
