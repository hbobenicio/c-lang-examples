#include "ast.h"

#include <stdlib.h>

#include <aqls-parser/utils.h>

AqlsAst* aqls_ast_new(struct aqls_ast_compilation_unit* compilation_unit)
{
    AqlsAst* ast = ALLOC_OR_PANIC(AqlsAst);
    ast->compilation_unit = compilation_unit;

    return ast;
}

struct aqls_ast_compilation_unit* aqls_ast_compilation_unit_new(struct aqls_ast_stmt_list* statements, struct aqls_ast_end* end)
{
    struct aqls_ast_compilation_unit* compilation_unit = ALLOC_OR_PANIC(struct aqls_ast_compilation_unit);
    compilation_unit->statements = statements;
    compilation_unit->end = end;

    return compilation_unit;
}

struct aqls_ast_stmt_list* aqls_ast_statements_new(struct aqls_ast_stmt* statement)
{
    struct aqls_ast_stmt_list* list = ALLOC_OR_PANIC(struct aqls_ast_stmt_list);
    list->statement = statement;
    list->next = NULL;

    return list;
}

struct aqls_ast_stmt_list* aqls_ast_statements_append(struct aqls_ast_stmt_list* list, struct aqls_ast_stmt* statement)
{
    struct aqls_ast_stmt_list* new_node = aqls_ast_statements_new(statement);
    if (list != NULL) {
        list->next = new_node;
    }
    return new_node;
}

struct aqls_ast_stmt* aqls_ast_statement_write_new(AqlsToken write_token, AqlsToken operand)
{
    struct aqls_ast_stmt_write* write_stmt = ALLOC_OR_PANIC(struct aqls_ast_stmt_write);
    write_stmt->write_token = write_token;
    write_stmt->operand = operand;
    
    struct aqls_ast_stmt* statement = ALLOC_OR_PANIC(struct aqls_ast_stmt);
    statement->kind = AQLS_AST_STATEMENT_WRITE;
    statement->as.write_stmt = write_stmt;

    return statement;
}

struct aqls_ast_end* aqls_ast_end_new(AqlsToken end_token)
{
    struct aqls_ast_end* end = ALLOC_OR_PANIC(struct aqls_ast_end);
    end->end_token = end_token;

    return end;
}
