#ifndef _PS_DISPLAY_H_
#define _PS_DISPLAY_H_

#include "pigeon_stepper.h"
#include "pinout.h"
#include "types.h"
#include "pico/stdlib.h"

ExitCode_t Display_SendBuffer(const uint8_t* buffer, size_t len);
ExitCode_t Display_SendCmd(uint8_t cmd);
ExitCode_t Display_SendData(uint8_t data);
ExitCode_t Display_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);


static inline void display_begin() {
    gpio_put(DISPLAY_CS, 0);
}

static inline void display_end() {
    gpio_put(DISPLAY_CS, 1);
}
ExitCode_t Display_WritePixels(const uint8_t* data, size_t len);

#define DISPLAY_RESOLUTION_WIDTH            128U
#define DISPLAY_RESOLUTION_HEIGHT           160U
#define DISPLAY_RGB_SIZE                      2U
extern uint8_t g_displayBuffer[DISPLAY_RESOLUTION_WIDTH * DISPLAY_RESOLUTION_HEIGHT * DISPLAY_RGB_SIZE];

#define DISPLAY_SPI_PORT spi1


#endif // _PS_DISPLAY_H_
