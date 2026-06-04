/*
 * ssd1036_Custom.h
 *
 *  Created on: May 16, 2026
 *      Author: tonym
 */

#ifndef __SSD1306_CUSTOM_H__
#define __SSD1306_CUSTOM_H__

/*************************
 * This header file will take inspiration from the stm32-sd1306 Github Repo header files.
 * The project's license is included in the LICENSE folder.
 **************************/

// Header File Configuration

// Choose a microcontroller family
//#define STM32F1
#define STM32L4


// I2C Config
//#define SSD1306_I2C_ADDR (0x7B << 1)
//#define SSD1306_I2C_ADDR (0x78 << 1)
#define SSD1306_I2C_ADDR (0x3C << 1)
#define SSD1306_I2C_PORT hi2c1 // Change this for difference I2C ports on the STM32 boards

// SSD1306 Font Selection
#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_11x18
//#define SSD1306_INCLUDE_FONT_16x26
//#define SSD1306_INCLUDE_FONT_16x24
//#define SSD1306_INCLUDE_FONT_16x15


// Include C Library Definitions
#include <stddef.h>
#include <stdint.h>
#include <_ansi.h>

// Include Microcontroller respective HAL library
#if defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#else
#error "SSD1306 (Custom) only supports the STM32F1 and STM32L4 series. Please use the official repo for more board-series support!"
#endif

// Define Helpful Macros / Structs for Custom Library Functions

// SSD1306 OLED Screen Dimensions (For The OLED I bought)
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT 64
#endif

#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH 128
#endif

#ifndef SSD1306_BUFFER_SIZE
#define SSD1306_BUFFER_SIZE SSD1306_WIDTH * SSD1306_HEIGHT / 8
#endif


// Enum for the OLED Color
typedef enum{
	Black = 0x00,
	White = 0x01
}	SSD1306_COLOR;

// For defining some function's return values
typedef enum{
	SSD1306_OK = 0x00,
	SSD1306_ERR = 0x01 // Error State
} SSD1306_Error_State;

// Struct for Display Transformations
typedef struct{
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Initialized;
	uint8_t DisplayOn;
} SSD1306_Transform;

// Struct for Font Config
typedef struct{
	const uint8_t width;
	const uint8_t height;
	const uint16_t *const data;
	const uint8_t *const char_width;
} SSD1306_Font;


// Function Declarations

// Hardware Function Declarations
void ssd1306_Reset(void);
void ssd1306_Send_Cmd(uint8_t inByte, I2C_HandleTypeDef hi2cIn);
void ssd1306_Send_Data_Buff(uint8_t *bufferBytes, size_t bufferSize, I2C_HandleTypeDef hi2cIn);

// "Fundamental Commands" from SSD1306 datasheet

// Parameters: inByte = 0 --> OFF, inByte != 0 --> ON
void ssd1306_SetScreen(const uint8_t inByte, I2C_HandleTypeDef hi2cIn); // For sending the "Set Display OFF" Command

// Parameters: inByte: Contrast Value (Higher Value means Higher Contrast)
void ssd1306_Set_Contrast_Control(uint8_t inByte, I2C_HandleTypeDef hi2cIn); // Will use the lowest 7 bits to determine contrast

// Procedure Function Declarations from repo
void ssd1306_Init(I2C_HandleTypeDef hi2cIn);
// Zeros all data from the Screen Buffer
void ssd1306_ClearBuffer(void);
// Fills all data in the Screen Buffer with max value
void ssd1306_FillBuffer(void);
SSD1306_Error_State ssd1306_UpdateBuffer(uint8_t* buffer, uint8_t bufferLen);
void ssd1306_UpdateScreen(I2C_HandleTypeDef hi2cIn);

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR inColor);
void ssd1306_DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmapArray, uint8_t w, uint8_t h, SSD1306_COLOR inColor);

void ssd1306_SetFontCursor(uint8_t x, uint8_t y);
char ssd1306_WriteChar(char ch, SSD1306_Font fontType);
char ssd1306_WriteString(char* str, SSD1306_Font fontType);

#endif /* __SSD1306_CUSTOM_H__*/
