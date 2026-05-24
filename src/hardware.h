#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "sm.h"

// SPI COMMUNICATION
typedef enum : int{
    // PENTRU CS
    SPI_START_COM   = 0,
    SPI_END_COM     = 1,
    
    // PENTRU DC
    SPI_CMD_SEND    = 0,
    SPI_BUFFER_SEND = 1,
} SpiCommunication_t;


// Metodele clasei Buzzer 
void playTone(float clkdiv,
                     uint16_t wrap,
                     uint16_t level,
                     uint32_t duration_ms);

// TODO: Display Pixels Buffer si functii specifice

#define DISPLAY_SPI_PORT spi1

#ifdef __SM_DEBUG__
    ExitCode_t playFullSound(void);
    ExitCode_t displayTest(void);
#endif //__SM_DEBUG__

#endif // __HARDWARE_H__