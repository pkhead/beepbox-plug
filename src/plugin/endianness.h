#ifndef _endianness_h_
#define _endianness_h_

#include <stdint.h>

// some linux system library crap
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif

#ifdef LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif

#define BIG_ENDIAN      0
#define LITTLE_ENDIAN   1

static inline uint8_t endianness() {
    uint16_t endianness_test_value = 1;
    return ((uint8_t*)&endianness_test_value)[0];
}

#endif