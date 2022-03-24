#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "utils.h"
#include "string-view.h"

static void test_fixture_minimal(const char* fixtures_path, size_t fixtures_path_len)
{
    (void) fixtures_path_len;

    const char* fixture_file_name = "minimal.nsp";
    char fixture_file_path[2048];
    snprintf(fixture_file_path, sizeof(fixture_file_path) - 1, "%s/%s", fixtures_path, fixture_file_name);
    fprintf(stderr, "%s:%s:%s\n", __FILE__, __func__, fixture_file_path);

    char* fixture_content = file_read_or_panic(fixture_file_path);
    size_t fixture_content_len = strlen(fixture_content);

    struct aqls_parser parser = {0};
    aqls_parser_init(&parser);

    assert(aqls_parser_parse(&parser, fixture_content, fixture_content_len) == 0);

    struct aqls_ast* ast = parser.ast;
    assert(ast != NULL);

    aqls_parser_free(&parser);
}

static void test_fixture_hello_world(const char* fixtures_path, size_t fixtures_path_len)
{
    (void) fixtures_path_len;
    
    const char* fixture_file_name = "hello-world.nsp";
    char fixture_file_path[2048];
    snprintf(fixture_file_path, sizeof(fixture_file_path) - 1, "%s/%s", fixtures_path, fixture_file_name);
    fprintf(stderr, "%s:%s:%s\n", __FILE__, __func__, fixture_file_path);

    char* fixture_content = file_read_or_panic(fixture_file_path);
    size_t fixture_content_len = strlen(fixture_content);

    struct aqls_parser parser = {0};
    aqls_parser_init(&parser);

    assert(aqls_parser_parse(&parser, fixture_content, fixture_content_len) == 0);
}

int main(int argc, char* argv[])
{
    assert(argc == 2);
    const char* fixtures_path = argv[1];
    size_t fixtures_path_len = strlen(fixtures_path);

    test_fixture_minimal(fixtures_path, fixtures_path_len);
    test_fixture_hello_world(fixtures_path, fixtures_path_len);

    return 0;
}
