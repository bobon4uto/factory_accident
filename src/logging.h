#ifdef    MONO_BUILD
#define LOGGING_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _LOGGING_H_
#define    _LOGGING_H_
// logging interface

#include "stdio.h"
#define logi(...)
//#define logi(...) do { printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while (0)

#ifdef      LOGGING_IMPLEMENTATION
// logging implementation



#endif   // LOGGING_IMPLEMENTATION
#endif   //_LOGGING_H_

