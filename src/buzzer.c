#include "pigeon_stepper.h"
#include "buzzer.h"

#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "utils.h"

// Frecvente de note: Toata gama cromatica C1-C7
// NOTE: Tinem doar frecventele (1 float) si calculam parametrii necesari pt play la rt ca sa economisim memorie.
const float NOTE_C0_FREQ  = 16.35f;
const float NOTE_CS0_FREQ = 17.32f;
const float NOTE_D0_FREQ  = 18.35f;
const float NOTE_DS0_FREQ = 19.45f;
const float NOTE_E0_FREQ  = 20.60f;
const float NOTE_F0_FREQ  = 21.83f;
const float NOTE_FS0_FREQ = 23.12f;
const float NOTE_G0_FREQ  = 24.50f;
const float NOTE_GS0_FREQ = 25.96f;
const float NOTE_A0_FREQ  = 27.50f;
const float NOTE_AS0_FREQ = 29.14f;
const float NOTE_B0_FREQ  = 30.87f;

/* C1 */
const float NOTE_C1_FREQ  = 32.70f;
const float NOTE_CS1_FREQ = 34.65f;
const float NOTE_D1_FREQ  = 36.71f;
const float NOTE_DS1_FREQ = 38.89f;
const float NOTE_E1_FREQ  = 41.20f;
const float NOTE_F1_FREQ  = 43.65f;
const float NOTE_FS1_FREQ = 46.25f;
const float NOTE_G1_FREQ  = 49.00f;
const float NOTE_GS1_FREQ = 51.91f;
const float NOTE_A1_FREQ  = 55.00f;
const float NOTE_AS1_FREQ = 58.27f;
const float NOTE_B1_FREQ  = 61.74f;

/* C2 */
const float NOTE_C2_FREQ  = 65.41f;
const float NOTE_CS2_FREQ = 69.30f;
const float NOTE_D2_FREQ  = 73.42f;
const float NOTE_DS2_FREQ = 77.78f;
const float NOTE_E2_FREQ  = 82.41f;
const float NOTE_F2_FREQ  = 87.31f;
const float NOTE_FS2_FREQ = 92.50f;
const float NOTE_G2_FREQ  = 98.00f;
const float NOTE_GS2_FREQ = 103.83f;
const float NOTE_A2_FREQ  = 110.00f;
const float NOTE_AS2_FREQ = 116.54f;
const float NOTE_B2_FREQ  = 123.47f;

/* C3 */
const float NOTE_C3_FREQ  = 130.81f;
const float NOTE_CS3_FREQ = 138.59f;
const float NOTE_D3_FREQ  = 146.83f;
const float NOTE_DS3_FREQ = 155.56f;
const float NOTE_E3_FREQ  = 164.81f;
const float NOTE_F3_FREQ  = 174.61f;
const float NOTE_FS3_FREQ = 185.00f;
const float NOTE_G3_FREQ  = 196.00f;
const float NOTE_GS3_FREQ = 207.65f;
const float NOTE_A3_FREQ  = 220.00f;
const float NOTE_AS3_FREQ = 233.08f;
const float NOTE_B3_FREQ  = 246.94f;

/* C4 (Middle C) */
const float NOTE_C4_FREQ  = 261.63f;
const float NOTE_CS4_FREQ = 277.18f;
const float NOTE_D4_FREQ  = 293.66f;
const float NOTE_DS4_FREQ = 311.13f;
const float NOTE_E4_FREQ  = 329.63f;
const float NOTE_F4_FREQ  = 349.23f;
const float NOTE_FS4_FREQ = 369.99f;
const float NOTE_G4_FREQ  = 392.00f;
const float NOTE_GS4_FREQ = 415.30f;
const float NOTE_A4_FREQ  = 440.00f;
const float NOTE_AS4_FREQ = 466.16f;
const float NOTE_B4_FREQ  = 493.88f;

/* C5 */
const float NOTE_C5_FREQ  = 523.25f;
const float NOTE_CS5_FREQ = 554.37f;
const float NOTE_D5_FREQ  = 587.33f;
const float NOTE_DS5_FREQ = 622.25f;
const float NOTE_E5_FREQ  = 659.25f;
const float NOTE_F5_FREQ  = 698.46f;
const float NOTE_FS5_FREQ = 739.99f;
const float NOTE_G5_FREQ  = 783.99f;
const float NOTE_GS5_FREQ = 830.61f;
const float NOTE_A5_FREQ  = 880.00f;
const float NOTE_AS5_FREQ = 932.33f;
const float NOTE_B5_FREQ  = 987.77f;

/* C6 */
const float NOTE_C6_FREQ  = 1046.50f;
const float NOTE_CS6_FREQ = 1108.73f;
const float NOTE_D6_FREQ  = 1174.66f;
const float NOTE_DS6_FREQ = 1244.51f;
const float NOTE_E6_FREQ  = 1318.51f;
const float NOTE_F6_FREQ  = 1396.91f;
const float NOTE_FS6_FREQ = 1479.98f;
const float NOTE_G6_FREQ  = 1567.98f;
const float NOTE_GS6_FREQ = 1661.22f;
const float NOTE_A6_FREQ  = 1760.00f;
const float NOTE_AS6_FREQ = 1864.66f;
const float NOTE_B6_FREQ  = 1975.53f;

