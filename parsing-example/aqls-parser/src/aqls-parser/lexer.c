#include "lexer.h"

// libc
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// posix
#include <sys/types.h>
#include <regex.h>

#include "utils.h"

#define AQLS_LOOKAHEAD 10

static void regex_compile_or_die(regex_t* regex, const char* pattern, int flags);
static void skip_ignored(AqlsLexer* lexer);
static void advance(AqlsLexer* lexer, size_t offset);
static void advance_token(AqlsLexer* lexer, AqlsToken token);

void aqls_lexer_init(AqlsLexer* lexer, const char* input, size_t input_len)
{
    // input initialization
    lexer->input = input;
    lexer->input_len = input_len;

    // offsets initialization
    lexer->cursor = input;
    lexer->pos = 0;
    
    // lookahead initialization
    aqls_token_queue_init(&lexer->lookahead, AQLS_LOOKAHEAD);

    // regex initialization
    // TODO check if I need another flag for keep scanning beyond the new lines
    regex_compile_or_die(&lexer->ignore, "^[ \t\r\n]+", REG_EXTENDED);

    regex_compile_or_die(&lexer->patterns[0].regex, "^'[^']*'", REG_EXTENDED);
    lexer->patterns[0].token_kind = AQLS_TOKEN_STRING_LITERAL;

    regex_compile_or_die(&lexer->patterns[1].regex, "^[a-zA-Z#][a-zA-Z#\\.]*", REG_EXTENDED | REG_ICASE);
    lexer->patterns[1].token_kind = AQLS_TOKEN_ID;
}

bool aqls_lexer_has_more_tokens(const AqlsLexer* lexer)
{
    return lexer->pos < lexer->input_len;
}

AqlsToken aqls_lexer_peek_token(AqlsLexer* lexer)
{
    // if we have the token already in our lookahead queue, just return it right away
    if (!aqls_token_queue_is_empty(&lexer->lookahead)) {
        return aqls_token_queue_peek(&lexer->lookahead);
    }

    if (!aqls_lexer_has_more_tokens(lexer)) {
        AqlsToken eof = aqls_token_eof();
        aqls_token_queue_append(&lexer->lookahead, eof);
        return eof;
    }

    // peek function should not advance the input cursor in any way. Make a copy of the Scanning Position State here
    const char* cursor = lexer->cursor;
    size_t pos = lexer->pos;

    // TODO review this one! gotta continue scanning after the ignored patterns, but we shall not advance anymore
    //skip_ignored(lexer);
    {
        regmatch_t match;
        int rc = regexec(&lexer->ignore, cursor, 1, &match, REG_NOTEOL);
        if (rc != REG_NOMATCH) {
            int match_len = match.rm_eo - match.rm_so;
            assert(match_len > 0);
            assert(pos + match_len <= lexer->input_len);

            pos += (size_t) match_len;
            cursor += (size_t) match_len;
        }
    }
    
    if (pos >= lexer->input_len) {
        AqlsToken eof = aqls_token_eof();
        aqls_token_queue_append(&lexer->lookahead, eof);
        return eof;
    }

    for (size_t i = 0; i < AQLS_LEXER_PATTERNS_LEN; i++) {
        regmatch_t match;
        int rc = regexec(&lexer->patterns[i].regex, cursor, 1, &match, REG_NOTEOL);
        if (rc == REG_NOMATCH) {
            continue;
        }

        int match_len = match.rm_eo - match.rm_so;
        assert(match_len > 0);

        AqlsToken token = {
            .kind = lexer->patterns[i].token_kind,
            .lexeme = (StringView) {
                .str = cursor,
                .len = (size_t) match_len,
            },
        };

        aqls_token_queue_append(&lexer->lookahead, token);
        return token;
    }

    // TODO should I save the byte position here? should it be in the token?
    AqlsToken error_token = {
        .kind = AQLS_TOKEN_ERROR,
        .lexeme = (StringView) {
            .str = cursor,
            .len = 0,
        },
    };
    aqls_token_queue_append(&lexer->lookahead, error_token);
    return error_token;
}

size_t aqls_lexer_peek_tokens(AqlsLexer* lexer, size_t n, AqlsToken* tokens)
{
    (void) lexer;
    (void) n;
    (void) tokens;

    //TODO implement
    
    return 0;
}

