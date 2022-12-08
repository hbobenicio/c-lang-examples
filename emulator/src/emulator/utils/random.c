#include "random.h"

#include <stdlib.h>
#include <limits.h>

uint8_t random_u8(void)
{
    int x = rand();
    return x % (UINT8_MAX + 1);
}
