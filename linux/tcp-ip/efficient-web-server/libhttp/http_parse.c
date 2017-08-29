#include "http_parse.h"
#include "http_request.h"

/*
 * 请求行组成
 * 请求方法|空格|URI|空格|协议版本|回车符|换行符
 * GET /index.html HTTP/1.1
 */
enum {
    line_start = 0,
    line_method,
    line_space_before_uri,
    line_after_slash_in_uri,
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
} line_index;

int zero_http_parse_request_line(struct zero_http_request_t *request)
{
    char ch, *p, *q;
    int pos;
    line_index = request->index;

    for (pos = request->start; pos < request->end; pos++) {
        p = &request->ring_buf[pos % MAX_BUF];
        ch = *p;
        

        switch(line_index) {
            /* http methods: GET, HEAD, POST */
            case line_start:
               request->request_start = p;

               if (ch == CR || ch == LF) break;
               if ((ch < 'A' || ch > 'Z') && ch != '_') return ZERO_HTTP_REQUEST_INVALID_METHOD; 

                line_index = line_method;
                break;
            
            case line_method:
                if (ch == ' ') {
                    request->method_end = p;
                    q = request->start;
                    
                    request->method = ZERO_HTTP_METHOD_UNKNOWN;
                    switch(p - q) {
                        case 3:
                            if (zero_str_cmp(q, p, "GET")) {
                                request->method = ZERO_HTTP_METHOD_GET;
                            }
                            break;
                        case 4:
                            if (zero_str_cmp(q, p, "HEAD")) {
                                request->method = ZERO_HTTP_METHOD_HEAD;
                            }
                            if (zero_str_cmp(q, p, "POST")) {
                                request->method = ZERO_HTTP_METHOD_POST;
                            }
                            break;
                    }
                    line_index = line_space_before_uri;
                    break;
                }
                if ((ch < 'A' || ch > 'Z') && ch != '_') return ZERO_HTTP_REQUEST_INVALID_METHOD;
                break;
            /* space before uri */
            case line_space_before_uri:
                if (ch == '/') {
                    request->uri_start = p;
                    line_index = line_after_slash_in_uri;
                    break;
                }
                switch (ch) {
                    case ' ':
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_after_slash_in_uri:
                switch (ch) {
                    case ' ':
                        request->uri_end = p;
                        line_index = line_http;
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
                        line_index = line_http_h;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_h:
                switch (ch) {
                    case 'T':
                        line_index = line_http_ht;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_ht:
                switch (ch) {
                    case 'T':
                        line_index = line_http_htt;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_htt:
                switch (ch) {
                    case 'P':
                        line_index = line_http_http;
                        break;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_REQUEST;
                }
                break;
            case line_http_http:
                switch (ch) {
                    case '/':
                        line_index = line_http_first_major_digit;
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
                line_index = line_http_major_digit;
                break;
            /* major HTTP version or dot */
            case line_http_major_digit:
                if (ch == '.') {
                    line_index = line_http_first_minor_digit;
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
                line_index = line_http_minor_digit;
                break;
            /* minor HTTP version or end of request line */
            case line_http_minor_digit:
                if (ch == CR) {
                    line_index = line_almost_done;
                    break;
                }
                if (ch == LF) {
                    goto done;
                }

                if (ch == ' ') {
                    line_index = line_spaces_after_digit;
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
                        line_index = line_almost_done;
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
    request->index = line_index;
    return ZERO_HTTP_AGAIN;

done:
    request->start = pos + 1;
    request->index = line_start;
    return ZERO_HTTP_OK;
}

/*
 * 请求头部组成
 * 头部字段名：值|回车符|换行符
 * ...
 * 回车符|换行符
 * Host: www.baidu.com
 * Accept: text/html, image/gif, image/jpg
 * Accept-language: en
 */
enum {
    header_start = 0,
    header_key,
    header_space_before_colon,
    header_space_after_colon,
    header_value,
    header_cr,
    header_crlf,
    header_crlfcr
} header_index;

int zero_http_parse_request_headers(struct zero_http_request_t *request)
{
    header_index = request->index;
    assert(header_index == 0);

    struct zero_http_header_t *hd;
    void *hd_key_start, *hd_key_end, *hd_value_start, *hd_value_end;
    char ch, *p, *q;
    int pos;

    for (pos = request->start; pos < request->end; pos++) {
        p = &request->ring_buf[pos % MAX_BUF];
        ch = *p;

        switch(header_index) {
            case header_start:
                if (ch == CR || ch == LF) break;
                hd_key_start = p;
                header_index = header_key;
                break;
            case header_key:
                if (ch == ' ') {
                    hd_key_end = p;
                    header_index = header_space_before_colon;
                    break;
                }
                if (ch == ':') {
                    hd_key_end =p;
                    header_index = header_space_after_colon;
                    break;
                }
                break;
            case header_space_before_colon:
                if (ch == ' ') break;
                if (ch == ':') {
                    header_index = header_space_after_colon;
                    break;
                }
                return ZERO_HTTP_REQUEST_INVALID_HEADER;
            case header_space_after_colon:
                if (ch == ' ') break;
                header_index = header_value;
                hd_value_start = p;
                break;
            case header_value:
                if (ch == CR) {
                    hd_value_end = p;
                    header_index = header_cr;
                }
                if (ch == LF) {
                    hd_value_end = p;
                    header_index = header_crlf;
                }
                break;
            case header_cr:
                if (ch == LF) {
                    header_index = header_crlf;
                    hd = (struct zero_http_header_t *)malloc(sizeof(struct zero_http_header_t));
                    hd->key_start = hd_key_start;
                    hd->key_end = hd_key_end;
                    hd->value_start = hd_value_start;
                    hd->value_end = hd_value_end;
                    request->header = hd;
                    list_add(&(hd->list), &(request->list));
                    break;
                }
                return ZERO_HTTP_REQUEST_INVALID_HEADER;
            case header_crlf:
                if (ch == CR) {
                    header_index = header_crlfcr;
                } else {
                    hd_key_start = p;
                    header_index = header_start;
                }
                break;
            case header_crlfcr:
                switch(ch) {
                    case LF:
                        goto done;
                    default:
                        return ZERO_HTTP_REQUEST_INVALID_HEADER;
                }
                break;
            default:
                break;
        }
    }
    request->start = pos;
    request->index = header_index;

    return ZERO_HTTP_AGAIN;

done:
    request->start = pos + 1;
    request->index = header_start;

    return ZERO_HTTP_OK;
}



