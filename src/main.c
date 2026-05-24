#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "hardware.h"
#include "sm.h"
#include "funcs.h"


int main() {
    fprintf(stdout, "Pornire program\n");

    if (init()) {
        fprintf(stderr, "main.c init(): Eroare de initializare\n");
        return 67;
    }

    if (mainLoop()) {
        playFullSound();
        fprintf(stderr, "main.c mainLoop(): Ceva rau s-a intamplat\n");
        return 67;
    }

}