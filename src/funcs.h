#ifndef __FUNCS_H__
#define __FUNCS_H__

#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "sm.h"

ExitCode_t init(void);
ExitCode_t mainLoop(void);
ExitCode_t displaySendBuffer(const uint8_t* buffer, size_t len);
ExitCode_t displaySendCmd(uint8_t cmd);
ExitCode_t displaySendData(uint8_t data);
ExitCode_t displaySetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
#endif // __FUNCS_H__
