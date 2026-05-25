#ifndef _PS_HAPTIC_H_
#define _PS_HAPTIC_H_

#include <stdint.h>
#include <stddef.h>

#include "pigeon_stepper.h"
#include "types.h"

void Haptic_Init(void);
ExitCode_t Haptic_Vibrate(uint16_t timeMs, uint16_t intensity);

#endif // _PS_HAPTIC_H_