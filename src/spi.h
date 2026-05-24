#ifndef _PS_SPI_H_
#define _PS_SPI_H_

#include "pigeon_stepper.h"

#include "types.h"
#include "pinout.h"

typedef enum : int {
    // PENTRU PINUL CS
    SPI_START_COM   = 0,
    SPI_END_COM     = 1,

    // PENTRU PINUL DC
    SPI_CMD_SEND    = 0,
    SPI_BUFFER_SEND = 1,
} SpiCommunication_t;

ExitCode_t SPI_PortInit();

#endif // _PS_SPI_H_
