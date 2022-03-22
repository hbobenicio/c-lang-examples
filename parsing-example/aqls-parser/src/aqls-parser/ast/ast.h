#ifndef AQLS_PARSER_AST2_H
#define AQLS_PARSER_AST2_H

#include <stddef.h>

#include <aqls-parser/token/token.h>

// Another way around maybe
// AqlsAstCompilationUnit
//    AqlsAstStatements
//        AqlsAstStatement (tagged union)
//    AqlsEndStatement

typedef struct AqlsAst AqlsAst;
typedef struct AqlsAstCompilationUnit AqlsAstCompilationUnit;
typedef struct AqlsAstStatementList AqlsAstStatementList;
typedef struct AqlsAstStatement AqlsAstStatement;
typedef struct AqlsAstStatementWrite AqlsAstStatementWrite;
typedef struct AqlsAstEnd AqlsAstEnd;

struct AqlsAst {
    AqlsAstCompilationUnit* compilation_unit;
};

struct AqlsAstCompilationUnit {
    AqlsAstStatementList* statements;
    AqlsAstEnd* end;
};

struct AqlsAstStatementList {
    AqlsAstStatement* statement;
    AqlsAstStatementList* next;
};

typedef enum {
    AQLS_AST_STATEMENT_WRITE
} AqlsAstStatementKind;

struct AqlsAstStatement {
    AqlsAstStatementKind kind;
    union {
        AqlsAstStatementWrite* write_statement;
    } as;
};

struct AqlsAstStatementWrite {
    AqlsToken write_token;
    AqlsToken operand;
};

struct AqlsAstEnd {
    AqlsToken end_token;
};

AqlsAst* aqls_ast_new(AqlsAstCompilationUnit* compilation_unit);
AqlsAstCompilationUnit* aqls_ast_compilation_unit_new(AqlsAstStatementList* statements, AqlsAstEnd* end);
AqlsAstStatementList* aqls_ast_statements_new(AqlsAstStatement* statement);
AqlsAstStatementList* aqls_ast_statements_append(AqlsAstStatementList* list, AqlsAstStatement* statement);
AqlsAstStatement* aqls_ast_statement_write_new(AqlsToken write_token, AqlsToken operand);
AqlsAstEnd* aqls_ast_end_new(AqlsToken end_token);

#endif
