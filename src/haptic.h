#ifndef _PS_HAPTIC_H_
#define _PS_HAPTIC_H_

// Comenteaza daca vrei pwm, dar sa stii ca nu vibreaza tare cu pwm.
#define _HAPTIC_NO_PWM_

#include <stdint.h>
#include <stddef.h>

#include "pigeon_stepper.h"
#include "types.h"

void Haptic_Init(void);
ExitCode_t Haptic_Vibrate(uint16_t timeMs, uint8_t intensity);

#define HAPTIC_VIBR_OFF (0)
#define HAPTIC_VIBR_LOW (90)
#define HAPTIC_VIBR_MED (130)
#define HAPTIC_VIBR_HIGH (250)

#endif // _PS_HAPTIC_H_