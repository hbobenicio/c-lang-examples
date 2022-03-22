#ifndef AQLS_PARSER_TOKEN_H
#define AQLS_PARSER_TOKEN_H

#include <stddef.h>
#include <stdio.h>

#include <aqls-parser/string-view.h>

typedef enum {
    AQLS_TOKEN_ERROR,
    AQLS_TOKEN_EOF,
    AQLS_TOKEN_STRING_LITERAL,
    AQLS_TOKEN_ID,
} AqlsTokenKind;

typedef struct {
    AqlsTokenKind kind;
    StringView lexeme;
} AqlsToken;

const char* aqls_token_str(AqlsTokenKind kind);

AqlsToken aqls_token_error();
AqlsToken aqls_token_eof();
AqlsToken aqls_token_id(const char* lexeme);
AqlsToken aqls_token_string_literal();

void aqls_token_fprint(FILE* stream, AqlsToken token);

#endif
