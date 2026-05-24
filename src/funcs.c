#include "hardware/pwm.h"

#include "sm.h"
#include "funcs.h"
#include "hardware.h"
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


ExitCode_t initPins() {
    if (initBuzzerPins()) {
        LOG("initBuzzerPins(): Nu s-a putut initializa buzzerul\n");
    }

    if (initDisplayPins()) {
        LOG("initDisplayPins(): Nu s-a putut initializa displayul\n")
    }

    gpio_set_dir(BUZZER_IO, GPIO_OUT);
    return SUCCESS;
}

static ExitCode_t displaySendBuffer(const uint8_t* buffer, size_t len) {
    gpio_put(DISPLAY_DC, SPI_BUFFER_SEND); // DC = data
    gpio_put(DISPLAY_CS, SPI_START_COM);
    
    if (len != spi_write_blocking(DISPLAY_SPI_PORT, buffer, len)) {
        LOG("displaySendBuffer() eroare interna rpi sdk");
        return FAIL;
    }

    // Dezactivam slave-ul spi
    gpio_put(DISPLAY_CS, SPI_END_COM);
    return SUCCESS;
} 

static ExitCode_t displaySendCmd(uint8_t cmd) {
    gpio_put(DISPLAY_DC, SPI_CMD_SEND); // DC = command
    gpio_put(DISPLAY_CS, SPI_START_COM); // CS low
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

    if (initPins()) {
        return FAIL;
    }

    return SUCCESS;
}

ExitCode_t mainLoop(void) {
    drawTest();

    return SUCCESS;
}