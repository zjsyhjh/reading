#ifndef __ZERO_TIMER_H__
#define __ZERO_TIMER_H__

#include <sys/time.h>
#include <assert.h>

#include "zero_priority_queue.h"
#include "../libhttp/http_request.h"

typedef int (*timer_handler_ptr)(zero_http_request_t *request);

struct zero_timer_node {
    size_t key;
    timer_handler_ptr handler_ptr;
    zero_http_request_t *request_ptr;
    /* if remote client close the socket first, set deleted to 1 */
    int deleted;
};

zero_pq_t zero_timer;
size_t zero_current_msec;


int zero_timer_cmp(void *p, void *q) {
    struct zero_timer_node *node_p = (struct zero_timer_node*)p;
    struct zero_timer_node *node_q = (struct zero_timer_node*)q;
    return node_p->key < node_q->key;
}

static void zero_timer_update() {
    struct timeval tv;
    int rc;
    if ((rc = gettimeofday(&tv, NULL)) != 0) {
        unix_error("gettimeofday error"); 
    }
    zero_current_msec = tv.tv_sec * 1000 + tv.tv_nsec / 1000;
}

void zero_timer_init() {
    zero_pq_init(&zero_time, zero_timer_cmp, PQ_DEFAULT_CAPACITY);
    zero_timer_update();
}

/*
 * insert timer_node to zero_timer heap;
 */
void zero_timer_add(zero_http_request_t *request_ptr, size_t timeout, timer_handler_ptr handler_ptr) {
    zero_timer_node *timer_node = (struct zero_timer_node *)malloc(sizeof(zero_timer_node));
    assert(timer_node != NULL);

    zero_timer_update();

    request_ptr->timer_node = timer_node;
    
    timer_node->key = zero_current_msec + timeout;
    timer_node->deleted = 0;
    timer_node->handler_ptr = handler_ptr;
    
    zero_pq_insert(&zero_timer, timer_node);
}

/*
 * to delete an zero_http_request, set request_ptr->timer_node->deleted = 1
 */
void zero_timer_del(zero_http_request_t *request_ptr) {
    zero_timer_update();
    assert(request_ptr->timer_node != NULL);

    request_ptr->timer_node->deleted = 1;
}

int zero_find_timer() 
{
    int zero_timer_time = -1;
    while (!zero_pq_empty(&zero_timer)) {
        zero_timer_update();

        zero_timer_node *timer_node = (struct zero_timer_node *)zero_pq_min(&zero_timer);
        assert(timer_node != NULL);

        if (timer_node->deleted) {
            zero_pq_delmin(&zero_timer);
            free(timer_node);
            continue; 
        } 

        //find a node that is not deleted.
        zero_timer_time = (int)(timer_node->key - zero_current_msec);
        zero_timer_time = (zero_timer_time > 0 ? zero_timer_time : 0);
        break;
    }
    return zero_timer_time;
}

/*
 * delete timer_node that it's key <= zero_current_msec
 */
void zero_handle_expire_timers() 
{
    while (!zero_pq_empty(&zero_timer)) {
        zero_timer_update();

        zero_timer_node *timer_node = (struct zero_timer_node *)zero_pq_min(&zero_timer);
        assert(timer_node != NULL);

        if (timer_node->deleted) {
            zero_pq_delmin(&zero_timer);
            free(timer_node);
            continue; 
        }
        if (timer_node->key > zero_current_msec) return;

        // timer_node->key <= zero_current_msec, need to handle
        if (timer_node->handler_ptr) {
            timer_node->handler_ptr(timer_node->request_ptr);
        }

        zero_pq_delmin(&zero_timer);
        free(timer_node);
    }
}


#endif
