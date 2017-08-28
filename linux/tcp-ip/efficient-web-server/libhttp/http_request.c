#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <math.h>
#include <unistd.h>
#include "http_request.h"
#include "http.h"
#include "../util/zero_list.h"

/* handle function */
static int zero_http_process_ignore(zero_http_request_t *request, zero_http_return_t *rt, char *data, int len);
static int zero_http_process_connection(zero_http_request_t *request, zero_http_return_t *rt, char *data, int len);
static int zero_http_process_if_modified_since(zero_http_request_t *request, zero_http_return_t *rt, char *data, int len);

zero_http_header_handler_t zero_http_header_handlers[] = {
    {"Host", zero_http_process_ignore},
    {"Connection", zero_http_process_connection},
    {"If-Modified-Since", zero_http_process_if_modified_since},
    {"", zero_http_process_ignore} 
};

int zero_init_request_t(zero_http_request_t *request, int fd, int epfd, zero_http_conf_t *cf) {
    request->fd = fd;
    request->epfd = epfd;
    request->start = request->end = 0;
    request->index = 0;
    request->root = cf->root;
    INIT_LIST_HEAD(&(request->list));

    return ZERO_HTTP_OK; 
}

int zero_init_return_t(zero_http_return_t *rt, int fd) {
    rt->fd = fd;
    rt->keep_alive = 0;
    rt->modified = 1;
    rt->status = 0;

    return ZERO_HTTP_OK;
}

void zero_http_handle_header(zero_http_request_t *request, zero_http_return_t *rt) {
    list_head *pos;
    zero_http_header_handler_t *header_handler;
    int len;

    list_for_each(pos, &(request->list)) {
        /* 得到pos指向的list所在对象 */
        zero_http_header_t *hd = list_entry(pos, zero_http_header_t, list);
        /* handle */
        for (header_handler = zero_http_header_handlers; strlen(header_handler->name) > 0; header_handler++) {
            if (strncmp(hd->key_start, header_handler->name, hd->key_end - hd->key_start) == 0) {
                len = hd->value_end - hd->value_start;
                (*(header_handler->handler_ptr))(request, rt, hd->value_start, len);
                break;
            }
        }

        list_del(pos);
        free(hd);
    }
}

int zero_http_close_conn(zero_http_request_t *request) {
    close(request->fd);
    free(request);
    return ZERO_HTTP_OK;
}



static int zero_http_process_ignore(zero_http_request_t *request, zero_http_return_t *rt, char *data, int len) {
    return ZERO_HTTP_OK;
}

static int zero_http_process_connection(zero_http_request_t *request, zero_http_return_t *rt, char *data, int len) {
    if (strncasecmp("keep-alive", date, len) == 0) {
        rt->keep_alive = 1;
    }
    return ZERO_HTTP_OK;
}

static int zero_http_process_if_modified_since(zero_http_request_t *request, zero_http_return_t *rt, char *data, int len) {
    struct tm tm;
    if (strptime(data, "%a, %d %b %Y %H:%M:%S GMT", &tm) == (char *)NULL) {
        return ZERO_HTTP_OK;
    }
    time_t client_time = mktime(&tm);

    double time_diff = difftime(out->mtime, client_time);
    if (fabs(time_diff) < 1e-6) {
        // log_info("content not modified clienttime = %d, mtime = %d\n", client_time, out->mtime);
        /* Not modified */
        rt->modified = 0;
        rt->status = ZERO_HTTP_RETURN_NOT_MODIFIED;
    }
    
    return ZERO_HTTP_OK;
}

const char *get_shortmsg_from_status_code(int status_code) {
    if (status_code == ZERO_HTTP_OK) return "OK";
    if (status_code == ZV_HTTP_NOT_MODIFIED) return "Not Modified";
    if (status_code == ZV_HTTP_NOT_FOUND) return "Not Found";
    return "Unknown";
}





