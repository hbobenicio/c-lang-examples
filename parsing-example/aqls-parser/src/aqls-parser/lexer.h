#ifndef AQLS_PARSER_LEXER_H
#define AQLS_PARSER_LEXER_H

// libc
#include <stddef.h>
#include <stdbool.h>

// posix
#include <regex.h>

#include "token/token.h"
#include "token/queue.h"

#define AQLS_LEXER_PATTERNS_LEN 2

typedef struct {
    enum aqls_token_kind token_kind;
    regex_t regex;
} AqlsLexerPattern;

typedef struct {
    const char* input;
    size_t input_len;
    
    const char* cursor;
    size_t pos;

    regex_t ignore;
    AqlsLexerPattern patterns[AQLS_LEXER_PATTERNS_LEN];

    struct aqls_token_queue lookahead;

} AqlsLexer;

void aqls_lexer_init(AqlsLexer* lexer, const char* input, size_t input_len);
void aqls_lexer_free(AqlsLexer* lexer);

bool aqls_lexer_has_more_tokens(const AqlsLexer* lexer);
AqlsToken aqls_lexer_peek_token(AqlsLexer* lexer);
size_t aqls_lexer_peek_tokens(AqlsLexer* lexer, size_t n, AqlsToken* tokens);

AqlsToken aqls_lexer_next_token(AqlsLexer* lexer);

void aqls_lexer_get_line_column(AqlsLexer* lexer, size_t* out_line, size_t* out_column);
void aqls_lexer_get_line_column_from_pos(AqlsLexer* lexer, size_t pos, size_t* out_line, size_t* out_column);
void aqls_lexer_get_line_column_from_cursor(AqlsLexer* lexer, const char* cursor, size_t* out_line, size_t* out_column);


#endif
