/*
 * mpu6050.h
 *
 *  Created on: May 31, 2026
 *      Author: Anthony Y.
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

// Choose a microcontroller family
//#define STM32F1
#define STM32L4

// Include necessary Board HAL Libraries
#if defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#endif

/* MPU-6050 Defs */
#define MPU6050_ADDR_I2C (0x68 << 1)
#define MPU6050_PROFILER_THRESH 25

/* MPU-6050 Register Map Consts */

// I2C Registers
#define MPU6050_REG_I2C_MST_CTRL 0x24
#define MPU6050_REG_I2C_MST_STATUS 0x36

// Control / Power Management Registers
#define MPU6050_REG_USER_CTRL 0x6A
#define MPU6050_REG_PWR_MGMT_1 0x6B

// Accelerometer Registers
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_ACCEL_XOUT_L 0x3C
#define MPU6050_REG_ACCEL_YOUT_H 0x3D
#define MPU6050_REG_ACCEL_YOUT_L 0x3E
#define MPU6050_REG_ACCEL_ZOUT_H 0x3F
#define MPU6050_REG_ACCEL_ZOUT_L 0x40

// MPU-6050 Acceleration Variables Struct
typedef struct{
	int16_t accel_X_RAW, accel_Y_RAW, accel_Z_RAW;
} mpu6050_AccelStruct;

/* MPU-6050 Function Declarations*/

HAL_StatusTypeDef mpu6050_Wake(I2C_HandleTypeDef hi2cIn);
HAL_StatusTypeDef mpu6050_Sleep(I2C_HandleTypeDef hi2cIn);
HAL_StatusTypeDef mpu6050_MeasureAccel(I2C_HandleTypeDef hi2cIn, mpu6050_AccelStruct *mpuStructPtr);
#endif /* INC_MPU6050_H_ */
