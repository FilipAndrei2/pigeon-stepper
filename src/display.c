#include "pico/stdlib.h"

#include "hardware/sync.h"

#include "pigeon_stepper.h"
#include "display.h"
#include "pinout.h"
#include "spi.h"
#include "utils.h"

//////////////////////////////////////////////////
/// LOCKURI STATICE
//////////////////////////////////////////////////
spin_lock_t* s_stateLock = 0U;
uint       s_stateLockNum = 0u;

spin_lock_t* s_levelLock = 0U;
uint       s_levelLockNum = 0u;

spin_lock_t* s_stepsLock = 0U;
uint       s_stepsLockNum = 0u;

///////////////
//GLOBALE
///////////////

uint8_t g_displayBuffer [DISPLAY_RESOLUTION_WIDTH * DISPLAY_RESOLUTION_HEIGHT * DISPLAY_RGB_SIZE];

// Font (doar literele necesare L e v e l : S t p s
static const uint8_t font5x7[128][5] = {
    [' '] = {0,0,0,0,0},

    ['L'] = {0x7F,0x08,0x08,0x08,0x07},
    ['e'] = {0x3C,0x4A,0x4A,0x4A,0x30},
    ['v'] = {0x08,0x10,0x20,0x10,0x08},
    ['l'] = {0x00,0x41,0x7F,0x40,0x00},

    ['S'] = {0x32,0x49,0x49,0x49,0x26},
    ['t'] = {0x10,0x7E,0x10,0x10,0x00},
    ['p'] = {0x7F,0x12,0x12,0x12,0x0C},

    [':'] = {0x00,0x36,0x36,0x00,0x00},

    ['0'] = {0x3E,0x51,0x49,0x45,0x3E},
    ['1'] = {0x00,0x42,0x7F,0x40,0x00},
    ['2'] = {0x42,0x61,0x51,0x49,0x46},
    ['3'] = {0x21,0x41,0x45,0x4B,0x31},
    ['4'] = {0x18,0x14,0x12,0x7F,0x10},
    ['5'] = {0x27,0x45,0x45,0x45,0x39},
    ['6'] = {0x3C,0x4A,0x49,0x49,0x30},
    ['7'] = {0x01,0x71,0x09,0x05,0x03},
    ['8'] = {0x36,0x49,0x49,0x49,0x36},
    ['9'] = {0x06,0x49,0x49,0x29,0x1E},
};

// helpers
static void put_pixel_txt(uint16_t* buf, int x, int y, uint16_t color) {
    if (x < 0 || y < 0) return;
    if (x >= DISPLAY_RESOLUTION_WIDTH) return;
    if (y >= DISPLAY_RESOLUTION_HEIGHT) return;

    buf[y * DISPLAY_RESOLUTION_WIDTH + x] = color;
}

static void draw_char(uint16_t* buf, int x, int y, char c, uint16_t color) {
    if (c < 32 || c > 126) return;

    const uint8_t* glyph = font5x7[(int)c];

    for (int col = 0; col < 5; col++) {
        uint8_t line = glyph[col];

        for (int row = 0; row < 7; row++) {
            if (line & (1 << row)) {
                put_pixel_txt(buf, x + col, y + row, color);
            }
        }
    }
}

static void draw_string(uint16_t* buf, int x, int y, const char* s, uint16_t color) {
    while (*s) {
        draw_char(buf, x, y, *s, color);
        x += 6;
        s++;
    }
}

static inline void put_pixel(
    volatile uint16_t* buf,
    int x,
    int y,
    uint16_t color
) {
    if (x < 0 || y < 0) return;
    if (x >= PIGEON_RESOLUTION_WIDTH) return;
    if (y >= PIGEON_RESOLUTION_HEIGHT) return;

    buf[y * PIGEON_RESOLUTION_WIDTH + x] = (color << 8) | (color >> 8);
}

static void fill_bg(volatile uint16_t* buf, uint16_t color) {

    for (size_t i = 0;
         i < PIGEON_RESOLUTION_WIDTH * PIGEON_RESOLUTION_HEIGHT;
         i++) {

        buf[i] = color;
    }
}