AqlsToken aqls_lexer_next_token(AqlsLexer* lexer)
{
    if (!aqls_token_queue_is_empty(&lexer->lookahead)) {
        AqlsToken token = aqls_token_queue_pop(&lexer->lookahead);
        if (token.lexeme.str != NULL) {
            size_t offset = 0;
            for (const char* c = lexer->cursor; c != token.lexeme.str; c++) {
                offset += 1;
            }
            advance(lexer, offset);
            advance_token(lexer, token);
        }
        return token;
    }

    if (!aqls_lexer_has_more_tokens(lexer)) {
        return aqls_token_eof();
    }

    skip_ignored(lexer);

    if (!aqls_lexer_has_more_tokens(lexer)) {
        return aqls_token_eof();
    }

    for (size_t i = 0; i < AQLS_LEXER_PATTERNS_LEN; i++) {
        regmatch_t match;
        int rc = regexec(&lexer->patterns[i].regex, lexer->cursor, 1, &match, REG_NOTEOL);
        if (rc == REG_NOMATCH) {
            continue;
        }

        int match_len = match.rm_eo - match.rm_so;
        assert(match_len > 0);

        AqlsToken token = {
            .kind = lexer->patterns[i].token_kind,
            .lexeme = (StringView) {
                .str = lexer->cursor,
                .len = (size_t) match_len,
            },
        };

        advance_token(lexer, token);
        return token;
    }

    AqlsToken error_token = {
        .kind = AQLS_TOKEN_ERROR,
        .lexeme = (StringView) {
            .str = lexer->cursor,
            .len = 0,
        },
    };
    // return aqls_token_error();
    return error_token;
}

void aqls_lexer_free(AqlsLexer* lexer)
{
    regfree(&lexer->ignore);
    for (size_t i = 0; i < AQLS_LEXER_PATTERNS_LEN; i++) {
        regfree(&lexer->patterns[i].regex);
    }
    
    lexer->input = NULL;
    lexer->input_len = 0;

    lexer->cursor = NULL;
    lexer->pos = 0;

    aqls_token_queue_free(&lexer->lookahead);
}

static void skip_ignored(AqlsLexer* lexer)
{
    regmatch_t match;
    int rc = regexec(&lexer->ignore, lexer->cursor, 1, &match, REG_NOTEOL);
    if (rc == REG_NOMATCH) {
        return;
    }

    int match_len = match.rm_eo - match.rm_so;
    assert(match_len > 0);
    advance(lexer, (size_t) match_len);
}

static void advance(AqlsLexer* lexer, size_t offset)
{
    assert(lexer->pos + offset <= lexer->input_len);

    lexer->pos += offset;
    lexer->cursor += offset;
}

static void advance_token(AqlsLexer* lexer, AqlsToken token)
{
    assert(lexer->pos + token.lexeme.len <= lexer->input_len);

    advance(lexer, token.lexeme.len);
}

static void regex_compile_or_die(regex_t* regex, const char* pattern, int flags)
{
    int rc = regcomp(regex, pattern, flags);
    if (rc != 0) {
        // When we call regerror with 0, 0 as last parameters, it returns the error msg size
        // (already considering the '\0')
        size_t error_msg_size = regerror(rc, regex, 0, 0);

        char error_msg[error_msg_size];

        // We call it once again, now with the error_msg buffer to be set
        regerror(rc, regex, error_msg, error_msg_size);

        fprintf(stderr, "error: aqls: lexer: regex: compilation failed. pattern=\"%s\"\n", pattern);
        exit(EXIT_FAILURE);
    }
}

void aqls_lexer_get_line_column(AqlsLexer* lexer, size_t* out_line, size_t* out_column)
{
    aqls_lexer_get_line_column_from_pos(lexer, lexer->pos, out_line, out_column);
}

void aqls_lexer_get_line_column_from_pos(AqlsLexer* lexer, size_t pos, size_t* out_line, size_t* out_column)
{
    *out_line = *out_column = 1;
    for (size_t i = 0; i < pos && i < lexer->input_len; i++) {
        if (lexer->input[i] == '\n') {
            (*out_line)++;
            (*out_column) = 1;
        } else {
            (*out_column)++;
        }
    }
}

void aqls_lexer_get_line_column_from_cursor(AqlsLexer* lexer, const char* cursor, size_t* out_line, size_t* out_column)
{
    *out_line = *out_column = 1;
    for (const char* c = lexer->input; c < cursor; c++) {
        if (*c == '\n') {
            (*out_line)++;
            (*out_column) = 1;
        } else {
            (*out_column)++;
        }
    }
}
