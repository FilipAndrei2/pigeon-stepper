#include "pigeon_stepper.h"
#include "types.h"
#include "haptic.h"
#include "pinout.h"
#include "utils.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"

void Haptic_Init(void) {
    LOG("Haptic init\n");
    gpio_set_function(HAPTIC_PWM_PIN, GPIO_FUNC_PWM);

    // Aflăm ce "motor" intern PWM (slice) controlează acest pin
    uint slice_num = pwm_gpio_to_slice_num(HAPTIC_PWM_PIN);

    // Setăm "rezoluția" PWM-ului (Wrap value). 
    // Valoarea 65535 ne dă control foarte fin asupra intensității.
    pwm_set_wrap(slice_num, 65535);

    // Pornim generatorul PWM (cu duty cycle 0 implicit)
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(HAPTIC_PWM_PIN), 0);
    pwm_set_enabled(slice_num, true);
}

/// @brief 
/// @param timeMs timpul cat sa vibreze 
/// @param intensity intensitatea cat sa vibreze interval (0, 65535)
/// @return 
ExitCode_t Haptic_Vibrate(uint16_t timeMs, uint16_t intensity) {
    VLOG("Haptic vibrate: intensity: %d\n", intensity);
    uint slice_num = pwm_gpio_to_slice_num(HAPTIC_PWM_PIN);
    uint chan = pwm_gpio_to_channel(HAPTIC_PWM_PIN);

    // PASUL A: Pornim vibrația setând Duty Cycle-ul
    pwm_set_chan_level(slice_num, chan, intensity);

    // PASUL B: Așteptăm timpul dorit (ținem procesorul aici cât timp motorul merge)
    sleep_ms(timeMs);

    // PASUL C: Oprim motorul tăind Duty Cycle-ul înapoi la 0
    pwm_set_chan_level(slice_num, chan, 0);

    return SUCCESS;
}