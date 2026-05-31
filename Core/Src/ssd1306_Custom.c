/*
 * ssd1036_Custom.c
 *
 *  Created on: May 22, 2026
 *      Author: tonym
 */


#include <math.h>
#include <ssd1306_Custom.h>
#include <stdlib.h>
#include <string.h> // For memcpy

// Hardware Function Defs
void ssd1306_Reset(void){
	/* for I2C - do nothing */
}

void ssd1306_Send_Cmd(uint8_t inByte, I2C_HandleTypeDef hi2cIn){
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &inByte, 1, HAL_MAX_DELAY);
//	HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)
}

void ssd1306_Send_Data_Buf(uint8_t *bufferBytes, size_t bufferSize, I2C_HandleTypeDef hi2cIn){
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x40, 1, bufferBytes, bufferSize, HAL_MAX_DELAY);
}

void ssd1306_Display_Set(const uint8_t inByte, I2C_HandleTypeDef hi2cIn){
	switch(inByte){
			case 0:
				// In this case, set the Display to OFF
				uint8_t ssd1306_Cmd_Display_Off = 0xAE;
				HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &ssd1306_Cmd_Display_Off, 1, HAL_MAX_DELAY);
				break;
			default:
				uint8_t ssd1306_Cmd_Display_On = 0xAF;
				HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &ssd1306_Cmd_Display_On, 1, HAL_MAX_DELAY);
				break;
	}
}

void ssd1306_Set_Contrast_Control(uint8_t inByte, I2C_HandleTypeDef hi2cIn){
	uint8_t ssd1306_Cmd_Contrast = 0x81;
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &ssd1306_Cmd_Contrast, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &inByte, 1, HAL_MAX_DELAY);
}

// Procedure Function Definitions

void ssd1306_Init(I2C_HandleTypeDef hi2cIn){
	// Initialization Constants Definitions
//	uint8_t ssd1306_Cmd_Mux_1 = 0xA8;
//	uint8_t ssd1306_Cmd_Mux_2 = 0x3F;
//	uint8_t ssd1306_Cmd_Display_Offset_1 = 0xD3;
//	uint8_t ssd1306_Cmd_Display_Offset_2 = 0x00;
//	uint8_t ssd1306_Cmd_Display_Start_Line = 0x40;
//	uint8_t ssd1306_Cmd_Display_Segment_Remap_1 = 0xA0;
//	uint8_t ssd1306_Cmd_Display_Segment_Remap_2 = 0xA1;

	ssd1306_Send_Cmd(0xA8, hi2cIn); // Set MUX ratio
	ssd1306_Send_Cmd(0x3F, hi2cIn);

	ssd1306_Send_Cmd(0xD3, hi2cIn); // Cmd Display Offset 1
	ssd1306_Send_Cmd(0x00, hi2cIn); // Cmd Display Offset 2

	ssd1306_Send_Cmd(0x40, hi2cIn); // Cmd Display Start Line

	ssd1306_Send_Cmd(0xA0, hi2cIn); // Cmd Display Segment Remap 1
	ssd1306_Send_Cmd(0xA1, hi2cIn); // Cmd Display Segment Remap 2

	ssd1306_Send_Cmd(0xC0, hi2cIn); // Set COM Output Scan Direction
	ssd1306_Send_Cmd(0xC8, hi2cIn);

	ssd1306_Send_Cmd(0xDA, hi2cIn); // Set COM Pins Hardware Configuration
	ssd1306_Send_Cmd(02, hi2cIn);

	ssd1306_Send_Cmd(0x81, hi2cIn); // Set Contrast Control
	ssd1306_Send_Cmd(0x7F, hi2cIn);

	ssd1306_Send_Cmd(0xA4, hi2cIn); // Disable Entire Display On

	ssd1306_Send_Cmd(0xA6, hi2cIn); // Set Normal Display

	ssd1306_Send_Cmd(0xD5, hi2cIn); // Set Oscillation Freq
	ssd1306_Send_Cmd(0x80, hi2cIn);

	ssd1306_Send_Cmd(0x8D, hi2cIn); // Enable Charge Pump Regulator
	ssd1306_Send_Cmd(0x14, hi2cIn);

	ssd1306_Send_Cmd(0xAF, hi2cIn); // Finally, Turn Display On
}

//void ssd1306_UpdateScreen

	// Continue to write initialization commands

