#ifndef NATURAL_PARSER_LEXER_H
#define NATURAL_PARSER_LEXER_H

// libc
#include <stddef.h>
#include <stdbool.h>

// posix
#include <regex.h>

#include "token/token.h"
#include "token/queue.h"

#define NATURAL_LEXER_PATTERNS_LEN 2

typedef struct {
    NaturalTokenKind token_kind;
    regex_t regex;
} NaturalLexerPattern;

typedef struct {
    const char* input;
    size_t input_len;
    
    const char* cursor;
    size_t pos;

    regex_t ignore;
    NaturalLexerPattern patterns[NATURAL_LEXER_PATTERNS_LEN];

    NaturalTokenQueue lookahead;

} NaturalLexer;

void natural_lexer_init(NaturalLexer* lexer, const char* input, size_t input_len);
void natural_lexer_free(NaturalLexer* lexer);

bool natural_lexer_has_more_tokens(const NaturalLexer* lexer);
NaturalToken natural_lexer_peek_token(NaturalLexer* lexer);
size_t natural_lexer_peek_tokens(NaturalLexer* lexer, size_t n, NaturalToken* tokens);

NaturalToken natural_lexer_next_token(NaturalLexer* lexer);

void natural_lexer_get_line_column(NaturalLexer* lexer, size_t* out_line, size_t* out_column);
void natural_lexer_get_line_column_from_pos(NaturalLexer* lexer, size_t pos, size_t* out_line, size_t* out_column);
void natural_lexer_get_line_column_from_cursor(NaturalLexer* lexer, const char* cursor, size_t* out_line, size_t* out_column);


#endif
