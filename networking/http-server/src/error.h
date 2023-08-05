#pragma once

/**
 * A general opaque error code enumeration for minimal error handling
 */
enum return_code {
    RC_OK = 0,
    RC_ERROR = 1,
};

#define IF_ERROR_RETURN(EXPR) if ((EXPR) != RC_OK) return RC_ERROR
