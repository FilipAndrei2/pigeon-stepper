#include "button.h"

#include <stdint.h>
#include <stddef.h>

#include "haptic.h"
#include "buzzer.h"
#include "pedometer.h"
#include "pinout.h"
#include "utils.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"
 
extern Pedometer s_pedometer;

void Buttons_Init(void){
    gpio_init(BTN_B1_PIN);
    gpio_init(BTN_B2_PIN);
    gpio_init(BTN_B3_PIN);

    gpio_set_dir(BTN_B1_PIN, GPIO_IN);
    gpio_set_dir(BTN_B2_PIN, GPIO_IN);
    gpio_set_dir(BTN_B3_PIN, GPIO_IN);

    gpio_pull_up(BTN_B1_PIN);
    gpio_pull_up(BTN_B2_PIN);
    gpio_pull_up(BTN_B3_PIN);

    gpio_set_irq_enabled_with_callback(
        BTN_B1_PIN,                 // Pinul de monitorizat
        GPIO_IRQ_EDGE_FALL,         // Evenimentul: declanșează doar când voltajul scade (EDGE_FALL)
        true,                       // true = activează întreruperea
        &Button1_CallBack           // Numele (adresa) funcției de mai sus
    );

    gpio_set_irq_enabled_with_callback(
        BTN_B2_PIN,                 // Pinul de monitorizat
        GPIO_IRQ_EDGE_FALL,         // Evenimentul: declanșează doar când voltajul scade (EDGE_FALL)
        true,                       // true = activează întreruperea
        &Button2_CallBack           // Numele (adresa) funcției de mai sus
    );

    gpio_set_irq_enabled_with_callback(
        BTN_B3_PIN,             // Pinul de monitorizat
        GPIO_IRQ_EDGE_FALL,     // Evenimentul: declanșează doar când voltajul scade (EDGE_FALL)
        true,                   // true = activează întreruperea
        &Button3_CallBack          // Numele (adresa) funcției de mai sus
    );
}

void Button1_CallBack(uint gpio, uint32_t events) {
    LOG("Button1 pressed\n");
    Buzzer_PlayDoubleBeep();
    sleep_ms(100);
}

void Button2_CallBack(uint gpio, uint32_t events) {
    LOG("Button2 pressed\n");

    if (Pedometer_ShouldLevelUp(&s_pedometer)) {
        Buzzer_PlayMarioLevelUp();
        Pedometer_LevelUp(&s_pedometer);
    } else {
        Buzzer_PlayBadSound();
        Haptic_Vibrate(1000, 30000);
    }
    sleep_ms(100);
}

void Button3_CallBack(uint gpio, uint32_t events) {
    LOG("Button3 pressed\n");
    Haptic_Vibrate(1000, 30000);
    sleep_ms(100);
}
