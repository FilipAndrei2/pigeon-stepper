#ifndef _GYRO_ACCEL_H_
#define _GYRO_ACCEL_H_

#include "hardware/i2c.h"

// AD0 = GND => I2C_ADDRESS = 0X68
// AD0 - 3V3 => I2C_ADDRESS = 0X69 
#define GYA_I2C_ADDR (0X68)

#define GYA_I2C_PORT i2c1

// Registrele interne ale MPU6050
#define GYA_REG_PWR_MGMT_1   0x6B
#define GYA_REG_ACCEL_XOUT_H 0x3B // Primul registru de date

/// @brief 
/// @param accel 
/// @param gyro 
/// @param temp poate sa fie NULL safely (nu se va intampla nimic)
/// @return 
static ExitCode_t gyaReadRaw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    uint8_t buffer[14];
    uint8_t reg_start = GYA_REG_ACCEL_XOUT_H;

    // Pasul A: Îi spunem senzorului de la ce registru vrem să începem citirea.
    // Setăm 'true' la final (nostop) pentru a ține magistrala I2C activă!
    i2c_write_blocking(GYA_I2C_PORT, GYA_I2C_ADDR, &reg_start, 1, true);

    // Pasul B: Citim 14 octeți consecutiv (Accel X,Y,Z + Temp + Gyro X,Y,Z = 7 valori * 2 bytes)
    // Setăm 'false' la final pentru a elibera magistrala după ce am terminat.
    int ret = i2c_read_blocking(GYA_I2C_PORT, GYA_I2C_ADDR, buffer, 14, false);
    
    if (ret == PICO_ERROR_GENERIC) {
         return FAIL;
    }

    // Pasul C: Reconstruim valorile pe 16 biți (High Byte shiftat cu 8 + Low Byte)
    accel[0] = (buffer[0] << 8) | buffer[1];  // Accel X
    accel[1] = (buffer[2] << 8) | buffer[3];  // Accel Y
    accel[2] = (buffer[4] << 8) | buffer[5];  // Accel Z

    if (temp) {
        *temp    = (buffer[6] << 8) | buffer[7];  // Temperatura
    }

    gyro[0]  = (buffer[8] << 8) | buffer[9];  // Gyro X
    gyro[1]  = (buffer[10] << 8) | buffer[11]; // Gyro Y
    gyro[2]  = (buffer[12] << 8) | buffer[13]; // Gyro Z

    return SUCCESS;
}

static ExitCode_t initGya() {
    uint8_t buffer[2];
    buffer[0] = GYA_REG_PWR_MGMT_1; // Adresa registrului de power
    buffer[1] = 0x00;               // 0x00 scoate senzorul din Sleep Mode

    // Scriem pe I2C
    int ret = i2c_write_blocking(GYA_I2C_PORT, GYA_I2C_ADDR, buffer, 2, false);
    
    if (ret == PICO_ERROR_GENERIC) {
        LOG("Eroare: MPU6050 nu a raspuns la trezire.\n");
        return FAIL;
    }

    return SUCCESS;
}

#endif // _GYRO_ACCEL_H_