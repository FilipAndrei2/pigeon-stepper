#include "sm.h"
#include "funcs.h"
#include "hardware.h"
#include "pinout.h"

ExitCode initPins() {
    gpio_init(BUZZER_IO);

    gpio_set_dir(BUZZER_IO, GPIO_OUT);
    return SUCCESS;
}

// @returns FALSE for succes, TRUE on error
ExitCode init(void) {
    stdio_init_all(); // s-ar putea sa vrem sa verificam si codul de eroare

    if (cyw43_arch_init()) { // initializeaza ceva wireless
        return FAIL;
    }

    if (initPins()) {
        return FAIL;
    }

    return SUCCESS;
}

ExitCode mainLoop(void) {
    while (TRUE) {
        if (playSound(150000000)) {
            fprintf(stderr, "Nu s-a putut canta\n");
        }
    }

    return SUCCESS;
}