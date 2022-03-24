#ifndef AQLS_PARSER_AST2_H
#define AQLS_PARSER_AST2_H

#include <stddef.h>

#include <aqls-parser/token/token.h>

// Another way around maybe
// struct aqls_ast_compilation_unit
//    struct aqls_ast_stmts
//        struct aqls_ast_stmt (tagged union)
//    AqlsEndStatement

typedef struct AqlsAst AqlsAst;
struct aqls_ast_compilation_unit;
struct aqls_ast_stmt_list;
struct aqls_ast_stmt;
struct aqls_ast_stmt_write;
struct aqls_ast_end;

struct AqlsAst {
    struct aqls_ast_compilation_unit* compilation_unit;
};

struct aqls_ast_compilation_unit {
    struct aqls_ast_stmt_list* statements;
    struct aqls_ast_end* end;
};

struct aqls_ast_stmt_list {
    struct aqls_ast_stmt* statement;
    struct aqls_ast_stmt_list* next;
};

enum aqls_ast_stmt_kind {
    AQLS_AST_STATEMENT_WRITE
};

struct aqls_ast_stmt {
    enum aqls_ast_stmt_kind kind;
    union {
        struct aqls_ast_stmt_write* write_stmt;
    } as;
};

struct aqls_ast_stmt_write {
    AqlsToken write_token;
    AqlsToken operand;
};

struct aqls_ast_end {
    AqlsToken end_token;
};

AqlsAst* aqls_ast_new(struct aqls_ast_compilation_unit* compilation_unit);
struct aqls_ast_compilation_unit* aqls_ast_compilation_unit_new(struct aqls_ast_stmt_list* statements, struct aqls_ast_end* end);
struct aqls_ast_stmt_list* aqls_ast_statements_new(struct aqls_ast_stmt* statement);
struct aqls_ast_stmt_list* aqls_ast_statements_append(struct aqls_ast_stmt_list* list, struct aqls_ast_stmt* statement);
struct aqls_ast_stmt* aqls_ast_statement_write_new(AqlsToken write_token, AqlsToken operand);
struct aqls_ast_end* aqls_ast_end_new(AqlsToken end_token);

#endif
