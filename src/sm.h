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
    #define LOG(MSG) fprintf(MSG)
#else
    #define LOG(MSG) do { } while (0);
#endif

#endif // __SM_H__