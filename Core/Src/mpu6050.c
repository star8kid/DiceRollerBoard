/*
 * mpu6050.c
 *
 *  Created on: Jun 1, 2026
 *      Author: tonym
 */

#include <stdint.h>
#include <mpu6050.h>

// To-dos:
// Update all function defs / declarations to use pointer arguments instead of value

/* MPU-6050 Function Definitions */
HAL_StatusTypeDef mpu6050_Wake(I2C_HandleTypeDef hi2cIn){
	// To wake the device, write a 0 to the SLEEP bit field in the PWR_MGMT_1 register in the memory map
	uint8_t dataW = 0x00;
	// Write to the register and return the state of the I2C Communication
	// Here, `I2C_MEMADD_SIZE_8BIT` means the total size of the memory space of the register
	// Blocking Mode
	return HAL_I2C_Mem_Write(&hi2cIn, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &dataW, 1, HAL_MAX_DELAY);
	// Use Master TX Function instead
//	return HAL_I2C_Master_Transmit(hi2c, DevAddress, pData, Size, Timeout)
//	return HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, &dataW, 1, HAL_MAX_DELAY);
	// Non-Blocking (Interrupt) Mode
//	return HAL_I2C_Mem_Write_IT(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, 1, &dataW, 1);
}

HAL_StatusTypeDef mpu6050_Sleep(I2C_HandleTypeDef hi2cIn){
	// To sleep the device, write a 1 to the SLEEP bit field in the PWR_MGMT_1 register in the memory map
	uint8_t dataW = 0x70;
	return HAL_I2C_Mem_Write(&hi2cIn, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &dataW, 1, HAL_MAX_DELAY);
//	return HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, &dataW, 1, HAL_MAX_DELAY);
}


HAL_StatusTypeDef mpu6050_MeasureAccel(I2C_HandleTypeDef hi2cIn, mpu6050_AccelStruct *mpuStructPtr){
	// To receive data, create a 6-Byte buffer to hold it
	uint8_t dataBuff[6];
	HAL_StatusTypeDef status;

	// Next, write to the first acceleration register
	status = HAL_I2C_Mem_Read(&hi2cIn, MPU6050_ADDR_I2C, MPU6050_REG_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, dataBuff, 6, HAL_MAX_DELAY);
//	status = HAL_I2C_Master_Receive(hi2c, DevAddress, pData, Size, Timeout);
//	status = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR_I2C, pData, Size, Timeout);
	// Good Practice: If the status of the comms is not okay, return status
	if(status != HAL_OK) return status;

	// Next, redefine the values in the acceleration struct
	mpuStructPtr->accel_X_RAW = (int16_t)((dataBuff[0] << 8) | dataBuff[1]);
	mpuStructPtr->accel_Y_RAW = (int16_t)((dataBuff[2] << 8) | dataBuff[3]);
	mpuStructPtr->accel_Z_RAW = (int16_t)((dataBuff[4] << 8) | dataBuff[5]);

	return HAL_OK;
}
