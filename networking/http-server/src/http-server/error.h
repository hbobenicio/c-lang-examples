#pragma once

/**
 * A general opaque result type for a simple yet efficient error reporting/handling
 */
enum result {
    RESULT_OK = 0,
    RESULT_ERR = 1,
};

#define IF_ERROR_RETURN(EXPR) if ((EXPR) != RESULT_OK) return RESULT_ERROR

