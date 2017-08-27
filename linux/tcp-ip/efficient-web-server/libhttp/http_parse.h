#ifndef __HTTP_PARSE_H__
#define __HTTP_PARSE_H__


#define CR '\r'
#define LF '\n'
#define CRLF '\r\n'

int zero_http_parse_request_line(zero_http_request_t *request);
int zero_http_parse_request_body(zero_http_request_t *request);

#endif