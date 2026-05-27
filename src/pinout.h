#ifndef __PINOUT_H__
#define __PINOUT_H_

#include "pigeon_stepper.h"

/*
    TODO: REDENUMESTE CONSTANTELE ASTA SA URMEZE CONVENTIA
    NUMEDEVICE_NUMEPIN_PIN
*/

// Buzzer pins
#define BUZZER_IO               (0)

// Display pins
#define DISPLAY_CLK             (14)
#define DISPLAY_SDA             (15)
#define DISPLAY_CS              (13)

#define DISPLAY_RES             (21)
#define DISPLAY_DC              (22)

// GYA pins
#define GYA_SCL_PIN             (7)
#define GYA_SDA_PIN             (6)
#define GYA_INT_PIN             (4)

// 3-Button module pins
#define BTN_B1_PIN		        (16)
#define BTN_B2_PIN		        (17)
#define BTN_B3_PIN		        (18)

// Haptic driver pins
#define HAPTIC_PWM_PIN		    (20)

#endif // __PINOUT_H_