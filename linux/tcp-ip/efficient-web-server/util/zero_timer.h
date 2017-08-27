#ifndef __ZERO_TIMER_H__
#define __ZERO_TIMER_H__

#include "../libhttp/http_request.h"

typedef int (*timer_handler_ptr)(zero_http_request_t *request);

struct zero_timer_node {
    size_t key;
    timer_handler_ptr handler_ptr;
    zero_http_request_t *request_ptr;
    /* if remote client close the socket first, set deleted to 1 */
    int deleted;
};

void zero_timer_init() {
}

void zero_timer_add(zero_http_request_t *request_ptr, size_t timeout, timer_handler_ptr handler_ptr) {
}

void zero_timer_del(zero_http_request_t *request_ptr) {
}

int zero_find_timer();

void zero_handle_expire_timers();


#endif
