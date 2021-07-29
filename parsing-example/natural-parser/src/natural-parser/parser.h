#ifndef NATURAL_PARSER_H
#define NATURAL_PARSER_H

#include "lexer.h"
#include "ast/ast.h"

typedef struct {
    NaturalLexer lexer;
    NaturalAst* ast;
} NaturalParser;

void natural_parser_init(NaturalParser* parser);
void natural_parser_free(NaturalParser* parser);

int natural_parser_parse(NaturalParser* parser, const char* input, size_t input_len);

#endif
