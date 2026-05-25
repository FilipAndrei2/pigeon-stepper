#ifndef _PS_BUTTON_H_
#define _PS_BUTTON_H_

#include "pico/types.h"

#include <stdint.h>
#include <stddef.h>

// Callbackurile
void Buttons_Init();

void Button1_CallBack(uint gpio, uint32_t events);
void Button2_CallBack(uint gpio, uint32_t events);
void Button3_CallBack(uint gpio, uint32_t events);

#endif // _PS_BUTTON_H_