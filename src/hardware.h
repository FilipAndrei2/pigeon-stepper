#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "sm.h"

// SPI COMMUNICATION
#define DISPLAY_SPI_PORT spi1
typedef enum : int {
    // PENTRU CS
    SPI_START_COM   = 0,
    SPI_END_COM     = 1,
    
    // PENTRU DC
    SPI_CMD_SEND    = 0,
    SPI_BUFFER_SEND = 1,
} SpiCommunication_t;


// Buzzer
void playTone(float clkdiv,
                     uint16_t wrap,
                     uint16_t level,
                     uint32_t duration_ms);


// Display
#define DISPLAY_RESOLUTION_WIDTH    128U
#define DISPLAY_RESOLUTION_HEIGHT   160U
#define RGB_SIZE                      2U
extern uint8_t displayBuffer[DISPLAY_RESOLUTION_WIDTH * DISPLAY_RESOLUTION_HEIGHT * RGB_SIZE];

#ifdef __SM_DEBUG__
    ExitCode_t playFullSound(void);
    ExitCode_t displayTest(void);
    ExitCode_t gyaTest(void);
#endif //__SM_DEBUG__

#endif // __HARDWARE_H__