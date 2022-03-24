#ifndef AQLS_PARSER_TOKEN_H
#define AQLS_PARSER_TOKEN_H

#include <stddef.h>
#include <stdio.h>

#include <aqls-parser/string-view.h>

enum aqls_token_kind {
    AQLS_TOKEN_ERROR,
    AQLS_TOKEN_EOF,
    AQLS_TOKEN_STRING_LITERAL,
    AQLS_TOKEN_ID,
};

struct aqls_token {
    enum aqls_token_kind kind;
    struct strview lexeme;
};

const char* aqls_token_str(enum aqls_token_kind kind);

struct aqls_token aqls_token_error();
struct aqls_token aqls_token_eof();
struct aqls_token aqls_token_id(const char* lexeme);
struct aqls_token aqls_token_string_literal();

void aqls_token_fprint(FILE* stream, struct aqls_token token);

#endif
