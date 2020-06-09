#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// POSIX
#include <arpa/inet.h>

static uint8_t short_get_byte(uint16_t number, unsigned short byte_index) {
    const uint8_t* addr = (uint8_t*) &number;

    assert(byte_index < sizeof(uint16_t));

    return addr[byte_index];
}

int main() {
    uint16_t msg_size_little_endian = 255;
    uint16_t msg_size_big_endian = htons(msg_size_little_endian);

    printf("msg_size(little): 0x%02x 0x%02x\n", short_get_byte(msg_size_little_endian, 0), short_get_byte(msg_size_little_endian, 1));
    printf("msg_size(big): 0x%02x 0x%02x\n", short_get_byte(msg_size_big_endian, 0), short_get_byte(msg_size_big_endian, 1));
}
