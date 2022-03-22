#include "ast.h"

#include <stdlib.h>

#include <aqls-parser/utils.h>

AqlsAst* aqls_ast_new(AqlsAstCompilationUnit* compilation_unit)
{
    AqlsAst* ast = ALLOC_OR_PANIC(AqlsAst);
    ast->compilation_unit = compilation_unit;

    return ast;
}

AqlsAstCompilationUnit* aqls_ast_compilation_unit_new(AqlsAstStatementList* statements, AqlsAstEnd* end)
{
    AqlsAstCompilationUnit* compilation_unit = ALLOC_OR_PANIC(AqlsAstCompilationUnit);
    compilation_unit->statements = statements;
    compilation_unit->end = end;

    return compilation_unit;
}

AqlsAstStatementList* aqls_ast_statements_new(AqlsAstStatement* statement)
{
    AqlsAstStatementList* list = ALLOC_OR_PANIC(AqlsAstStatementList);
    list->statement = statement;
    list->next = NULL;

    return list;
}

AqlsAstStatementList* aqls_ast_statements_append(AqlsAstStatementList* list, AqlsAstStatement* statement)
{
    AqlsAstStatementList* new_node = aqls_ast_statements_new(statement);
    if (list != NULL) {
        list->next = new_node;
    }
    return new_node;
}

AqlsAstStatement* aqls_ast_statement_write_new(AqlsToken write_token, AqlsToken operand)
{
    AqlsAstStatementWrite* write_statement = ALLOC_OR_PANIC(AqlsAstStatementWrite);
    write_statement->write_token = write_token;
    write_statement->operand = operand;
    
    AqlsAstStatement* statement = ALLOC_OR_PANIC(AqlsAstStatement);
    statement->kind = AQLS_AST_STATEMENT_WRITE;
    statement->as.write_statement = write_statement;

    return statement;
}

AqlsAstEnd* aqls_ast_end_new(AqlsToken end_token)
{
    AqlsAstEnd* end = ALLOC_OR_PANIC(AqlsAstEnd);
    end->end_token = end_token;

    return end;
}
