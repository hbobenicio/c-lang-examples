#ifndef AQLS_PARSER_PARSER_H
#define AQLS_PARSER_PARSER_H

#include <aqls-parser/lexer.h>
#include <aqls-parser/ast/ast.h>

struct aqls_parser {
    struct aqls_lexer lexer;
    struct aqls_ast* ast;
};

void aqls_parser_init(struct aqls_parser* parser);
void aqls_parser_free(struct aqls_parser* parser);

int aqls_parser_parse(struct aqls_parser* parser, const char* input, size_t input_len);

#endif
