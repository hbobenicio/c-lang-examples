#pragma once

#define UNIMPLEMENTED() \
    log_fatalf("TODO: unimplemented function %s at:\n>>> %s:%d", __func__, __FILE__, __LINE__)
