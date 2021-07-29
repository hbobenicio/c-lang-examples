#ifndef NATURAL_PARSER_TOKEN_H
#define NATURAL_PARSER_TOKEN_H

#include <stddef.h>
#include <stdio.h>

#include <natural-parser/string-view.h>

typedef enum {
    NATURAL_TOKEN_ERROR,
    NATURAL_TOKEN_EOF,
    NATURAL_TOKEN_STRING_LITERAL,
    NATURAL_TOKEN_ID,
} NaturalTokenKind;

typedef struct {
    NaturalTokenKind kind;
    StringView lexeme;
} NaturalToken;

const char* natural_token_str(NaturalTokenKind kind);

NaturalToken natural_token_error();
NaturalToken natural_token_eof();
NaturalToken natural_token_id(const char* lexeme);
NaturalToken natural_token_string_literal();

void natural_token_fprint(FILE* stream, NaturalToken token);

#endif
