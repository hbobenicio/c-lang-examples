#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string-view.h"

static struct aqls_ast_compilation_unit* parse_compilation_unit(AqlsParser* parser);
static struct aqls_ast_stmt_list* parse_statements(AqlsParser* parser);
static struct aqls_ast_stmt* parse_statement(AqlsParser* parser);
static struct aqls_ast_stmt* parse_statement_write(AqlsParser* parser);
static struct aqls_ast_end* parse_end(AqlsParser* parser);

static void panic_unexpected_token(AqlsParser* parser, AqlsToken* expected_tokens, size_t expected_tokens_len);

void aqls_parser_init(AqlsParser* parser)
{
    parser->ast = NULL;
}

void aqls_parser_free(AqlsParser* parser)
{
    aqls_lexer_free(&parser->lexer);
    // TODO ast is leaking ATM. traverse it and free all the nodes! (with a visitor maybe)
}

/**
 * <Ast> := <CompilationUnit> EOF
 */
int aqls_parser_parse(AqlsParser* parser, const char* input, size_t input_len)
{
    aqls_lexer_init(&parser->lexer, input, input_len);

    struct aqls_ast_compilation_unit* compilation_unit = parse_compilation_unit(parser);
    if (compilation_unit == NULL) {
        return 1;
    }
    parser->ast = aqls_ast_new(compilation_unit);

    return 0;
}

/**
 * <CompilationUnit> := <StatementList> <End>
 */
static struct aqls_ast_compilation_unit* parse_compilation_unit(AqlsParser* parser)
{
    struct aqls_ast_stmt_list* statements = parse_statements(parser);
    struct aqls_ast_end* end = parse_end(parser);

    return aqls_ast_compilation_unit_new(statements, end);
}

/**
 * <StatementList> := <Statement>*
 */
static struct aqls_ast_stmt_list* parse_statements(AqlsParser* parser)
{
    struct aqls_ast_stmt_list *statements = NULL, *last = NULL;
    struct aqls_ast_stmt* statement = NULL;

    while ((statement = parse_statement(parser)) != NULL) {
        last = aqls_ast_statements_append(last, statement);
    }

    return statements;
}

/**
 * <Statement> := <StatementWrite> | ... (more yet to come)
 */
static struct aqls_ast_stmt* parse_statement(AqlsParser* parser)
{
    struct aqls_ast_stmt* statement = NULL;

    AqlsToken lookahead = aqls_lexer_peek_token(&parser->lexer);
    if (lookahead.kind == AQLS_TOKEN_ID && string_view_cmp_sized_cstr(lookahead.lexeme, "WRITE", 5) == 0) {
        statement = parse_statement_write(parser);
    }

    return statement;
}

/**
 * <StatementWrite> := WRITE STRING_LITERAL
 */
static struct aqls_ast_stmt* parse_statement_write(AqlsParser* parser)
{
    // can't fail. we've just checked it
    // WRITE
    AqlsToken write_token = aqls_lexer_next_token(&parser->lexer);

    // STRING_LITERAL
    AqlsToken operand_token = aqls_lexer_next_token(&parser->lexer);
    if (operand_token.kind != AQLS_TOKEN_STRING_LITERAL) {
        AqlsToken expected_tokens[] = {
            aqls_token_string_literal(),
        };
        panic_unexpected_token(parser, expected_tokens, sizeof(expected_tokens) / sizeof(AqlsToken));
    }

    return aqls_ast_statement_write_new(write_token, operand_token);
}

/**
 * <End> := END
 */
static struct aqls_ast_end* parse_end(AqlsParser* parser)
{
    AqlsToken end_token = aqls_lexer_next_token(&parser->lexer);
    if (end_token.kind != AQLS_TOKEN_ID || string_view_cmp_sized_cstr(end_token.lexeme, "END", 3) != 0) {
        AqlsToken expected_tokens[] = {
            aqls_token_id("END"),
        };
        panic_unexpected_token(parser, expected_tokens, sizeof(expected_tokens) / sizeof(AqlsToken));
    }

    return aqls_ast_end_new(end_token);
}

static void panic_unexpected_token(AqlsParser* parser, AqlsToken* expected_tokens, size_t expected_tokens_len)
{
    size_t line, column;
    aqls_lexer_get_line_column(&parser->lexer, &line, &column);

    fprintf(stderr, "error: aqls: parser: unexpected token at line %zu, column %zu. expecting tokens [", line, column);
    for (size_t i = 0; i < expected_tokens_len; i ++) {
        if (i != 0) {
            fputs(", ", stderr);
        }
        aqls_token_fprint(stderr, expected_tokens[i]);
    }
    fprintf(stderr, "], but '%c' was found.\n", parser->lexer.cursor[0]);

    aqls_parser_free(parser);
    exit(EXIT_FAILURE);
}
