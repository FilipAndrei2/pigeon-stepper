#ifndef _GYRO_ACCEL_H_
#define _GYRO_ACCEL_H_

#include "hardware/i2c.h"

// AD0 = GND => I2C_ADDRESS = 0X68
// AD0 - 3V3 => I2C_ADDRESS = 0X69 
#define GYA_I2C_ADDR (0X68)

#define GYA_I2C_PORT i2c1

#endif // _GYRO_ACCEL_H_