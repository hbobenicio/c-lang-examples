#ifndef AQLS_PARSER_AST_H
#define AQLS_PARSER_AST_H

#include <stddef.h>

#include "token.h"

// Another way around maybe
// struct aqls_ast_compilation_unit
//    struct aqls_ast_stmts
//        struct aqls_ast_stmt (tagged union)
//    AqlsEndStatement

struct aqls_ast_node aqls_ast_node;
struct aqls_ast_node_compilation_unit aqls_ast_node_compilation_unit;
struct aqls_ast_node_stmt_write aqls_ast_node_stmt_write;
struct aqls_ast_node_statement_end aqls_ast_node_statement_end;

enum aqls_ast_node_kind {
    AQLS_AST_NODE_COMPILATION_UNIT,
    AQLS_AST_NODE_STATEMENT_WRITE,
    AQLS_AST_NODE_STATEMENT_END,
};

struct aqls_ast_node {
    enum aqls_ast_node_kind kind;
    union {
        struct aqls_ast_node_compilation_unit* compilation_unit;
        struct aqls_ast_node_stmt_write* write_stmt;
        struct aqls_ast_node_statement_end* end_statement;
    } as;
};

struct aqls_ast_node_compilation_unit {
    struct aqls_ast_node* statement;
    struct aqls_ast_node* end_statement;
};

struct aqls_ast_node_stmt_write {
    struct aqls_token write_token;
    struct aqls_token operand_token;
};

struct aqls_ast_node_statement_end {
    struct aqls_token end_token;
};

struct aqls_ast_node* aqls_ast_new_compilation_unit(struct aqls_ast_node* statement, struct aqls_ast_node* end_statement);
struct aqls_ast_node* aqls_ast_new_statement_write(struct aqls_token write_token, struct aqls_token operand_token);
struct aqls_ast_node* aqls_ast_new_statenet_end(struct aqls_token end_token);

#endif
