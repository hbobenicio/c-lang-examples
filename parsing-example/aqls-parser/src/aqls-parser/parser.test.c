#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "utils.h"
#include "string-view.h"

#define PRINT_OK fprintf(stderr, "✅ %s \n", __func__)

static void test_fixture_minimal(struct strview fixtures_path)
{
    char fixture_file_path[2048] = {0};
    snprintf(fixture_file_path, sizeof(fixture_file_path) - 1, "%s/minimal.nsp", fixtures_path.str);

    char* fixture_content = file_read_or_panic(fixture_file_path);
    size_t fixture_content_len = strlen(fixture_content);

    struct aqls_parser parser = {0};
    aqls_parser_init(&parser, string_view_from_cstr(fixture_file_path));

    assert(aqls_parser_parse(&parser, fixture_content, fixture_content_len) == 0);

    struct aqls_ast* ast = parser.ast;
    assert(ast != NULL);

    aqls_parser_free(&parser);

    free(fixture_content);

    PRINT_OK;
}

static void test_fixture_hello_world(struct strview fixtures_path)
{
    char fixture_file_path[1024] = {0};
    snprintf(fixture_file_path, sizeof(fixture_file_path) - 1, "%s/hello-world.nsp", fixtures_path.str);

    char* fixture_content = file_read_or_panic(fixture_file_path);
    size_t fixture_content_len = strlen(fixture_content);

    struct aqls_parser parser = {0};
    aqls_parser_init(&parser, string_view_from_cstr(fixture_file_path));

    assert(aqls_parser_parse(&parser, fixture_content, fixture_content_len) == 0);

    free(fixture_content);

    PRINT_OK;
}

int main(int argc, char* argv[])
{
    assert(argc == 2);

    struct strview fixtures_path = {
        .str = argv[1],
        .len = strlen(argv[1])
    };

    fprintf(stderr, "%s:\n", __FILE__);
    test_fixture_minimal(fixtures_path);
    test_fixture_hello_world(fixtures_path);

    return 0;
}
