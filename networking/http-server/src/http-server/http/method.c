#include "method.h"

#include <assert.h>

static const char *const http_method_to_cstr_map[HTTP_METHOD_COUNT] = {
    [HTTP_METHOD_GET] = "GET",
    [HTTP_METHOD_PUT] = "PUT",
    [HTTP_METHOD_POST] = "POST",
    [HTTP_METHOD_HEAD] = "HEAD",
    [HTTP_METHOD_TRACE] = "TRACE",
    [HTTP_METHOD_DELETE] = "DELETE",
    [HTTP_METHOD_CONNECT] = "CONNECT",
    [HTTP_METHOD_OPTIONS] = "OPTIONS",
};

const char* http_method_cstr(enum http_method method)
{
    assert(method < HTTP_METHOD_COUNT);
    return http_method_to_cstr_map[method];
}

