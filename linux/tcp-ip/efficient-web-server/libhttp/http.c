#include <sys/epoll.h>
#include "http.h"
#include "http_parse.h"
#include "http_request.h"

struct mime_type_t zero_mime[] = 
{
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/msword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"},
    {NULL ,"text/plain"}
};

static char *ROOT = NULL;

void do_request(void *ptr) 
{
    struct zero_http_request_t *r = (struct zero_http_request_t *)ptr;
    int fd = r->fd;
    int rc, n, remain_size;
    char filename[FILESIZE];
    struct stat sbuf;
    ROOT = r->root;
    char *plast = NULL;
    
    zero_timer_del(r);

    for(;;) {
        plast = &r->ring_buf[r->end % MAX_BUF];
        remain_size = MIN(MAX_BUF - (r->end - r->start) - 1, MAX_BUF - r->end % MAX_BUF);

        n = read(fd, plast, remain_size);
        assert(r->end - r->start < MAX_BUF);

        if (n == 0) {   
            // EOF
            log_info("read return 0, ready to close fd %d, remain_size = %zu", fd, remain_size);
            goto err;
        }

        if (n < 0) {
            if (errno != EAGAIN) {
                log_err("read err, and errno = %d", errno);
                goto err;
            }
            break;
        }

        r->end += n;
        check(r->end - r->start < MAX_BUF, "request buffer overflow!");
        
        log_info("ready to parse request line"); 

        rc = zero_http_parse_request_line(r);
        if (rc == ZERO_HTTP_AGAIN) {
            continue;
        } else if (rc != ZERO_HTTP_OK) {
            log_err("rc != ZERO_HTTP_OK");
            goto err;
        }

        log_info("method == %.*s", (int)(r->method_end - r->request_start), (char *)r->request_start);
        log_info("uri == %.*s", (int)(r->uri_end - r->uri_start), (char *)r->uri_start);

        debug("ready to parse request header");
        rc = zero_http_parse_request_headers(r);
        if (rc == ZERO_HTTP_AGAIN) {
            continue;
        } else if (rc != ZERO_HTTP_OK) {
            log_err("rc != ZV_OK");
            goto err;
        }
        
        /*
        *   handle http header
        */
        zv_http_out_t *out = (zv_http_out_t *)malloc(sizeof(zv_http_out_t));
        if (out == NULL) {
            log_err("no enough space for zv_http_out_t");
            exit(1);
        }

        rc = zv_init_out_t(out, fd);
        check(rc == ZV_OK, "zv_init_out_t");

        parse_uri(r->uri_start, r->uri_end - r->uri_start, filename, NULL);

        if(stat(filename, &sbuf) < 0) {
            do_error(fd, filename, "404", "Not Found", "zaver can't find the file");
            continue;
        }

        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
        {
            do_error(fd, filename, "403", "Forbidden",
                    "zaver can't read the file");
            continue;
        }
        
        out->mtime = sbuf.st_mtime;

        zv_http_handle_header(r, out);
        check(list_empty(&(r->list)) == 1, "header list should be empty");
        
        if (out->status == 0) {
            out->status = ZV_HTTP_OK;
        }

        serve_static(fd, filename, sbuf.st_size, out);

        if (!out->keep_alive) {
            log_info("no keep_alive! ready to close");
            free(out);
            goto close;
        }
        free(out);

    }
    
    struct epoll_event event;
    event.data.ptr = ptr;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;

    zv_epoll_mod(r->epfd, r->fd, &event);
    zv_add_timer(r, TIMEOUT_DEFAULT, zv_http_close_conn);
    return;

err:
close:
    rc = zv_http_close_conn(r);
    check(rc == 0, "do_request: zv_http_close_conn");
}




