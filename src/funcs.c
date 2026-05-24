#include "hardware/pwm.h"

#include "sm.h"
#include "funcs.h"
#include "hardware.h"
#include "pinout.h"
#include "params.h"

static ExitCode_t initBuzzerPins() {

    // daca apelez gpio_set_function(...), 
    // nu mai am nevoie de apel catre gpio_init(uint gpio)
    gpio_set_function(BUZZER_IO, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(BUZZER_IO);

    pwm_config config = pwm_get_default_config();

    pwm_config_set_clkdiv(&config, BUZZER_PWM_CLK_DIV);
    pwm_config_set_wrap(&config, BUZZER_PWM_WRAP);

    pwm_init(slice, &config, true);

    // 1.5ms pulse
    pwm_set_gpio_level(BUZZER_IO, BUZZER_PWM_LEVEL); // FIXME: muta ma unde tb

    return SUCCESS;
}

static ExitCode_t initDisplayPins() {

}

ExitCode_t initPins() {
    if (initBuzzerPins()) {
        LOG("initBuzzerPins(): Nu s-a putut initializa buzzerul\n");
    }

    if (initDisplayPins()) {
        LOG("initDisplayPins(): Nu s-a putut initializa displayul\n")
    }

    gpio_set_dir(BUZZER_IO, GPIO_OUT);
    return SUCCESS;
}

// @returns FALSE for succes, TRUE on error
ExitCode_t init(void) {
    stdio_init_all(); // s-ar putea sa vrem sa verificam si codul de eroare

    if (cyw43_arch_init()) { // initializeaza  wireless
        return FAIL;
    }

    if (initPins()) {
        return FAIL;
    }

    return SUCCESS;
}

ExitCode_t mainLoop(void) {
    while (TRUE) {
        if (playSound(150000000)) {
            fprintf(stderr, "Nu s-a putut canta\n");
        }
    }

    return SUCCESS;
}