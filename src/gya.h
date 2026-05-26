#ifndef _GYA_H_
#define _GYA_H_

#include "pigeon_stepper.h"
#include "hardware/i2c.h"

#include "types.h"

// AD0 = GND => I2C_ADDRESS = 0X68
// AD0 - 3V3 => I2C_ADDRESS = 0X69 
#define GYA_I2C_ADDR (0X68)

#define GYA_I2C_PORT i2c1

// Registrele interne ale MPU6050
#define GYA_REG_PWR_MGMT_1   0x6B
#define GYA_REG_ACCEL_XOUT_H 0x3B // Primul registru de date

#define MINIMUM_TIME_THRESHOLD 400
#define STATIC_THRESHOLD 0.12
#define MOVING_THRESHOLD 0.25 // Maybe 0.22
typedef struct {
    int16_t lastAccel[3], curAccel[3];
    int16_t lastGyro[3], curGyro[3];

    double lastMag, curMag;
    double baselineAcceleration;
    uint32_t lastStepTime;
} Stepper;

/// @returns number of steps detected
void Stepper_Init(Stepper* this); // Apeleaza-l dupa GYA_Init()
uint32_t Stepper_DetectStep(Stepper* this);

ExitCode_t GYA_Init();

// Foloseste functiile de mai jos doar pt test te rog.
ExitCode_t GYA_ReadRaw(int16_t accel[3], int16_t gyro[3], int16_t *temp);

#endif // _GYA_H_
