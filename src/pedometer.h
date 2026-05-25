#ifndef _PS_PEDOMETER_H_
#define _PS_PEDOMETER_H_

#include "pigeon_stepper.h"


typedef struct Pedometer Pedometer;
// Tine state-ul pedometrului

void Pedometer_Init(Pedometer* this);
void Pedometer_AddStep(Pedometer* this);
void Pedometer_LevelUp(Pedometer* this);

int Pedometer_ShouldLevelUp(Pedometer* this);
size_t Pedometer_GetLevel(Pedometer* this);
size_t Pedometer_GetSteps(Pedometer* this);

#endif // _PS_PEDOMETER_H_
