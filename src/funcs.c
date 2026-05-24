#include "hardware/pwm.h"
#include "hardware/i2c.h"

#define __SM_DEBUG__

#include "sm.h"
#include "funcs.h"
#include "hardware.h"
#include "gyroaccel.h"
#include "pinout.h"
#include "params.h"

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

void initGyroaccelPins() {
    gpio_set_function(GYA_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(GYA_SCL_PIN, GPIO_FUNC_I2C);

    gpio_init(GYA_INT_PIN);
    gpio_set_dir(GYA_INT_PIN, GPIO_IN);
}

ExitCode_t initPins() {
    if (initBuzzerPins()) {
        LOG("initBuzzerPins(): Nu s-a putut initializa buzzerul\n");
    }

    if (initDisplayPins()) {
        LOG("initDisplayPins(): Nu s-a putut initializa displayul\n");
    }

    initGyroaccelPins();

    return SUCCESS;
}

static ExitCode_t initDisplayController() {
    // Dam un reset la pornire
    gpio_put(DISPLAY_RES, 0);
    sleep_ms(50);
    gpio_put(DISPLAY_RES, 1);
    sleep_ms(120);    

    // Trimitem niste comenzi
    // TODO: da ti seama ce comenzi
    displaySendCmd(0x11); 
    sleep_ms(120);        
    displaySendCmd(0x3A); 
    displaySendData(0x05); 
    displaySendCmd(0x36);
    displaySendData(0x00); 
    displaySendCmd(0x29);
    sleep_ms(20);

    return SUCCESS;
}

ExitCode_t displaySetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Setare limite coloane (X)
    displaySendCmd(0x2A); // CASET
    displaySendData(x0 >> 8); displaySendData(x0 & 0xFF);
    displaySendData(x1 >> 8); displaySendData(x1 & 0xFF);

    // Setare limite randuri (Y)
    displaySendCmd(0x2B); // RASET
    displaySendData(y0 >> 8); displaySendData(y0 & 0xFF);
    displaySendData(y1 >> 8); displaySendData(y1 & 0xFF);

    displaySendCmd(0x2C); 
    
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

// @returns FALSE for succes, TRUE on error
ExitCode_t init(void) {
    stdio_init_all(); // s-ar putea sa vrem sa verificam si codul de eroare



    if (cyw43_arch_init()) { // initializeaza  wireless
        return FAIL;
    }

    // spi display port init
    spi_init(DISPLAY_SPI_PORT, 20 * 1000 * 1000); // 40 MHz
    spi_set_format(DISPLAY_SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    if (initPins()) {
        return FAIL;
    }

    // activam rezistentele pull-up interne ale pico
    gpio_pull_up(GYA_SDA_PIN);
    gpio_pull_up(GYA_SCL_PIN);

    // i2c gya port init
    i2c_init(GYA_I2C_PORT, 
        400 * 1000 // baudrate = 400Khz
    );

    // Secventa de init a displayului

    if (initDisplayController() == FAIL) {
        return FAIL;
    }
    
    if (initGya()) {
        return FAIL;
    }

#ifdef __SM_DEBUG__
    // Asteapta pana usb e conectat
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    LOG("USB conectat!\n");
#endif
    
    return SUCCESS;
}

ExitCode_t mainLoop(void) {
    playFullSound();

    while (1) {
        // playFullSound();
        // displayTest();
        gyaTest();
    }
    
    return SUCCESS;
}