#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string-view.h"

static NaturalAstCompilationUnit* parse_compilation_unit(NaturalParser* parser);
static NaturalAstStatementList* parse_statements(NaturalParser* parser);
static NaturalAstStatement* parse_statement(NaturalParser* parser);
static NaturalAstStatement* parse_statement_write(NaturalParser* parser);
static NaturalAstEnd* parse_end(NaturalParser* parser);

static void panic_unexpected_token(NaturalParser* parser, NaturalToken* expected_tokens, size_t expected_tokens_len);

void natural_parser_init(NaturalParser* parser)
{
    parser->ast = NULL;
}

void natural_parser_free(NaturalParser* parser)
{
    natural_lexer_free(&parser->lexer);
    // TODO ast is leaking ATM. traverse it and free all the nodes! (with a visitor maybe)
}

/**
 * <Ast> := <CompilationUnit> EOF
 */
int natural_parser_parse(NaturalParser* parser, const char* input, size_t input_len)
{
    natural_lexer_init(&parser->lexer, input, input_len);

    NaturalAstCompilationUnit* compilation_unit = parse_compilation_unit(parser);
    if (compilation_unit == NULL) {
        return 1;
    }
    parser->ast = natural_ast_new(compilation_unit);

    return 0;
}

/**
 * <CompilationUnit> := <StatementList> <End>
 */
static NaturalAstCompilationUnit* parse_compilation_unit(NaturalParser* parser)
{
    NaturalAstStatementList* statements = parse_statements(parser);
    NaturalAstEnd* end = parse_end(parser);

    return natural_ast_compilation_unit_new(statements, end);
}

/**
 * <StatementList> := <Statement>*
 */
static NaturalAstStatementList* parse_statements(NaturalParser* parser)
{
    NaturalAstStatementList *statements = NULL, *last = NULL;
    NaturalAstStatement* statement = NULL;

    while ((statement = parse_statement(parser)) != NULL) {
        last = natural_ast_statements_append(last, statement);
    }

    return statements;
}

/**
 * <Statement> := <StatementWrite> | ... (more yet to come)
 */
static NaturalAstStatement* parse_statement(NaturalParser* parser)
{
    NaturalAstStatement* statement = NULL;

    NaturalToken lookahead = natural_lexer_peek_token(&parser->lexer);
    if (lookahead.kind == NATURAL_TOKEN_ID && string_view_cmp_sized_cstr(lookahead.lexeme, "WRITE", 5) == 0) {
        statement = parse_statement_write(parser);
    }

    return statement;
}

/**
 * <StatementWrite> := WRITE STRING_LITERAL
 */
static NaturalAstStatement* parse_statement_write(NaturalParser* parser)
{
    // can't fail. we've just checked it
    // WRITE
    NaturalToken write_token = natural_lexer_next_token(&parser->lexer);

    // STRING_LITERAL
    NaturalToken operand_token = natural_lexer_next_token(&parser->lexer);
    if (operand_token.kind != NATURAL_TOKEN_STRING_LITERAL) {
        NaturalToken expected_tokens[] = {
            natural_token_string_literal(),
        };
        panic_unexpected_token(parser, expected_tokens, sizeof(expected_tokens) / sizeof(NaturalToken));
    }

    return natural_ast_statement_write_new(write_token, operand_token);
}

/**
 * <End> := END
 */
static NaturalAstEnd* parse_end(NaturalParser* parser)
{
    NaturalToken end_token = natural_lexer_next_token(&parser->lexer);
    if (end_token.kind != NATURAL_TOKEN_ID || string_view_cmp_sized_cstr(end_token.lexeme, "END", 3) != 0) {
        NaturalToken expected_tokens[] = {
            natural_token_id("END"),
        };
        panic_unexpected_token(parser, expected_tokens, sizeof(expected_tokens) / sizeof(NaturalToken));
    }

    return natural_ast_end_new(end_token);
}

static void panic_unexpected_token(NaturalParser* parser, NaturalToken* expected_tokens, size_t expected_tokens_len)
{
    size_t line, column;
    natural_lexer_get_line_column(&parser->lexer, &line, &column);

    fprintf(stderr, "error: natural: parser: unexpected token at line %zu, column %zu. expecting tokens [", line, column);
    for (size_t i = 0; i < expected_tokens_len; i ++) {
        if (i != 0) {
            fputs(", ", stderr);
        }
        natural_token_fprint(stderr, expected_tokens[i]);
    }
    fprintf(stderr, "], but '%c' was found.\n", parser->lexer.cursor[0]);

    natural_parser_free(parser);
    exit(EXIT_FAILURE);
}
