#include "pigeon_stepper.h"
#include "types.h"
#include "haptic.h"
#include "pinout.h"
#include "utils.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"

void Haptic_Init(void) {
    LOG("Haptic init\n");
#ifdef _HAPTIC_NO_PWM_
    gpio_init(HAPTIC_PWM_PIN);
    gpio_set_dir(HAPTIC_PWM_PIN, GPIO_OUT);

    gpio_put(HAPTIC_PWM_PIN, 0); // OFF default
#else
    gpio_set_function(HAPTIC_PWM_PIN, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(HAPTIC_PWM_PIN);

    pwm_set_wrap(slice, 255);   // rezoluTie 8-bit
    pwm_set_chan_level(slice, PWM_CHAN_A, 0);

    pwm_set_enabled(slice, true);
#endif
}

/// @brief 
/// @param timeMs timpul cat sa vibreze 
/// @param intensity intensitatea cat sa vibreze interval (0, 255]
/// @return 
ExitCode_t Haptic_Vibrate(uint16_t timeMs, uint8_t intensity) {
#ifdef _HAPTIC_NO_PWM_
    (void)intensity; // nu se foloseste

    LOG("Haptic vibrate NO PWM\n");

    gpio_put(HAPTIC_PWM_PIN, 1);  // ON
    sleep_ms(timeMs);
    gpio_put(HAPTIC_PWM_PIN, 0);  // OFF
#else
    pwm_set_gpio_level(HAPTIC_PWM_PIN, intensity);
        sleep_ms(timeMs);
    pwm_set_gpio_level(HAPTIC_PWM_PIN, 0);
    LOG("Haptic viBrate pwm");
#endif
    return SUCCESS;
}