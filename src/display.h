#ifndef _PS_DISPLAY_H_
#define _PS_DISPLAY_H_

#include "pigeon_stepper.h"
#include "pinout.h"
#include "types.h"
#include "pico/stdlib.h"

// Display-ul 128 x 160
// Pigeon     80 x 60

#define DISPLAY_RESOLUTION_WIDTH            128U
#define DISPLAY_RESOLUTION_HEIGHT           160U
#define DISPLAY_RGB_SIZE                      2U

#define PIGEON_RESOLUTION_WIDTH             80U
#define PIGEON_RESOLUTION_HEIGHT            60U

// Interfata publica

enum  : uint16_t {
    COLOR_BG    = 0X10A2,
    COLOR_TEXT  = 0xEF7D,

    COLOR_PIGEON_BODY_LIGHT = 0XC638,
    COLOR_PIGEON_BODY_DARK  = 0X7BEF,
    COLOR_PIGEON_WING       = 0X5AEB,
    COLOR_PIGEON_NECK       = 0X03EF,
    COLOR_PIGEON_BEAK       = 0XFD20,
    COLOR_EYE               = 0X0000,
    COLOR_PIGEON_EYE_RING   = 0XFFFF,
    COLOR_PIGEON_FEET       = 0XF9A8,
    COLOR_SKY_BG            = 0XAEDC,
    COLOR_CLOUD_WHITE       = 0XFFFF     
} Colors;

typedef enum : uint8_t {
    PIGEON_IDLE = 0,
    PIGEON_WINGS,
    PIGEON_EAT
} PigeonStates;

typedef struct {
    PigeonStates state;
    size_t level;
    size_t steps;

    uint16_t backgroundColor;
} Frame;

void Frame_Init(Frame * this, size_t level, size_t steps);
void Frame_UpdatePigeon(Frame* this, PigeonState newState);
void Frame_UpdateUi(Frame* this, size_t level, size_t steps);
void Frame_DrawPigeon(Frame * this);
void Frame_DrawLevel(Frame* this);
void Frame_DrawSteps(Frame* this)

// Nu folosi astea decat pentru test
// TODO: Sterge-le pe astea din header ca sa nu fie accesibile din exterior
ExitCode_t Display_SendBuffer(const uint8_t* buffer, size_t len);
ExitCode_t Display_SendCmd(uint8_t cmd);
ExitCode_t Display_SendData(uint8_t data);
ExitCode_t Display_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);


static inline void display_begin() {
    gpio_put(DISPLAY_CS, 0);
}

static inline void display_end() {
    gpio_put(DISPLAY_CS, 1);
}

ExitCode_t Display_WritePixels(const uint8_t* data, size_t len);

// TODO: Bufferul de mai jos e nefolosit in logica actuala, dar daca il scot nu mai compileaza acum. Sterge-l din toate partile!
extern uint8_t g_displayBuffer[DISPLAY_RESOLUTION_WIDTH * DISPLAY_RESOLUTION_HEIGHT * DISPLAY_RGB_SIZE];

#define DISPLAY_SPI_PORT spi1


#endif // _PS_DISPLAY_H_
