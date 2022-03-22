#ifndef AQLS_PARSER_PARSER_H
#define AQLS_PARSER_PARSER_H

#include <aqls-parser/lexer.h>
#include <aqls-parser/ast/ast.h>

typedef struct {
    AqlsLexer lexer;
    AqlsAst* ast;
} AqlsParser;

void aqls_parser_init(AqlsParser* parser);
void aqls_parser_free(AqlsParser* parser);

int aqls_parser_parse(AqlsParser* parser, const char* input, size_t input_len);

#endif
