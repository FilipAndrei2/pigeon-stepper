#ifndef _PS_TYPES_H_
#define _PS_TYPES_H_

#include <stdint.h>
#include <stddef.h>

typedef enum : uint8_t {
    FALSE = 0,
    TRUE
} Bool_t;

typedef enum : uint8_t {
    SUCCESS = 0,
    FAIL
} ExitCode_t;  

#endif // _PS_TYPES_H_