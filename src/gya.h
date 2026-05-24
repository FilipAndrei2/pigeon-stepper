#ifndef _GYA_H_
#define _GYA_H_

#include "hardware/i2c.h"

#include "types.h"

// AD0 = GND => I2C_ADDRESS = 0X68
// AD0 - 3V3 => I2C_ADDRESS = 0X69 
#define GYA_I2C_ADDR (0X68)

#define GYA_I2C_PORT i2c1

// Registrele interne ale MPU6050
#define GYA_REG_PWR_MGMT_1   0x6B
#define GYA_REG_ACCEL_XOUT_H 0x3B // Primul registru de date

ExitCode_t GYA_Init();
ExitCode_t GYA_ReadRaw(int16_t accel[3], int16_t gyro[3], int16_t *temp);

#endif // _GYA_H_