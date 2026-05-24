#ifndef __SM_PARAMS__
#define __SM_PARAMS__

/*
    f_PWM = f_clock / (clkdivider * (wrap + 1))

    pulse_ms =  clkdivider * (LEVEL / (WRAP + 1)) / (f_clock) 
*/
// configul asta se vrea sa fie pentru frecventa audio de 440Hz (nota A4??)
#define BUZZER_PWM_CLK_DIV      64.0f
#define BUZZER_PWM_WRAP         5325
#define BUZZER_PWM_LEVEL        2662

#endif // __SM_PARAMS__