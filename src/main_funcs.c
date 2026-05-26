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

    if (initDisplayController() == FAIL) {
        return FAIL;
    }
    
    if (GYA_Init()) {
        return FAIL;
    }

    Haptic_Init();

    Buttons_Init();
    
    // initializari obiecte
    Stepper_Init(&s_stepper);
    Pedometer_Init(&s_pedometer);
    
    Frame_Init(&s_frame, Pedometer_GetLevel(), Pedometer_GetSteps());

    return SUCCESS;
}

void displayLoop(void) {

    PigeonState pigeonState = 0;
    while (1) {
        
        pigeonState = (pigeonState + 1) % 3;

        Frame_UpdatePigeon(&s_frame, pigeonState);
        Frame_DrawPigeon(&s_frame);
        
        // nush exact daca e nevoie de delay, 
        // activeaza in caz de flickering ig
        // sleep_ms(100);
    }
}

ExitCode_t mainLoop(void) {
    PigeonState pigeonState = PIGEON_IDLE;

    Buzzer_PlayStartupSound();
    while (1) {
        
        if (Stepper_DetectStep(&s_stepper)) {
            pigeonState = PIGEON_WINGS;
            Pedometer_AddStep(&s_pedometer);
        } else {
            pigeonState = PIGEON_IDLE;
        }
        VLOG("Level: %zu; Pasi: %zu;\n", Pedometer_GetLevel(&s_pedometer), Pedometer_GetSteps(&s_pedometer));
        
        Frame_UpdateState)&s_frame, pigeonState);
        Frame_UpdateSteps(&s_frame, Pedometer_GetSteps(&s_pedometer);
        Frame_UpdateLevel(&s_frame, Pedometer_GetLevel(&s_pedometer);
        
        sleep_ms(2000);
    }

    return FAIL; // Teoretic, bucla infinita nu ar trebui să ajunga niciodata aici
}

static ExitCode_t initBuzzerPins() {

    // daca apelez gpio_set_function(...), 
    // nu mai am nevoie de apel catre gpio_init(uint gpio)
    gpio_set_function(BUZZER_IO, GPIO_FUNC_PWM);

    return SUCCESS;
}

static ExitCode_t initDisplayPins() {
    gpio_set_function(DISPLAY_CLK, GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_SDA, GPIO_FUNC_SPI);
    
    // DC, RES si CS trebuie initializat normal
    gpio_init(DISPLAY_DC);
    gpio_set_dir(DISPLAY_DC, GPIO_OUT);

    gpio_init(DISPLAY_RES);
    gpio_set_dir(DISPLAY_RES, GPIO_OUT);

    gpio_init(DISPLAY_CS);
    gpio_set_dir(DISPLAY_CS, GPIO_OUT);
    
    gpio_put(DISPLAY_CS, SPI_END_COM);
    
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

    if (initDisplayPins()) {
        LOG("initDisplayPins(): Nu s-a putut initializa displayul\n");
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

static ExitCode_t initDisplayController() {
    gpio_put(DISPLAY_RES, 1);
    sleep_ms(10);

    gpio_put(DISPLAY_RES, 0);
    sleep_ms(50);

    gpio_put(DISPLAY_RES, 1);
    sleep_ms(120);

    Display_SendCmd(0x01); // SWRESET
    sleep_ms(150);

    Display_SendCmd(0x11); // SLPOUT
    sleep_ms(120);

    // --- SETARE CULOARE: COLMOD (0x3A) = 0x55 (16-bit) ---
    gpio_put(DISPLAY_CS, 0); // CS LOW fix pentru această comandă + parametru
    uint8_t cmd_colmod = 0x3A; 
    uint8_t arg_colmod = 0x55; // RGB565
    gpio_put(DISPLAY_DC, 0);
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd_colmod, 1);
    gpio_put(DISPLAY_DC, 1);
    spi_write_blocking(DISPLAY_SPI_PORT, &arg_colmod, 1);
    gpio_put(DISPLAY_CS, 1); // CS HIGH

    // --- SETARE ORIENTARE: MADCTL (0x36) = 0x00 ---
    gpio_put(DISPLAY_CS, 0);
    uint8_t cmd_madctl = 0x36;
    uint8_t arg_madctl = 0x00;
    gpio_put(DISPLAY_DC, 0);
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd_madctl, 1);
    gpio_put(DISPLAY_DC, 1);
    spi_write_blocking(DISPLAY_SPI_PORT, &arg_madctl, 1);
    gpio_put(DISPLAY_CS, 1);

    Display_SendCmd(0x21); // INVON (Inversare culori)
    Display_SendCmd(0x13); // NORON (Normal display)
    sleep_ms(10);
    Display_SendCmd(0x29); // DISPON (Pornește ecranul)
    sleep_ms(120);

    return SUCCESS;
}

