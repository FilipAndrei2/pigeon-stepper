#ifndef _PS_PEDOMETER_H_
#define _PS_PEDOMETER_H_

#include "pigeon_stepper.h"


// Tine state-ul pedometrului
typedef 
struct  {
	size_t level;
	size_t steps;
	size_t reqSteps4LevelUp;
} Pedometer;


void Pedometer_Init(Pedometer* this);
void Pedometer_AddStep(Pedometer* this);
void Pedometer_LevelUp(Pedometer* this);

int Pedometer_ShouldLevelUp(Pedometer* this);
size_t Pedometer_GetLevel(Pedometer* this);
size_t Pedometer_GetSteps(Pedometer* this);

#endif // _PS_PEDOMETER_H_
