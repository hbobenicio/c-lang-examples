/**
 * This is the unit tests main source file.
 */
// libc
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

// posix
#include <errno.h>

#include "utils.h"
#include "lexer.h"
#include "string-view.h"

static void test_fixture_minimal(const char* fixtures_path, size_t fixtures_path_len)
{
    const char* fixture_file_name = "minimal.nsp";
    char fixture_file_path[2048];
    snprintf(fixture_file_path, sizeof(fixture_file_path) - 1, "%s/%s", fixtures_path, fixture_file_name);
    fprintf(stderr, "%s:%s:%s\n", __FILE__, __func__, fixture_file_path);

    char* fixture_content = file_read_or_panic(fixture_file_path);
    size_t fixture_content_len = strlen(fixture_content);

    NaturalLexer lexer = {0};
    natural_lexer_init(&lexer, fixture_content, fixture_content_len);

    assert(natural_lexer_has_more_tokens(&lexer) == true);

    NaturalToken token = {0};

    token = natural_lexer_next_token(&lexer);
    assert(token.kind == NATURAL_TOKEN_ID);
    assert(string_view_cmp_cstr(token.lexeme, "END") == 0);

    token = natural_lexer_next_token(&lexer);
    assert(token.kind == NATURAL_TOKEN_EOF);
    assert(natural_lexer_has_more_tokens(&lexer) == false);

    fputs("\n", stderr);
}

static void test_fixture_hello_world(const char* fixtures_path, size_t fixtures_path_len)
{
    const char* fixture_file_name = "hello-world.nsp";
    char fixture_file_path[2048];
    snprintf(fixture_file_path, sizeof(fixture_file_path) - 1, "%s/%s", fixtures_path, fixture_file_name);
    fprintf(stderr, "%s:%s:%s\n", __FILE__, __func__, fixture_file_path);

    char* fixture_content = file_read_or_panic(fixture_file_path);
    size_t fixture_content_len = strlen(fixture_content);

    NaturalLexer lexer = {0};
    natural_lexer_init(&lexer, fixture_content, fixture_content_len);

    assert(natural_lexer_has_more_tokens(&lexer) == true);

    NaturalToken token = {0};

    token = natural_lexer_next_token(&lexer);
    assert(token.kind == NATURAL_TOKEN_ID);
    assert(string_view_cmp_cstr(token.lexeme, "WRITE") == 0);

    token = natural_lexer_next_token(&lexer);
    assert(token.kind == NATURAL_TOKEN_STRING_LITERAL);
    assert(string_view_cmp_cstr(token.lexeme, "'HELLO, WORLD!'") == 0);

    token = natural_lexer_next_token(&lexer);
    assert(token.kind == NATURAL_TOKEN_ID);
    assert(string_view_cmp_cstr(token.lexeme, "END") == 0);

    token = natural_lexer_next_token(&lexer);
    assert(token.kind == NATURAL_TOKEN_EOF);
    assert(natural_lexer_has_more_tokens(&lexer) == false);

    fputs("\n", stderr);
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
