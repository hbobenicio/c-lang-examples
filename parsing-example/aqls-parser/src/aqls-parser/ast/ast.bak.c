#include "ast.h"

#include <stdlib.h>

#include "utils.h"

AqlsAstNode* aqls_ast_new_compilation_unit(AqlsAstNode* statement, AqlsAstNode* end_statement)
{
    AqlsAstNodeCompilationUnit* compilation_unit = ALLOC_OR_PANIC(AqlsAstNodeCompilationUnit);
    compilation_unit->statement = statement;
    compilation_unit->end_statement = end_statement;

    AqlsAstNode* node = ALLOC_OR_PANIC(AqlsAstNode);
    node->kind = AQLS_AST_NODE_COMPILATION_UNIT;
    node->as.compilation_unit = compilation_unit;

    return node;
}

AqlsAstNode* aqls_ast_new_statement_write(AqlsToken write_token, AqlsToken operand_token)
{
    AqlsAstNodeStatementWrite* write_statement = ALLOC_OR_PANIC(AqlsAstNodeStatementWrite);
    write_statement->write_token = write_token;
    write_statement->operand_token = operand_token;

    AqlsAstNode* node = ALLOC_OR_PANIC(AqlsAstNode);
    node->kind = AQLS_AST_NODE_STATEMENT_WRITE;
    node->as.write_statement = write_statement;

    return node;
}

AqlsAstNode* aqls_ast_new_statenet_end(AqlsToken end_token)
{
    AqlsAstNodeStatementEnd* end_statement = ALLOC_OR_PANIC(AqlsAstNodeStatementEnd);
    end_statement->end_token = end_token;

    AqlsAstNode* node = ALLOC_OR_PANIC(AqlsAstNode);
    node->kind = AQLS_AST_NODE_STATEMENT_END;
    node->as.end_statement = end_statement;
    
    return node;
}
