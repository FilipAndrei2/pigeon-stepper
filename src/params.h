#ifndef __SM_PARAMS__
#define __SM_PARAMS__

#include "pigeon_stepper.h"

/*
    f_PWM = f_clock / (clkdivider * (wrap + 1))

    pulse_ms =  clkdivider * (LEVEL / (WRAP + 1)) / (f_clock) 
*/
// configul asta se vrea sa fie pentru frecventa audio de 440Hz (nota A4??)
#define BUZZER_PWM_CLK_DIV      64.0f
#define BUZZER_PWM_WRAP         5325
#define BUZZER_PWM_LEVEL        2662

#define BUZZER_PWM_CLK_DIV_C5      64.0f
#define BUZZER_PWM_WRAP_C5         3731
#define BUZZER_PWM_LEVEL_C5        1865

#define BUZZER_PWM_CLK_DIV_E5      64.0f
#define BUZZER_PWM_WRAP_E5         2961
#define BUZZER_PWM_LEVEL_E5        1480

#define BUZZER_PWM_CLK_DIV_G5      64.0f
#define BUZZER_PWM_WRAP_G5         2490
#define BUZZER_PWM_LEVEL_G5        1245

#endif // __SM_PARAMS__
