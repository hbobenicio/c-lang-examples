#ifndef AQLS_PARSER_AST_H
#define AQLS_PARSER_AST_H

#include <stddef.h>

#include "token.h"

// Another way around maybe
// AqlsAstCompilationUnit
//    AqlsAstStatements
//        AqlsAstStatement (tagged union)
//    AqlsEndStatement

typedef struct AqlsAstNode AqlsAstNode;
typedef struct AqlsAstNodeCompilationUnit AqlsAstNodeCompilationUnit;
typedef struct AqlsAstNodeStatementWrite AqlsAstNodeStatementWrite;
typedef struct AqlsAstNodeStatementEnd AqlsAstNodeStatementEnd;

typedef enum {
    AQLS_AST_NODE_COMPILATION_UNIT,
    AQLS_AST_NODE_STATEMENT_WRITE,
    AQLS_AST_NODE_STATEMENT_END,
} AqlsAstNodeKind;

struct AqlsAstNode {
    AqlsAstNodeKind kind;
    union {
        AqlsAstNodeCompilationUnit* compilation_unit;
        AqlsAstNodeStatementWrite* write_statement;
        AqlsAstNodeStatementEnd* end_statement;
    } as;
};

struct AqlsAstNodeCompilationUnit {
    AqlsAstNode* statement;
    AqlsAstNode* end_statement;
};

struct AqlsAstNodeStatementWrite {
    AqlsToken write_token;
    AqlsToken operand_token;
};

struct AqlsAstNodeStatementEnd {
    AqlsToken end_token;
};

AqlsAstNode* aqls_ast_new_compilation_unit(AqlsAstNode* statement, AqlsAstNode* end_statement);
AqlsAstNode* aqls_ast_new_statement_write(AqlsToken write_token, AqlsToken operand_token);
AqlsAstNode* aqls_ast_new_statenet_end(AqlsToken end_token);

#endif
