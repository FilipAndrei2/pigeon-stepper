#ifndef _MAIN_FUNCS_H_
#define _MAIN_FUNCS_H_

#include "pigeon_stepper.h"

#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "types.h"

ExitCode_t init(void);
ExitCode_t mainLoop(void);

#endif // _MAIN_FUNCS_H_
