#ifndef __HTTP_PARSE_H__
#define __HTTP_PARSE_H__

#include "http_request.h"
/* 
 * http协议头的字段，是在请求(request)或响应(respone)行（一条消息的第一行内容）之后传输的
 * 协议头的字段是以明文的字符串格式传输，以冒号分隔的键名与键值对，以回车(CR)加换行(LF)符号序列结尾
 * 协议头部分的结尾以一个空白字段标识，结果就是，也就是传输两个连续的CR+LF
 */
#define CR '\r'
#define LF '\n'
#define CRLF '\r\n'
#define CRLFCRLF '\r\n\r\n'

/* http请求行（响应行） */
int zero_http_parse_request_line(struct zero_http_request_t *request);
/* http请求首部（响应首部） */
int zero_http_parse_request_headers(struct zero_http_request_t *request);

#endif
