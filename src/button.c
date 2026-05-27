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
    BTN_B1_PIN,
    GPIO_IRQ_EDGE_FALL,
    true,
    &Button_Callback
);

gpio_set_irq_enabled(
    BTN_B2_PIN,
    GPIO_IRQ_EDGE_FALL,
    true
);

gpio_set_irq_enabled(
    BTN_B3_PIN,
    GPIO_IRQ_EDGE_FALL,
    true
);


}

static void Button_Callback(uint gpio, uint32_t events) {

    switch(gpio) {

        case BTN_B1_PIN:
            LOG("Button1 pressed\n");
            Buzzer_PlayDoubleBeep();
            break;

        case BTN_B2_PIN:
            LOG("Button2 pressed\n");

            if (Pedometer_ShouldLevelUp(&s_pedometer)) {
                Buzzer_PlayMarioLevelUp();
                Pedometer_LevelUp(&s_pedometer);
                LOG("Level up\n");
            } else {
                Buzzer_PlayBadSound();
                Haptic_Vibrate(450, HAPTIC_VIBR_MED);
                LOG("No level up\n");
            }
            break;

        case BTN_B3_PIN:
            Haptic_Vibrate(450, HAPTIC_VIBR_MED);
            Buzzer_PlayBadSound();
            LOG("Button3 pressed\n");
            break;
    }
}
