#include "pico/stdlib.h"

#include "pigeon_stepper.h"
#include "display.h"
#include "pinout.h"
#include "spi.h"
#include "utils.h"

void Frame_Init(Frame * this, size_t level, size_t steps) {
    this->state = PIGEON_IDLE;
    this->level = level;
    this->steps = steps;

    this->backgroundColor = COLOR_BG;
}

void Frame_UpdatePigeon(Frame* this, PigeonState newState) {
    this->state = newState;
}

void Frame_UpdateUi(Frame* this, size_t level, size_t steps) {
    this->level = level;
    this->steps = steps;
}

/// @brief Deseneaza doar 
/// '''
/// Level: 
/// Steps:
/// ''',
/// fara a completa cu valorile din stateul jocului 
/// @param  
void Frame_DrawText(Frame* this) {

}

void Frame_DrawLevel(Frame* this) {

}

void Frame_DrawSteps(Frame* this) {
    
}

void Frame_DrawPigeon(Frame* this) {

}

ExitCode_t Display_SendBuffer(const uint8_t* buffer, size_t len) {
    gpio_put(DISPLAY_CS, SPI_START_COM);
    gpio_put(DISPLAY_DC, SPI_BUFFER_SEND); // DC = data
    
    
    if (len != spi_write_blocking(DISPLAY_SPI_PORT, buffer, len)) {
        LOG("displaySendBuffer() eroare interna rpi sdk");
        return FAIL;
    }

    // Dezactivam slave-ul spi
    gpio_put(DISPLAY_CS, SPI_END_COM);
    return SUCCESS;
} 

ExitCode_t Display_SendCmd(uint8_t cmd) {

    gpio_put(DISPLAY_CS, 0);        // start SPI frame
    gpio_put(DISPLAY_DC, 0);        // command mode

    if (spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1) != 1) {
        LOG("Display_SendCmd(): SPI error");
        gpio_put(DISPLAY_CS, 1);
        return FAIL;
    }

    gpio_put(DISPLAY_CS, 1);        // end frame

    return SUCCESS;
}

ExitCode_t Display_SendData(uint8_t data) {
    gpio_put(DISPLAY_CS, 0);
    gpio_put(DISPLAY_DC, SPI_BUFFER_SEND); // DC = data

    if (1 != spi_write_blocking(DISPLAY_SPI_PORT, &data, 1)) {
        LOG("displaySendData(): eroare interna rpi sdk");
        gpio_put(DISPLAY_CS, 1); // Eliberăm magistrala și în caz de eroare!
        return FAIL;
    }

    gpio_put(DISPLAY_CS, 1); // <--- Asta lipsea!
    return SUCCESS;
}

ExitCode_t Display_WritePixels(const uint8_t* data, size_t len) {

    gpio_put(DISPLAY_DC, 1);   // data mode
    gpio_put(DISPLAY_CS, 0);   // start frame

    int written = spi_write_blocking(DISPLAY_SPI_PORT, data, len);

    gpio_put(DISPLAY_CS, 1);   // end frame

    if (written != (int)len) {
        LOG("Display_WritePixels(): SPI error");
        return FAIL;
    }

    return SUCCESS;
}

ExitCode_t Display_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t cmd;
    uint8_t data[4];

    // Tragem CS pe LOW o singură dată pentru TOATA secventa!
    gpio_put(DISPLAY_CS, 0); 

    // --- CASET (Coloane) ---
    cmd = 0x2A;
    data[0] = x0 >> 8; data[1] = x0 & 0xFF; 
    data[2] = x1 >> 8; data[3] = x1 & 0xFF;
    gpio_put(DISPLAY_DC, 0); // Command mode
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1);
    gpio_put(DISPLAY_DC, 1); // Data mode
    spi_write_blocking(DISPLAY_SPI_PORT, data, 4);

    // --- RASET (Rânduri) ---
    cmd = 0x2B;
    data[0] = y0 >> 8; data[1] = y0 & 0xFF; 
    data[2] = y1 >> 8; data[3] = y1 & 0xFF;
    gpio_put(DISPLAY_DC, 0);
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1);
    gpio_put(DISPLAY_DC, 1);
    spi_write_blocking(DISPLAY_SPI_PORT, data, 4);

    // --- RAMWR (Pregătire de pixeli) ---
    cmd = 0x2C;
    gpio_put(DISPLAY_DC, 0);
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1);

    // Gata, ridicăm CS-ul
    gpio_put(DISPLAY_CS, 1); 

    return SUCCESS;
}