#define __SM_DEBUG__

#include "hardware/pwm.h"

#include "sm.h"
#include "funcs.h"
#include "hardware.h"
#include "pinout.h"
#include "params.h"

void playTone(float clkdiv,
                     uint16_t wrap,
                     uint16_t level,
                     uint32_t duration_ms)
{
    uint slice = pwm_gpio_to_slice_num(BUZZER_IO);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clkdiv);
    pwm_config_set_wrap(&config, wrap);

    pwm_init(slice, &config, true);

    pwm_set_gpio_level(BUZZER_IO, level);

    pwm_set_enabled(slice, true);

    sleep_ms(duration_ms);

    pwm_set_gpio_level(BUZZER_IO, 0);
    
    sleep_ms(20);
}

#ifdef __SM_DEBUG__

// Functii de test pentru componente

ExitCode_t playFullSound() {
    playTone(BUZZER_PWM_CLK_DIV_C5, BUZZER_PWM_WRAP_C5, BUZZER_PWM_LEVEL_C5, 120); 
    playTone(BUZZER_PWM_CLK_DIV_E5, BUZZER_PWM_WRAP_E5, BUZZER_PWM_LEVEL_E5, 120); 
    playTone(BUZZER_PWM_CLK_DIV_G5, BUZZER_PWM_WRAP_G5, BUZZER_PWM_LEVEL_G5, 180); 
    sleep_ms(20);
    return SUCCESS;
}

uint8_t displayBuffer[DISPLAY_RESOLUTION_WIDTH * DISPLAY_RESOLUTION_HEIGHT * RGB_SIZE];

ExitCode_t displayTest() {
    for (int i = 0; i < DISPLAY_RESOLUTION_WIDTH * DISPLAY_RESOLUTION_HEIGHT; i++) {
        ((uint16_t*)displayBuffer)[i] = 0xF800; // Roșu
    }
    
    // SETEAZĂ ZONA DE DESENARE ÎNAINTE DE A TRIMITE BUFFERUL
    displaySetWindow(0, 0, DISPLAY_RESOLUTION_WIDTH - 1, DISPLAY_RESOLUTION_HEIGHT - 1);

    if (FAIL == displaySendBuffer(displayBuffer, sizeof(displayBuffer))) {
        LOG("Nu s-a putut desena");
        return FAIL;
    }

    return SUCCESS;
}

#endif // __SM_DEBUG__
