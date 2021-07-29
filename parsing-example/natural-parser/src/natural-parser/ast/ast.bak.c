#include "ast.h"

#include <stdlib.h>

#include "utils.h"

NaturalAstNode* natural_ast_new_compilation_unit(NaturalAstNode* statement, NaturalAstNode* end_statement)
{
    NaturalAstNodeCompilationUnit* compilation_unit = ALLOC_OR_PANIC(NaturalAstNodeCompilationUnit);
    compilation_unit->statement = statement;
    compilation_unit->end_statement = end_statement;

    NaturalAstNode* node = ALLOC_OR_PANIC(NaturalAstNode);
    node->kind = NATURAL_AST_NODE_COMPILATION_UNIT;
    node->as.compilation_unit = compilation_unit;

    return node;
}

NaturalAstNode* natural_ast_new_statement_write(NaturalToken write_token, NaturalToken operand_token)
{
    NaturalAstNodeStatementWrite* write_statement = ALLOC_OR_PANIC(NaturalAstNodeStatementWrite);
    write_statement->write_token = write_token;
    write_statement->operand_token = operand_token;

    NaturalAstNode* node = ALLOC_OR_PANIC(NaturalAstNode);
    node->kind = NATURAL_AST_NODE_STATEMENT_WRITE;
    node->as.write_statement = write_statement;

    return node;
}

NaturalAstNode* natural_ast_new_statenet_end(NaturalToken end_token)
{
    NaturalAstNodeStatementEnd* end_statement = ALLOC_OR_PANIC(NaturalAstNodeStatementEnd);
    end_statement->end_token = end_token;

    NaturalAstNode* node = ALLOC_OR_PANIC(NaturalAstNode);
    node->kind = NATURAL_AST_NODE_STATEMENT_END;
    node->as.end_statement = end_statement;
    
    return node;
}
