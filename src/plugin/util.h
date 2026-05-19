#ifndef _bpbxclap_util_h
#define _bpbxclap_util_h

#define membersize(type, member) (sizeof(((type *)0)->member))

#ifdef _MSC_VER
#include <string.h>
#define impl_strcpy_s strcpy_s
#else
#include <string.h>
static inline void impl_strcpy_s(char *restrict dest, size_t destsz, const char *restrict src) {
   strncpy(dest, src, destsz);
   dest[destsz-1] = '\0';
}
#endif

#endif