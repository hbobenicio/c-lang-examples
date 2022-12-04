#include "memory.h"

#include <string.h>

void memory_init(uint8_t* mem)
{
    memset(mem, 0, MEMORY_SIZE);
}
