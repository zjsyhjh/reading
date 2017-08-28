#ifndef __HTTP_H__
#define __HTTP_H__

#define FILESIZE 512
#define MIN(a, b) (a) < (b) ? (a) : (b)

struct mime_type_t {
    const char *type;
    const char *value;
};

void do_request(void *ptr);

#endif