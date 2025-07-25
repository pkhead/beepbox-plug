#ifndef _bpbxclap_system_h_
#define _bpbxclap_system_h_

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

// Apparently denormals aren't a problem on ARM & M1?
// https://en.wikipedia.org/wiki/Subnormal_number
// https://www.kvraudio.com/forum/viewtopic.php?t=575799
#if __arm64__
#define fp_env int
static inline fp_env disable_denormals() {};
static inline void enable_denormals(const fp_env *env) {};
#else
#if !defined(_WIN32)
#include <fenv.h>
#endif

#ifdef FE_DFL_DISABLE_SSE_DENORMS_ENV
#define fp_env fenv_t

static inline fp_env disable_denormals() {
   fenv_t env;
   fegetenv(&env);
   fesetenv(FE_DFL_DISABLE_SSE_DENORMS_ENV);
   return env;
}

static inline void enable_denormals(const fp_env env) {
   fesetenv(&env);
}
#else
#include <immintrin.h>
typedef uint32_t fp_env;

static inline fp_env disable_denormals() {
   const uint32_t mask = _MM_DENORMALS_ZERO_MASK | _MM_FLUSH_ZERO_MASK;
   const uint32_t state = _mm_getcsr();
   _mm_setcsr(state & ~mask);
   return state & mask;
}

static inline void enable_denormals(const fp_env env) {
   const uint32_t mask = _MM_DENORMALS_ZERO_MASK | _MM_FLUSH_ZERO_MASK;
   _mm_setcsr((_mm_getcsr() & ~mask) | env);
}
#endif
#endif