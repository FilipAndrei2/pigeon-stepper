#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#define __SM_DEBUG__
#include "hardware.h"
#include "sm.h"
#include "funcs.h"

int main() {

    if (init()) {
        LOG("main.c init(): Eroare de initializare\n");
        return EXIT_FAILURE;
    }

    if (mainLoop()) {
        LOG("main.c mainLoop(): Ceva rau s-a intamplat\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;

}
