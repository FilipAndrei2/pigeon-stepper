#include "sm.h"
#include "hardware.h"
#include "pinout.h"

ExitCode playSound(size_t duration) {
    while (duration) {
        gpio_put(BUZZER_IO, 1);
        duration--;
    }

    gpio_put(BUZZER_IO, 0); 

}