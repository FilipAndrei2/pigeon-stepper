#ifndef _PS_BUZZER_H_
#define _PS_BUZZER_H_

#include "pigeon_stepper.h"

#include "hardware/clocks.h"

#include "types.h"
#include "pinout.h"

ExitCode_t Buzzer_PlayBadSound();
ExitCode_t Buzzer_PlayDoubleBeep();
ExitCode_t Buzzer_PlayGoodSound();
ExitCode_t Buzzer_PlayMarioLevelUp();
typedef struct {
    float clkdiv;
    uint16_t wrap;
    uint16_t level;
} MusicalNote;

static inline MusicalNote Buzzer_MakeNote(float freq) {
    float clkdiv = 4.0f;
    uint32_t sys_clk = clock_get_hz(clk_sys);
    uint16_t wrap = (uint16_t)(sys_clk / (clkdiv * freq)) - 1;

    MusicalNote note = {
        .clkdiv = clkdiv,
        .wrap = wrap,
        .level = wrap / 2
    };

    return note;
}

#endif // _PS_BUZZER_H_
