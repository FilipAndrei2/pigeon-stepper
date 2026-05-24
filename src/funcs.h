#ifndef __FUNCS_H__
#define __FUNCS_H__

#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"


#include "sm.h"

ExitCode_t init(void);
ExitCode_t mainLoop(void);

#endif // __FUNCS_H__