static void draw_circle(
    volatile uint16_t* buf,
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
    volatile uint16_t* buf,
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
    volatile uint16_t* buf,
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

size_t Frame_GetLevel(Frame *this) {
    uint32_t state = spin_lock_blocking(s_levelLock);
    size_t level = this->level;
    spin_unlock(s_levelLock, state);
    return level;
}

size_t Frame_GetSteps(Frame *this) {
    uint32_t state = spin_lock_blocking(s_stepsLock);
    size_t steps = this->steps;
    spin_unlock(s_stepsLock, state);
    return steps;
}

PigeonStates Frame_GetPigeonState(Frame *this) {
    uint32_t lockState = spin_lock_blocking(s_stateLock);
    PigeonStates state = this->state;
    spin_unlock(s_stateLock, lockState);
    return state;
}

void Frame_Init(Frame * this, size_t level, size_t steps) {
    this->state = PIGEON_IDLE;
    this->level = level;
    this->steps = steps;

    this->backgroundColor = COLOR_BG;
    s_stateLockNum = spin_lock_claim_unused(true);
    s_stateLock = spin_lock_instance(s_stateLockNum);

    s_levelLockNum = spin_lock_claim_unused(true);
    s_levelLock = spin_lock_instance(s_levelLockNum);

    s_stepsLockNum = spin_lock_claim_unused(true);
    s_stepsLock = spin_lock_instance(s_stepsLockNum);
}

void Frame_UpdateState(Frame* this, PigeonStates newState) {
    uint32_t state = spin_lock_blocking(s_stateLock);
    this->state = newState;
    spin_unlock(s_stateLock, state);
}

void Frame_UpdateLevel(Frame* this, size_t level) {
    uint32_t state = spin_lock_blocking(s_levelLock);
    this->level = level;
    spin_unlock(s_levelLock, state);
}

void Frame_UpdateSteps(Frame* this, size_t steps) {
    uint32_t state = spin_lock_blocking(s_stepsLock);
    this->steps = steps;
    spin_unlock(s_stepsLock, state);
}

/// @brief Deseneaza doar 
/// '''
/// Level: 
/// Steps:
/// ''',
/// fara a completa cu valorile din stateul jocului 
/// @param  
void Frame_DrawText(Frame* this) {
    uint16_t* buf = (uint16_t*)g_displayBuffer;

    draw_string(buf, 2, 2, "Level:", COLOR_TEXT);
    draw_string(buf, 2, 12, "Steps:", COLOR_TEXT);
}

void Frame_DrawLevel(Frame* this) {
   uint32_t state = spin_lock_blocking(s_levelLock);
    size_t level = this->level;
    spin_unlock(s_levelLock, state);

    uint16_t* buf = (uint16_t*)g_displayBuffer;

    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%u", (unsigned)level);

    draw_string(buf, 60, 2, tmp, COLOR_TEXT);
}

void Frame_DrawSteps(Frame* this) {
    uint32_t state = spin_lock_blocking(s_stepsLock);
    size_t steps = this->steps;
    spin_unlock(s_stepsLock, state);

    uint16_t* buf = (uint16_t*)g_displayBuffer;

    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%u", (unsigned)steps);

    draw_string(buf, 60, 12, tmp, COLOR_TEXT);

}

void Frame_DrawPigeon(Frame* this) {

    fill_bg(this->pigeonBuffer, COLOR_SKY_BG);

    // nori
    draw_circle(this->pigeonBuffer, 15, 10, 8, COLOR_CLOUD_WHITE);
    draw_circle(this->pigeonBuffer, 23, 10, 8, COLOR_CLOUD_WHITE);

    // corp
    draw_ellipse(
        this->pigeonBuffer,
        40,
        35,
        22,
        16,
        COLOR_PIGEON_BODY_LIGHT
    );

    // cap
    draw_circle(
        this->pigeonBuffer,
        58,
        23,
        10,
        COLOR_PIGEON_BODY_DARK
    );

    // gat
    draw_circle(
        this->pigeonBuffer,
        50,
        30,
        6,
        COLOR_PIGEON_NECK
    );
    

    // aripa
    switch (Frame_GetPigeonState(this)) {

        case PIGEON_IDLE:

            draw_ellipse(
                this->pigeonBuffer,
                35,
                35,
                14,
                8,
                COLOR_PIGEON_WING
            );

            break;

        case PIGEON_WINGS:

            draw_ellipse(
                this->pigeonBuffer,
                35,
                24,
                18,
                7,
                COLOR_PIGEON_WING
            );

            break;

        case PIGEON_EAT:

            draw_ellipse(
                this->pigeonBuffer,
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
        this->pigeonBuffer,
        61,
        21,
        2,
        COLOR_PIGEON_EYE_RING
    );

    draw_circle(
        this->pigeonBuffer,
        61,
        21,
        1,
        COLOR_EYE
    );

    // cioc
    draw_line(
        this->pigeonBuffer,
        67,
        24,
        75,
        26,
        COLOR_PIGEON_BEAK
    );

    draw_line(
        this->pigeonBuffer,
        67,
        25,
        75,
        27,
        COLOR_PIGEON_BEAK
    );

    // picioare
    draw_line(
        this->pigeonBuffer,
        35,
        49,
        33,
        57,
        COLOR_PIGEON_FEET
    );

    draw_line(
        this->pigeonBuffer,
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
        (uint8_t*)this->pigeonBuffer,
        sizeof(this->pigeonBuffer)
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
        gpio_put(DISPLAY_CS, 1); 
        return FAIL;
    }

    gpio_put(DISPLAY_CS, 1); 
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

    gpio_put(DISPLAY_CS, 0); 

    // CASET (Coloane)
    cmd = 0x2A;
    data[0] = x0 >> 8; data[1] = x0 & 0xFF; 
    data[2] = x1 >> 8; data[3] = x1 & 0xFF;
    gpio_put(DISPLAY_DC, 0); // Command mode
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1);
    gpio_put(DISPLAY_DC, 1); // Data mode
    spi_write_blocking(DISPLAY_SPI_PORT, data, 4);

    // RASET (Randuri)
    cmd = 0x2B;
    data[0] = y0 >> 8; data[1] = y0 & 0xFF; 
    data[2] = y1 >> 8; data[3] = y1 & 0xFF;
    gpio_put(DISPLAY_DC, 0);
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1);
    gpio_put(DISPLAY_DC, 1);
    spi_write_blocking(DISPLAY_SPI_PORT, data, 4);

    // RAMWR
    cmd = 0x2C;
    gpio_put(DISPLAY_DC, 0);
    spi_write_blocking(DISPLAY_SPI_PORT, &cmd, 1);

    gpio_put(DISPLAY_CS, 1); 

    return SUCCESS;
}
