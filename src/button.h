#ifndef _PS_BUTTON_H_
#define _PS_BUTTON_H_

#include "pico/types.h"

#include <stdint.h>
#include <stddef.h>

// Callbackurile
void Buttons_Init();

static void Button_Callback(uint gpio, uint32_t events);

#endif // _PS_BUTTON_H_