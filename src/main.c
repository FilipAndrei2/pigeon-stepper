#include "pigeon_stepper.h"

#include <stdio.h>
#include <stdlib.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#define __SM_DEBUG__
#include "main_funcs.h"
#include "types.h"
#include "utils.h"

int main() {

    if (init()) {
        LOG("main.c init(): Eroare de initializare\n");
        return EXIT_FAILURE;
    }
    
    multicore_launch_core1(displayLoop);

    if (mainLoop()) {
        LOG("main.c mainLoop(): Ceva rau s-a intamplat\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;

}