/* C7 */
const float NOTE_C7_FREQ  = 2093.00f;
const float NOTE_CS7_FREQ = 2217.46f;
const float NOTE_D7_FREQ  = 2349.32f;
const float NOTE_DS7_FREQ = 2489.02f;
const float NOTE_E7_FREQ  = 2637.02f;
const float NOTE_F7_FREQ  = 2793.83f;
const float NOTE_FS7_FREQ = 2959.96f;
const float NOTE_G7_FREQ  = 3135.96f;
const float NOTE_GS7_FREQ = 3322.44f;
const float NOTE_A7_FREQ  = 3520.00f;
const float NOTE_AS7_FREQ = 3729.31f;
const float NOTE_B7_FREQ  = 3951.07f;

// Private
static void Buzzer_PlayNote(const MusicalNote* note, uint32_t duration_ms);

// C4  C#4  D4  D#4  E4  F4
void Buzzer_PlayStartupSound() {
    const MusicalNote C4    = Buzzer_MakeNote(NOTE_C4_FREQ);
    const MusicalNote CS4   = Buzzer_MakeNote(NOTE_CS4_FREQ);
    const MusicalNote D4    = Buzzer_MakeNote(NOTE_D4_FREQ);
    const MusicalNote DS4   = Buzzer_MakeNote(NOTE_DS4_FREQ);
    const MusicalNote E4    = Buzzer_MakeNote(NOTE_E4_FREQ);
    const MusicalNote F4    = Buzzer_MakeNote(NOTE_F4_FREQ);


    Buzzer_PlayNote(&C4, 120); 
    sleep_ms(40);

    Buzzer_PlayNote(&CS4, 120); 
    sleep_ms(40);
    
    Buzzer_PlayNote(&D4, 120); 
    sleep_ms(40);

    Buzzer_PlayNote(&DS4, 120); 
    sleep_ms(40);

    Buzzer_PlayNote(&E4, 120); 
    sleep_ms(40);

    Buzzer_PlayNote(&F4, 120); 
    sleep_ms(150);
}

/// @brief Do, Mi, Sol la 120ms fiecare
/// @return 
ExitCode_t Buzzer_PlayGoodSound() {
    LOG("Ding\n");

    // obtinem mai intai lucrurile necesare
    const MusicalNote C4 = Buzzer_MakeNote(NOTE_C4_FREQ);
    const MusicalNote E4 = Buzzer_MakeNote(NOTE_E4_FREQ);
    const MusicalNote G4 = Buzzer_MakeNote(NOTE_G4_FREQ);

    Buzzer_PlayNote(&C4, 120); 
    Buzzer_PlayNote(&E4, 120); 
    Buzzer_PlayNote(&G4, 180);

    sleep_ms(20);
    return SUCCESS;
}

/// @brief MI4,  pause, MI5
/// @return 
ExitCode_t Buzzer_PlayDoubleBeep() {
    LOG("Double Beep Sound playing\n");

    const MusicalNote E5 = Buzzer_MakeNote(NOTE_E5_FREQ);
    const MusicalNote E6 = Buzzer_MakeNote(NOTE_E6_FREQ);

    Buzzer_PlayNote(&E5, 120); 
    sleep_ms(222);
    Buzzer_PlayNote(&E6, 180);
    return SUCCESS;
}

/// @brief E4 G4 E5
/// @return 
ExitCode_t Buzzer_PlayMarioLevelUp() {
    LOG("Mario Level Up Sound playing\n");

    const MusicalNote E4 = Buzzer_MakeNote(NOTE_E4_FREQ);
    const MusicalNote G4 = Buzzer_MakeNote(NOTE_G4_FREQ);
    const MusicalNote E5 = Buzzer_MakeNote(NOTE_E5_FREQ);
    const MusicalNote G5 = Buzzer_MakeNote(NOTE_G5_FREQ);

    Buzzer_PlayNote(&E4, 100); sleep_ms(50);
    Buzzer_PlayNote(&G4, 100); sleep_ms(50);
    Buzzer_PlayNote(&E5, 100); sleep_ms(50);
    Buzzer_PlayNote(&G5, 100);

    sleep_ms(20);
    return SUCCESS;
}

/// @brief SOL4 MI4 DO4
/// @return 
ExitCode_t Buzzer_PlayBadSound() {
    // TODO: functia asta canta aceeasi melodie ca Buzzer_GoodSound, schimba notele ca sa transmita ca ceva rau s-a intamplat
    LOG("Ding\n");

    // obtinem mai intai lucrurile necesare
    const MusicalNote C4 = Buzzer_MakeNote(NOTE_C4_FREQ);
    const MusicalNote E4 = Buzzer_MakeNote(NOTE_E4_FREQ);
    const MusicalNote G4 = Buzzer_MakeNote(NOTE_G4_FREQ);

    Buzzer_PlayNote(&G4, 180); 
    Buzzer_PlayNote(&E4, 120); 
    Buzzer_PlayNote(&C4, 120); 
    sleep_ms(20);
    return SUCCESS;
}

static void Buzzer_PlayNote(const MusicalNote* note, uint32_t duration_ms) {
    uint slice = pwm_gpio_to_slice_num(BUZZER_IO);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, note->clkdiv);
    pwm_config_set_wrap(&config, note->wrap);

    pwm_init(slice, &config, true);

    pwm_set_gpio_level(BUZZER_IO, note->level);

    pwm_set_enabled(slice, true);

    sleep_ms(duration_ms);

    pwm_set_gpio_level(BUZZER_IO, 0);
    
    sleep_ms(20);
}
