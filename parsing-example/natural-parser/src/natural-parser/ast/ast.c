#include "ast.h"

#include <stdlib.h>

#include <natural-parser/utils.h>

NaturalAst* natural_ast_new(NaturalAstCompilationUnit* compilation_unit)
{
    NaturalAst* ast = ALLOC_OR_PANIC(NaturalAst);
    ast->compilation_unit = compilation_unit;

    return ast;
}

NaturalAstCompilationUnit* natural_ast_compilation_unit_new(NaturalAstStatementList* statements, NaturalAstEnd* end)
{
    NaturalAstCompilationUnit* compilation_unit = ALLOC_OR_PANIC(NaturalAstCompilationUnit);
    compilation_unit->statements = statements;
    compilation_unit->end = end;

    return compilation_unit;
}

NaturalAstStatementList* natural_ast_statements_new(NaturalAstStatement* statement)
{
    NaturalAstStatementList* list = ALLOC_OR_PANIC(NaturalAstStatementList);
    list->statement = statement;
    list->next = NULL;

    return list;
}

NaturalAstStatementList* natural_ast_statements_append(NaturalAstStatementList* list, NaturalAstStatement* statement)
{
    NaturalAstStatementList* new_node = natural_ast_statements_new(statement);
    if (list != NULL) {
        list->next = new_node;
    }
    return new_node;
}

NaturalAstStatement* natural_ast_statement_write_new(NaturalToken write_token, NaturalToken operand)
{
    NaturalAstStatementWrite* write_statement = ALLOC_OR_PANIC(NaturalAstStatementWrite);
    write_statement->write_token = write_token;
    write_statement->operand = operand;
    
    NaturalAstStatement* statement = ALLOC_OR_PANIC(NaturalAstStatement);
    statement->kind = NATURAL_AST_STATEMENT_WRITE;
    statement->as.write_statement = write_statement;

    return statement;
}

NaturalAstEnd* natural_ast_end_new(NaturalToken end_token)
{
    NaturalAstEnd* end = ALLOC_OR_PANIC(NaturalAstEnd);
    end->end_token = end_token;

    return end;
}
