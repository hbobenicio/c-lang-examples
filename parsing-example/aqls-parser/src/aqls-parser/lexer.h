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

struct aqls_lexer_pattern {
    enum aqls_token_kind token_kind;
    regex_t regex;
};

struct aqls_lexer {
    const char* input;
    size_t input_len;
    
    const char* cursor;
    size_t pos;

    regex_t ignore;
    struct aqls_lexer_pattern patterns[AQLS_LEXER_PATTERNS_LEN];

    struct aqls_token_queue lookahead;
};

void aqls_lexer_init(struct aqls_lexer* lexer, const char* input, size_t input_len);
void aqls_lexer_free(struct aqls_lexer* lexer);

bool aqls_lexer_has_more_tokens(const struct aqls_lexer* lexer);
struct aqls_token aqls_lexer_peek_token(struct aqls_lexer* lexer);
size_t aqls_lexer_peek_tokens(struct aqls_lexer* lexer, size_t n, struct aqls_token* tokens);

struct aqls_token aqls_lexer_next_token(struct aqls_lexer* lexer);

void aqls_lexer_get_line_column(struct aqls_lexer* lexer, size_t* out_line, size_t* out_column);
void aqls_lexer_get_line_column_from_pos(struct aqls_lexer* lexer, size_t pos, size_t* out_line, size_t* out_column);
void aqls_lexer_get_line_column_from_cursor(struct aqls_lexer* lexer, const char* cursor, size_t* out_line, size_t* out_column);


#endif
