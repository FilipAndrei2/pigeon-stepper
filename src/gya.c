#include "pigeon_stepper.h"
#include "gya.h"

#include "utils.h"
#include <math.h>



static double calculateMagnitude(Stepper* this) {
    return sqrt(this->curAccel[0] * this->curAccel[0] + this->curAccel[1] * this->curAccel[1] + this->curAccel[2] * this->curAccel[2]);
}

static int shouldStep(Stepper* this) {

// NOTE: Mai creste thresholdul asta daca detecteaza prea multi pasi
#define THRESHOLD 300 
    return this->curMag > THRESHOLD &&
        this->lastMag <= THRESHOLD &&
        to_ms_since_boot(get_absolute_time()) - this->lastStepTime >= 300; 
#undef THRESHOLD
}

static void nextRead(Stepper* this) {
    this->lastMag = this->curMag;

    for (int i = 0; i < 3; ++i) {
        this->lastAccel[i] = this->curAccel[i];
        this->lastGyro[i]  = this->curGyro[i];
    }

    GYA_ReadRaw(this->curAccel, this->curGyro, NULL);

    this->curMag = calculateMagnitude(this);
}

void Stepper_Init(Stepper* this) {
    int16_t accel[3], gyro[3];
    GYA_ReadRaw(accel, gyro, NULL);

    for (int i = 0; i < 3; ++i) {
        this->lastAccel[i] = accel[i];
        this->curAccel[i] = accel[i];

        this->lastGyro[i] = gyro[i];
        this->curGyro[i] = gyro[i];
    }

    this->lastMag = calculateMagnitude(this);
    this->curMag = this->lastMag;
    this->lastStepTime = to_ms_since_boot(get_absolute_time());
}

uint32_t Stepper_DetectStep(Stepper* this) {
    // realist nu poate sa detecteze decat 1 pas.
    nextRead(this);
    if (shouldStep(this)) {
        this->lastStepTime = to_ms_since_boot(get_absolute_time());
        return 1;
    }
    return 0;
}

/// @brief 
/// @param accel 
/// @param gyro 
/// @param temp poate sa fie NULL safely (nu se va intampla nimic)
/// @return 
ExitCode_t GYA_ReadRaw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
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

ExitCode_t GYA_Init() {
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
