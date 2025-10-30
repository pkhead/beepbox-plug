// atomic bool compatibility Crap.
#ifndef _atomic_bool_h_
#define _atomic_bool_h_

#include <stdbool.h>

#if (__STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__))
#include <stdatomic.h>
#else
// defined(_MSC_VER) TODO: use msvc atomics?
typedef volatile bool atomic_bool;

#define atomic_store(a, b) *(a) = (b)

inline static bool atomic_exchange(atomic_bool *a, bool b) {
    bool old = *a;
    *a = b;
    return old;
}
#endif

#endif