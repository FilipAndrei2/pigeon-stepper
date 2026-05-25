#include "pedometer.h"


struct Pedometer {
	size_t level;
	size_t steps;
	size_t reqSteps4LevelUp;
};

static void Pedometer_LevelUp(Pedometer* this);

void Pedometer_Init(Pedometer* this) {
	this->level = 0u;
	this->steps = 0u;
	this->reqSteps4LevelUp = 5u;
}

void Pedometer_AddStep(Pedometer* this) {
	this->steps += 1u;
}

size_t Pedometer_GetLevel(Pedometer* this) {
	return this->level;
}

size_t Pedometer_GetSteps(Pedometer* this) {
	return this->steps;
}

int Pedometer_ShouldLevelUp(Pedometer* this) {
	return this->steps >= this->reqSteps4LevelUp;
}

static void Pedometer_LevelUp(Pedometer* this) {
	this->steps = this->steps - this->reqSteps4LevelUp;
	this->level += 1;
	size_t tmp =  (size_t)(this->reqSteps4LevelUp * 1.25f);
	this->reqSteps4LevelUp = tmp < reqSteps4LevelUp ? reqSteps4LevelUp : tmp;
}
