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

static ExitCode_t initBuzzerPins(void);
static ExitCode_t initDisplayPins(void);
static void initGyroaccelPins(void);
static ExitCode_t initPins(void);
static ExitCode_t activateInternalPullUpResistors(void);
static ExitCode_t initDisplayController();

// @returns FALSE for succes, TRUE on error
ExitCode_t init(void) {
    stdio_init_all(); // s-ar putea sa vrem sa verificam si codul de eroare

#ifdef _PS_WAIT_4_USB_
    // Asteapta pana usb e conectat
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    LOG("USB conectat!\n");
#endif

    if (cyw43_arch_init()) { // initializeaza  wireless
        return FAIL;
    }

    if (SPI_PortInit()) {
        return FAIL;
    }

    if (initPins()) {
        return FAIL;
    }
    
    activateInternalPullUpResistors();

    // i2c gya port init
    i2c_init(GYA_I2C_PORT, 
        400 * 1000 // baudrate = 400Khz
    );

    if (initDisplayController() == FAIL) {
        return FAIL;
    }
    
    if (GYA_Init()) {
        return FAIL;
    }

    
    return SUCCESS;
}

ExitCode_t mainLoop(void) {
    Buzzer_PlayDoubleBeep();
    sleep_ms(3000);
    Buzzer_PlayMarioLevelUp();
    sleep_ms(3000);
    Buzzer_PlayGoodSound();
    sleep_ms(3000);
    Buzzer_PlayBadSound();
    sleep_ms(3000);

    while (1) {
        // playFullSound();
        // displayTest();
        // Test_Gya();
    }

    return FAIL; // ideal, niciodata atins
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

    return SUCCESS;
}

static ExitCode_t activateInternalPullUpResistors() {

    // activam rezistentele pull-up interne ale pico
    gpio_pull_up(GYA_SDA_PIN);
    gpio_pull_up(GYA_SCL_PIN);

}

static ExitCode_t initDisplayController() {
    // Dam un reset la pornire
    gpio_put(DISPLAY_RES, 0);
    sleep_ms(50);
    gpio_put(DISPLAY_RES, 1);
    sleep_ms(120);    

    // Trimitem niste comenzi
    // TODO: da ti seama ce comenzi
    Display_SendCmd(0x11); 
    sleep_ms(120);        
    Display_SendCmd(0x3A); 
    Display_SendData(0x05); 
    Display_SendCmd(0x36);
    Display_SendData(0x00); 
    Display_SendCmd(0x29);
    sleep_ms(20);

    return SUCCESS;
}

ExitCode_t displaySetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Setare limite coloane (X)
    Display_SendCmd(0x2A); // CASET
    Display_SendData(x0 >> 8); Display_SendData(x0 & 0xFF);
    Display_SendData(x1 >> 8); Display_SendData(x1 & 0xFF);

    // Setare limite randuri (Y)
    Display_SendCmd(0x2B); // RASET
    Display_SendData(y0 >> 8); Display_SendData(y0 & 0xFF);
    Display_SendData(y1 >> 8); Display_SendData(y1 & 0xFF);

    Display_SendCmd(0x2C); 
    
    return SUCCESS;
}

ExitCode_t displaySendBuffer(const uint8_t* buffer, size_t len) {
    gpio_put(DISPLAY_CS, SPI_START_COM);
    gpio_put(DISPLAY_DC, SPI_BUFFER_SEND); // DC = data
    
    if (1 != spi_write_blocking(DISPLAY_SPI_PORT, buffer, 1)) {
        LOG("displaySendBuffer() eroare interna rpi sdk");
        return FAIL;
    }

    // Dezactivam slave-ul spi
    gpio_put(DISPLAY_CS, SPI_END_COM);
    return SUCCESS;
} 

ExitCode_t displaySendData(uint8_t data) {
    gpio_put(DISPLAY_CS, SPI_START_COM);
    gpio_put(DISPLAY_DC, SPI_BUFFER_SEND); // DC = data
    
    if (1 != spi_write_blocking(DISPLAY_SPI_PORT, &data, 1)) {
        LOG("displaySendData(): eroare interna rpi sdk");
        return FAIL;
    }

    gpio_put(DISPLAY_CS, SPI_END_COM);
    return SUCCESS;
}

ExitCode_t displaySendCmd(uint8_t cmd) {
    gpio_put(DISPLAY_CS, SPI_START_COM);
    gpio_put(DISPLAY_DC, SPI_CMD_SEND); // DC = command
     // CS low
    if (1 != spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1)) {
        LOG("displaySendCmd(): eroare interna rpi sdk");
        return FAIL;
    }

    gpio_put(DISPLAY_CS, SPI_END_COM);

    return SUCCESS;
}

