#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "sm.h"

// Metodele clasei Buzzer 

ExitCode_t playFullSound(void);
void playTone(float clkdiv,
                     uint16_t wrap,
                     uint16_t level,
                     uint32_t duration_ms);
                     
#endif // __HARDWARE_H__