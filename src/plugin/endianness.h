#ifndef _endianness_h_
#define _endianness_h_

#include <stdint.h>

#define BIG_ENDIAN      0
#define LITTLE_ENDIAN   1

static uint16_t endianness_test_value = 1;

inline uint8_t endianness() {
    return ((uint8_t*)&endianness_test_value)[0];
}

#endif