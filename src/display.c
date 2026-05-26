#include "pico/stdlib.h"

#include "pigeon_stepper.h"
#include "display.h"
#include "pinout.h"
#include "spi.h"
#include "utils.h"

// helpers
static inline void put_pixel(
    uint16_t* buf,
    int x,
    int y,
    uint16_t color
) {
    if (x < 0 || y < 0) return;
    if (x >= PIGEON_RESOLUTION_WIDTH) return;
    if (y >= PIGEON_RESOLUTION_HEIGHT) return;

    buf[y * PIGEON_RESOLUTION_WIDTH + x] = (color << 8) | (color >> 8);
}

static void fill_bg(uint16_t* buf, uint16_t color) {

    for (size_t i = 0;
         i < PIGEON_RESOLUTION_WIDTH * PIGEON_RESOLUTION_HEIGHT;
         i++) {

        buf[i] = color;
    }
}

static void draw_circle(
    uint16_t* buf,
    int cx,
    int cy,
    int r,
    uint16_t color
) {
    for (int y = -r; y <= r; y++) {

        for (int x = -r; x <= r; x++) {

            if ((x*x + y*y) <= r*r) {
                put_pixel(buf, cx + x, cy + y, color);
            }
        }
    }
}

static void draw_ellipse(
    uint16_t* buf,
    int cx,
    int cy,
    int rx,
    int ry,
    uint16_t color
) {

    for (int y = -ry; y <= ry; y++) {

        for (int x = -rx; x <= rx; x++) {

            int lhs = (x * x) * (ry * ry)
                    + (y * y) * (rx * rx);

            int rhs = (rx * rx) * (ry * ry);

            if (lhs <= rhs) {
                put_pixel(buf, cx + x, cy + y, color);
            }
        }
    }
}

static void draw_line(
    uint16_t* buf,
    int x0,
    int y0,
    int x1,
    int y1,
    uint16_t color
) {

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;

    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;

    int err = dx + dy;

    while (1) {

        put_pixel(buf, x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

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

    fill_bg(s_pigeonBuffer, COLOR_SKY_BG);

    // nori
    draw_circle(s_pigeonBuffer, 15, 10, 8, COLOR_CLOUD_WHITE);
    draw_circle(s_pigeonBuffer, 23, 10, 8, COLOR_CLOUD_WHITE);

    // corp
    draw_ellipse(
        s_pigeonBuffer,
        40,
        35,
        22,
        16,
        COLOR_PIGEON_BODY_LIGHT
    );

    // cap
    draw_circle(
        s_pigeonBuffer,
        58,
        23,
        10,
        COLOR_PIGEON_BODY_DARK
    );

    // gat
    draw_circle(
        s_pigeonBuffer,
        50,
        30,
        6,
        COLOR_PIGEON_NECK
    );

    // aripa
    switch (this->state) {

        case PIGEON_IDLE:

            draw_ellipse(
                s_pigeonBuffer,
                35,
                35,
                14,
                8,
                COLOR_PIGEON_WING
            );

            break;

        case PIGEON_WINGS:

            draw_ellipse(
                s_pigeonBuffer,
                35,
                24,
                18,
                7,
                COLOR_PIGEON_WING
            );

            break;

        case PIGEON_EAT:

            draw_ellipse(
                s_pigeonBuffer,
                35,
                40,
                15,
                6,
                COLOR_PIGEON_WING
            );

            break;
    }

    // ochi
    draw_circle(
        s_pigeonBuffer,
        61,
        21,
        2,
        COLOR_PIGEON_EYE_RING
    );

    draw_circle(
        s_pigeonBuffer,
        61,
        21,
        1,
        COLOR_EYE
    );

    // cioc
    draw_line(
        s_pigeonBuffer,
        67,
        24,
        75,
        26,
        COLOR_PIGEON_BEAK
    );

    draw_line(
        s_pigeonBuffer,
        67,
        25,
        75,
        27,
        COLOR_PIGEON_BEAK
    );

    // picioare
    draw_line(
        s_pigeonBuffer,
        35,
        49,
        33,
        57,
        COLOR_PIGEON_FEET
    );

    draw_line(
        s_pigeonBuffer,
        45,
        49,
        47,
        57,
        COLOR_PIGEON_FEET
    );

    // trimite pe display
    Display_SetWindow(
        24,
        50,
        24 + PIGEON_RESOLUTION_WIDTH - 1,
        50 + PIGEON_RESOLUTION_HEIGHT - 1
    );

    Display_WritePixels(
        (uint8_t*)s_pigeonBuffer,
        sizeof(s_pigeonBuffer)
    );
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