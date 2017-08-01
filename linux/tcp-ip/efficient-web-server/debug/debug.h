/* $begin debug.h */
#ifndef __DEBUG_H__
#define __DEBUG_H__

/* error-handling functions */
void unix_error(char *msg);
void posix_error(int code, char *msg);
void dns_error(char *msg);
void gai_error(int code, char *msg);
void app_error(char *msg);

#endif
/* $end debug.h */