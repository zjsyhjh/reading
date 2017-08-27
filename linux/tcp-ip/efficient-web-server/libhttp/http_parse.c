#include "http_parse.h"
#include "http_request.h"

enum {
    line_start = 0,
    line_method,
    line_space_before_uri,
    line_space_slash_in_uri,
    line_http,
    line_http_h,
    line_http_ht,
    line_http_htt,
    line_http_http,
    line_http_first_major_digit,
    line_http_major_digit,
    line_http_first_minor_digit,
    line_http_minor_digit,
    line_spaces_after_digit,
    line_almost_done
} index;

/*
 * 请求行组成
 * 请求方法|空格|URI|空格|协议版本|回车符|换行符
 * 请求头部组成
 * 头部字段名：值|回车符|换行符
 * ...
 * 回车符|换行符
 */
int zero_http_parse_request_line(zero_http_request_t *request)
{
    char ch, *p, *q;
    int pos;
    index = request->index;

    for (pos = request->start; pos < request->end; pos++) {
        p = &request->ring_buf[pos % MAX_BUF];
        ch = *p;
        

        switch(index) {
            /* http methods: GET, HEAD, POST */
            case line_start:
               request->request_start = p;

               if (ch == CR || ch == LF) break;
               if ((ich < 'A' || ch > 'Z') && ch != '_') return ZERO_HTTP_REQUEST_INVALID_METHOD; 

                index = line_method;
                break;
            
            case line_method:
                if (ch == ' ') {
                    request->method_end = p;
                    q = request->start;
                    
                    request->method = ZERO_HTTP_METHOD_UNKNOWN;
                    switch(p - m) {
                        case 3:
                            if (zero_str_cmp(m, p, "GET")) {
                                request->method = ZERO_HTTP_METHOD_GET;
                            }
                            break;
                        case 4:
                            if (zero_str_cmp(m, p, "HEAD")) {
                                request->method = ZERO_HTTP_METHOD_HEAD;
                            }
                            if (zero_str_cmp(m, p, "POST")) {
                                request->method = ZERO_HTTP_METHOD_POST;
                            }
                            break;
                    }
                    index = line_space_before_uri;
                    break;
                }
                if ((ch < 'A' || ch > 'Z') && ch != '_') return ZERO_HTTP_REQUEST_INVALID_METHOD;
                break;
            /* space before uri */
            case line_space_before_uri:
                if (ch == '/') {
                    request->uri_start = p;
                    index = line_after_slash_in_uri;
                    break;
                }
                switch (ch) {
                    case ' ':
                        break;
                    default:
                        return ZV_HTTP_PARSE_INVALID_REQUEST;
                }
                break;
            case line_after_slash_in_uri:
                switch (ch) {
                    case ' ':
                        r->uri_end = p;
                        index = line_http;
                        break;
                    default:
                        break;
                }
                break;
            /* space after uri */
            case line_http:
                switch (ch) {
                    case ' ':
                        break;
                    case 'H':
                        index = line_http_h;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_h:
                switch (ch) {
                    case 'T':
                        index = line_http_ht;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_ht:
                switch (ch) {
                    case 'T':
                        index = line_http_htt;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_htt:
                switch (ch) {
                    case 'P':
                        index = line_http_http;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_http:
                switch (ch) {
                    case '/':
                        index = line_http_first_major_digit;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            /* first digit of major HTTP version */
            case line_http_first_major_digit:
                if (ch < '1' || ch > '9') {
                    return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }

                request->http_major = ch - '0';
                index = line_http_major_digit;
                break;
            /* major HTTP version or dot */
            case line_http_major_digit:
                if (ch == '.') {
                    index = line_http_first_minor_digit;
                    break;
                }

                if (ch < '0' || ch > '9') {
                    return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }

                request->http_major = request->http_major * 10 + ch - '0';
                break;
            /* first digit of minor HTTP version */
            case line_http_first_minor_digit:
                if (ch < '0' || ch > '9') {
                    return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }

                request->http_minor = ch - '0';
                index = line_http_minor_digit;
                break;
            /* minor HTTP version or end of request line */
            case line_http_minor_digit:
                if (ch == CR) {
                    index = line_almost_done;
                    break;
                }
                if (ch == LF) {
                    goto done;
                }

                if (ch == ' ') {
                    index = line_spaces_after_digit;
                    break;
                }

                if (ch < '0' || ch > '9') {
                    return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }

                request->http_minor = request->http_minor * 10 + ch - '0';
                break;
            case line_spaces_after_digit:
                switch (ch) {
                    case ' ':
                        break;
                    case CR:
                        index = line_almost_done;
                        break;
                    case LF:
                        goto done;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            /* end of request line */
            case line_almost_done:
                request->request_end = p - 1;
                switch (ch) {
                    case LF:
                        goto done;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            default:
                break;
        }
    }
    
    request->start = pos;
    request->index = index;
    return ZERO_HTTP_AGAIN;

done:
    request->start = pos + 1;
    request->index = line_start;
    return ZERO_HTTP_OK;
}



