#ifndef __SM_H__
#define __SM_H__

#include <stddef.h>
#include "pico/stdlib.h"

#include <stdint.h>

typedef enum : uint8_t {
    FALSE = 0,
    TRUE
} Bool_t;

typedef enum : uint8_t {
    SUCCESS = 0,
    FAIL
} ExitCode_t;  

#ifdef __SM_DEBUG__
    #define LOG(MSG)      fprintf(stdout, MSG)
    #define VLOG(MSG, ...) fprintf(stdout, MSG, __VA_ARGS__)
#else
    #define LOG(MSG)      do { } while (0);
    #define VLOG(MSG, ...) do { } while (0);
#endif

#endif // __SM_H__