#include "pigeon_stepper.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"

#define __SM_DEBUG__

#include "main_funcs.h"
#include "gya.h"
#include "params.h"
#include "pinout.h"
#include "test_funcs.h"
#include "utils.h"
#include "spi.h"
#include "display.h"
#include "buzzer.h"
#include "haptic.h"
#include "button.h"
#include "pedometer.h"
#include "display.h"

static ExitCode_t initBuzzerPins(void);
static ExitCode_t initDisplayPins(void);
static void initGyroaccelPins(void);
static ExitCode_t initPins(void);
static ExitCode_t activateInternalPullUpResistors(void);
static ExitCode_t initDisplayController();

////////////////////////////////////////////////////////////////////////////////
///                     GLOBALE (iertati-ne doamna Serban)
////////////////////////////////////////////////////////////////////////////////
Stepper      s_stepper   ;
Pedometer    s_pedometer ;
Frame        s_frame     ;

// @returns FALSE for succes, TRUE on error
ExitCode_t init(void) {
    stdio_init_all(); // s-ar putea sa vrem sa verificam si codul de eroare
    
    // NOTE: Poate nu vrem sa initializam wireless??
    // if (cyw43_arch_init()) { // initializeaza  wireless
    //     return FAIL;
    // }

#ifdef _PS_WAIT_4_USB_
    // Asteapta pana usb e conectat
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    LOG("USB conectat!\n");
#endif

    if (SPI_PortInit()) {
        return FAIL;
    }

    // i2c gya port init
    i2c_init(GYA_I2C_PORT, 
        400 * 1000 // baudrate = 400Khz
    );

    if (initPins()) {
        return FAIL;
    }
    
    activateInternalPullUpResistors();


    
    if (GYA_Init()) {
        return FAIL;
    }


    // Display_Init();
    Haptic_Init();

    Buttons_Init();
    
    // initializari obiecte
    Stepper_Init(&s_stepper);
    Pedometer_Init(&s_pedometer);
    
    // Frame_Init(&s_frame, Pedometer_GetLevel(&s_pedometer), Pedometer_GetSteps(&s_pedometer));

    return SUCCESS;
}

// FILIP 28.05.2026:
// Este 4:15 AM si ziua asta trebuie sa prezint proiectul
// Tocmai ce am topit o parte din buzzer cu ciocanul de lipit
// A trebuit sa tai firele de la display ca sa imi dau commit la ideea ca NU vom avea display pe masina
// rip display thread.
void displayLoop(void) {

    while (1) {
        
        sleep_ms(300);
        
        // Frame_DrawPigeon(&s_frame);
        // Frame_DrawText(&s_frame);
        // nush exact daca e nevoie de delay, 
        // activeaza in caz de flickering ig
        // sleep_ms(100);
    }
}

ExitCode_t mainLoop(void) {
    Buzzer_PlayStartupSound();
    Haptic_Vibrate(700, HAPTIC_VIBR_LOW);
    sleep_ms(2000);
    Haptic_Vibrate(700, HAPTIC_VIBR_MED);

    while (1) {
        
        if (Stepper_DetectStep(&s_stepper)) {
            Buzzer_PlayGoodSound();
            Pedometer_AddStep(&s_pedometer);
        } 

        VLOG("Level: %zu; Pasi: %zu;\n", Pedometer_GetLevel(&s_pedometer), Pedometer_GetSteps(&s_pedometer));
        
        // Frame_UpdateSteps(&s_frame, Pedometer_GetSteps(&s_pedometer));
        // Frame_UpdateLevel(&s_frame, Pedometer_GetLevel(&s_pedometer));
        // if (pigeonWingsMs > 0) {
        //     Frame_UpdateState(&s_frame, PIGEON_WINGS);
        //     pigeonWingsMs -= 1;
        // } else {
        //     Frame_UpdateState(&s_frame, PIGEON_IDLE);
        // }
    }

    return FAIL; // Teoretic, bucla infinita nu ar trebui să ajunga niciodata aici
}

static ExitCode_t initBuzzerPins() {

    // daca apelez gpio_set_function(...), 
    // nu mai am nevoie de apel catre gpio_init(uint gpio)
    gpio_set_function(BUZZER_IO, GPIO_FUNC_PWM);

    return SUCCESS;
}



static void initGyroaccelPins(void) {
    gpio_set_function(GYA_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(GYA_SCL_PIN, GPIO_FUNC_I2C);

    gpio_init(GYA_INT_PIN);
    gpio_set_dir(GYA_INT_PIN, GPIO_IN);
}



static ExitCode_t initPins(void) {
    if (initBuzzerPins()) {
        LOG("initBuzzerPins(): Nu s-a putut initializa buzzerul\n");
    }
    
    initGyroaccelPins();

    // Initializare haptic
    // daca apelez gpio_set_function(...), 
    // nu mai am nevoie de apel catre gpio_init(uint gpio)
    gpio_set_function(HAPTIC_PWM_PIN, GPIO_FUNC_PWM);

    return SUCCESS;
}

static ExitCode_t activateInternalPullUpResistors() {

    // activam rezistentele pull-up interne ale pico
    gpio_pull_up(GYA_SDA_PIN);
    gpio_pull_up(GYA_SCL_PIN);
    return SUCCESS;
}

