#pragma once

enum http_method {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_PUT,
    HTTP_METHOD_POST,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_TRACE,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_OPTIONS,
};
#define HTTP_METHOD_COUNT 8

const char* http_method_cstr(enum http_method method);

