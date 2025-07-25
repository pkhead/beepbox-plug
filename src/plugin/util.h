#ifndef _bpbxclap_util_h
#define _bpbxclap_util_h

#ifdef _MSC_VER
#define impl_strcpy_s strcpy_s
#else
static inline void impl_strcpy_s(char *restrict dest, size_t destsz, const char *restrict src) {
   strncpy(dest, src, destsz);
   dest[destsz-1] = '\0';
}
#endif

#endif