#include "ast.bak.h"

#include <stdlib.h>

#include "utils.h"

struct aqls_ast_node* aqls_ast_new_compilation_unit(struct aqls_ast_node* statement, struct aqls_ast_node* end_statement)
{
    struct aqls_ast_node_compilation_unit* compilation_unit = ALLOC_OR_PANIC(struct aqls_ast_node_compilation_unit);
    compilation_unit->statement = statement;
    compilation_unit->end_statement = end_statement;

    struct aqls_ast_node* node = ALLOC_OR_PANIC(aqls_ast_node);
    node->kind = AQLS_AST_NODE_COMPILATION_UNIT;
    node->as.compilation_unit = compilation_unit;

    return node;
}

struct aqls_ast_node* aqls_ast_new_statement_write(AqlsToken write_token, AqlsToken operand_token) 
{
    struct aqls_ast_node_stmt_write* write_stmt = ALLOC_OR_PANIC(struct aqls_ast_node_stmt_write);
    write_stmt->write_token = write_token;
    write_stmt->operand_token = operand_token;

    struct aqls_ast_node* node = ALLOC_OR_PANIC(aqls_ast_node);
    node->kind = AQLS_AST_NODE_STATEMENT_WRITE;
    node->as.write_stmt = write_stmt;

    return node;
}

struct aqls_ast_node* aqls_ast_new_statenet_end(AqlsToken end_token)
{
    struct aqls_ast_node_statement_end* end_statement = ALLOC_OR_PANIC(struct aqls_ast_node_statement_end);
    end_statement->end_token = end_token;

    struct aqls_ast_node* node = ALLOC_OR_PANIC(aqls_ast_node);
    node->kind = AQLS_AST_NODE_STATEMENT_END;
    node->as.end_statement = end_statement;
    
    return node;
}
