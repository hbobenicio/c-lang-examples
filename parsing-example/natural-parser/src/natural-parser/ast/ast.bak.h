#ifndef NATURAL_PARSER_AST_H
#define NATURAL_PARSER_AST_H

#include <stddef.h>

#include "token.h"

// Another way around maybe
// NaturalAstCompilationUnit
//    NaturalAstStatements
//        NaturalAstStatement (tagged union)
//    NaturalEndStatement

typedef struct NaturalAstNode NaturalAstNode;
typedef struct NaturalAstNodeCompilationUnit NaturalAstNodeCompilationUnit;
typedef struct NaturalAstNodeStatementWrite NaturalAstNodeStatementWrite;
typedef struct NaturalAstNodeStatementEnd NaturalAstNodeStatementEnd;

typedef enum {
    NATURAL_AST_NODE_COMPILATION_UNIT,
    NATURAL_AST_NODE_STATEMENT_WRITE,
    NATURAL_AST_NODE_STATEMENT_END,
} NaturalAstNodeKind;

struct NaturalAstNode {
    NaturalAstNodeKind kind;
    union {
        NaturalAstNodeCompilationUnit* compilation_unit;
        NaturalAstNodeStatementWrite* write_statement;
        NaturalAstNodeStatementEnd* end_statement;
    } as;
};

struct NaturalAstNodeCompilationUnit {
    NaturalAstNode* statement;
    NaturalAstNode* end_statement;
};

struct NaturalAstNodeStatementWrite {
    NaturalToken write_token;
    NaturalToken operand_token;
};

struct NaturalAstNodeStatementEnd {
    NaturalToken end_token;
};

NaturalAstNode* natural_ast_new_compilation_unit(NaturalAstNode* statement, NaturalAstNode* end_statement);
NaturalAstNode* natural_ast_new_statement_write(NaturalToken write_token, NaturalToken operand_token);
NaturalAstNode* natural_ast_new_statenet_end(NaturalToken end_token);

#endif
