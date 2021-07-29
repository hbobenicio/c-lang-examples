#ifndef NATURAL_PARSER_AST2_H
#define NATURAL_PARSER_AST2_H

#include <stddef.h>

#include <natural-parser/token/token.h>

// Another way around maybe
// NaturalAstCompilationUnit
//    NaturalAstStatements
//        NaturalAstStatement (tagged union)
//    NaturalEndStatement

typedef struct NaturalAst NaturalAst;
typedef struct NaturalAstCompilationUnit NaturalAstCompilationUnit;
typedef struct NaturalAstStatementList NaturalAstStatementList;
typedef struct NaturalAstStatement NaturalAstStatement;
typedef struct NaturalAstStatementWrite NaturalAstStatementWrite;
typedef struct NaturalAstEnd NaturalAstEnd;

struct NaturalAst {
    NaturalAstCompilationUnit* compilation_unit;
};

struct NaturalAstCompilationUnit {
    NaturalAstStatementList* statements;
    NaturalAstEnd* end;
};

struct NaturalAstStatementList {
    NaturalAstStatement* statement;
    NaturalAstStatementList* next;
};

typedef enum {
    NATURAL_AST_STATEMENT_WRITE
} NaturalAstStatementKind;

struct NaturalAstStatement {
    NaturalAstStatementKind kind;
    union {
        NaturalAstStatementWrite* write_statement;
    } as;
};

struct NaturalAstStatementWrite {
    NaturalToken write_token;
    NaturalToken operand;
};

struct NaturalAstEnd {
    NaturalToken end_token;
};

NaturalAst* natural_ast_new(NaturalAstCompilationUnit* compilation_unit);
NaturalAstCompilationUnit* natural_ast_compilation_unit_new(NaturalAstStatementList* statements, NaturalAstEnd* end);
NaturalAstStatementList* natural_ast_statements_new(NaturalAstStatement* statement);
NaturalAstStatementList* natural_ast_statements_append(NaturalAstStatementList* list, NaturalAstStatement* statement);
NaturalAstStatement* natural_ast_statement_write_new(NaturalToken write_token, NaturalToken operand);
NaturalAstEnd* natural_ast_end_new(NaturalToken end_token);

#endif
