#pragma once

#include <stdint.h>
#include <stdbool.h>

bool bit_is_set(uint8_t num, uint8_t bit_order);
uint8_t bit_most_significant(uint8_t num);
uint8_t bit_least_significant(uint8_t num);
