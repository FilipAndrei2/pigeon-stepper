#ifndef _PS_UTILS_H_
#define _PS_UTILS_H_

#include <stddef.h>
#include "pico/stdlib.h"

#include <stdint.h>

#ifdef _PS_DEBUG_
    #define LOG(MSG)      fprintf(stdout, MSG)
    #define VLOG(MSG, ...) fprintf(stdout, MSG, __VA_ARGS__)
#else
    #define LOG(MSG)      do { } while (0);
    #define VLOG(MSG, ...) do { } while (0);
#endif

#endif // _PS_UTILS_H_