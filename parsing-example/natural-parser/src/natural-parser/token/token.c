#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* natural_token_str(NaturalTokenKind kind)
{
    switch (kind)
    {
    case NATURAL_TOKEN_ERROR:
        return "ERROR";

    case NATURAL_TOKEN_EOF:
        return "EOF";

    case NATURAL_TOKEN_STRING_LITERAL:
        return "STRING_LITERAL";

    case NATURAL_TOKEN_ID:
        return "ID";

    default:
        fprintf(stderr, "error: natural: token: failed to get string representation of unknown token. kind=%d\n", kind);
        exit(EXIT_FAILURE);
    }
}

void natural_token_fprint(FILE* stream, NaturalToken token)
{
    fprintf(stream, "<%s", natural_token_str(token.kind));

    switch (token.kind) {
    case NATURAL_TOKEN_STRING_LITERAL:
    case NATURAL_TOKEN_ID:
        if (token.lexeme.len > 0) {
            fputc('(', stream);
            string_view_fwrite(token.lexeme, stream);
            fputc(')', stream);
        }

    default:
        break;
    }

    fputs("> ", stream);
}

NaturalToken natural_token_eof()
{
    return (NaturalToken) {
        .kind = NATURAL_TOKEN_EOF,
        .lexeme = {0},
    };
}

NaturalToken natural_token_error()
{
    return (NaturalToken) {
        .kind = NATURAL_TOKEN_ERROR,
        .lexeme = {0},
    };
}

NaturalToken natural_token_string_literal()
{
    return (NaturalToken) {
        .kind = NATURAL_TOKEN_STRING_LITERAL,
        .lexeme = {0},
    };
}

NaturalToken natural_token_id(const char* lexeme)
{
    return (NaturalToken) {
        .kind = NATURAL_TOKEN_ID,
        .lexeme = string_view_from_cstr(lexeme),
    };
}
