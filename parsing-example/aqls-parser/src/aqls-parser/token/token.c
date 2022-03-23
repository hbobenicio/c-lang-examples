#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* aqls_token_str(enum aqls_token_kind kind)
{
    switch (kind)
    {
    case AQLS_TOKEN_ERROR:
        return "ERROR";

    case AQLS_TOKEN_EOF:
        return "EOF";

    case AQLS_TOKEN_STRING_LITERAL:
        return "STRING_LITERAL";

    case AQLS_TOKEN_ID:
        return "ID";

    default:
        fprintf(stderr, "error: aqls: token: failed to get string representation of unknown token. kind=%d\n", kind);
        exit(EXIT_FAILURE);
    }
}

void aqls_token_fprint(FILE* stream, AqlsToken token)
{
    fprintf(stream, "<%s", aqls_token_str(token.kind));

    switch (token.kind) {
    case AQLS_TOKEN_STRING_LITERAL:
    case AQLS_TOKEN_ID:
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

AqlsToken aqls_token_eof()
{
    return (AqlsToken) {
        .kind = AQLS_TOKEN_EOF,
        .lexeme = {0},
    };
}

AqlsToken aqls_token_error()
{
    return (AqlsToken) {
        .kind = AQLS_TOKEN_ERROR,
        .lexeme = {0},
    };
}

AqlsToken aqls_token_string_literal()
{
    return (AqlsToken) {
        .kind = AQLS_TOKEN_STRING_LITERAL,
        .lexeme = {0},
    };
}

AqlsToken aqls_token_id(const char* lexeme)
{
    return (AqlsToken) {
        .kind = AQLS_TOKEN_ID,
        .lexeme = string_view_from_cstr(lexeme),
    };
}
