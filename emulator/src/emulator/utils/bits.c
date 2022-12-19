#include "bits.h"

#include <assert.h>

bool bit_is_set(uint8_t num, uint8_t bit_order)
{
    assert(bit_order < 8);

    if (num & (1 << bit_order)) {
        return true;
    }
    
    return false;
}
