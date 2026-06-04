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
#include <assert.h> // For assert

// Variable Defs:
static uint8_t ssd1306_Screen_Buff_Arr_Inst[SSD1306_BUFFER_SIZE];
static SSD1306_Transform ssd1306_Transform_Inst;

// Hardware Function Defs
void ssd1306_Reset(void){
	/* for I2C - do nothing */
}

void ssd1306_Send_Cmd(uint8_t inByte, I2C_HandleTypeDef hi2cIn){
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &inByte, 1, HAL_MAX_DELAY);
}

void ssd1306_Send_Data_Buff(uint8_t *bufferBytes, size_t bufferSize, I2C_HandleTypeDef hi2cIn){
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x40, 1, bufferBytes, bufferSize, HAL_MAX_DELAY);
}

void ssd1306_SetScreen(const uint8_t inByte, I2C_HandleTypeDef hi2cIn){
	switch(inByte){
			case 0:
				// In this case, set the Display to OFF
				uint8_t ssd1306_Cmd_Display_Off = 0xAE;
				HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &ssd1306_Cmd_Display_Off, 1, HAL_MAX_DELAY);
				ssd1306_Transform_Inst.DisplayOn = 0;
				break;
			default:
				uint8_t ssd1306_Cmd_Display_On = 0xAF;
				HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &ssd1306_Cmd_Display_On, 1, HAL_MAX_DELAY);
				ssd1306_Transform_Inst.DisplayOn = 1;
				break;
	}
}

void ssd1306_Set_Contrast_Control(uint8_t inByte, I2C_HandleTypeDef hi2cIn){
	uint8_t ssd1306_Cmd_Contrast = 0x81;
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &ssd1306_Cmd_Contrast, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2cIn, SSD1306_I2C_ADDR, 0x00, 1, &inByte, 1, HAL_MAX_DELAY);
}


// Procedure Function Definitions
SSD1306_Error_State ssd1306_UpdateBuffer(uint8_t* buffer, uint8_t bufferLen){
	SSD1306_Error_State funcReturn = SSD1306_ERR;
	// Copy the input argument into the SSD1306 buffer used by the functions
	if(bufferLen <= SSD1306_BUFFER_SIZE){
		memcpy(ssd1306_Screen_Buff_Arr_Inst, buffer, bufferLen);
		funcReturn = SSD1306_OK;
	}
	return funcReturn;
}

void ssd1306_ClearBuffer(void){
	// Overwrite the screen buffer to only be Black Pixels, then update the screen
	// To overwrite memory, use `memset`
	memset(ssd1306_Screen_Buff_Arr_Inst, 0x00, sizeof(ssd1306_Screen_Buff_Arr_Inst));
}
void ssd1306_FillBuffer(void){
	memset(ssd1306_Screen_Buff_Arr_Inst, 0xFF, sizeof(ssd1306_Screen_Buff_Arr_Inst));
}

void ssd1306_UpdateScreen(I2C_HandleTypeDef hi2cIn){
	// Write Data to each page of RAM
	for (uint8_t i = 0; i < SSD1306_HEIGHT / 8; i++){
		ssd1306_Send_Cmd(0xB0 + i, hi2cIn);
		ssd1306_Send_Cmd(0x00, hi2cIn); // Set Lower Column Starting Address for Page Address Mode
		ssd1306_Send_Cmd(0x10, hi2cIn); // Set Upper Column Starting Address for Page Address Mode
		// Send a Page of data from the library display buffer
		ssd1306_Send_Data_Buff(&ssd1306_Screen_Buff_Arr_Inst[SSD1306_WIDTH*i], SSD1306_WIDTH, hi2cIn);
	}
}

void ssd1306_Init(I2C_HandleTypeDef hi2cIn){
	// Reset the OLED Display and turn off the diplay
	ssd1306_Reset();
	HAL_Delay(100);
	ssd1306_SetScreen(0, hi2cIn); // 0 means "off"

	// Set Memory Addressing Mode
	ssd1306_Send_Cmd(0x20, hi2cIn); // Set Memory Addressing Mode
	ssd1306_Send_Cmd(0x00, hi2cIn); // "0x00" Chooses to have Horizontal Addressing Mode
//	ssd1306_Send_Cmd(0x01, hi2cIn); // "0x00" Chooses to have Page Addressing Mode

	// Change COM Scan direction for Screen (Mirroring)
	// Uncomment the setting you want to use
	ssd1306_Send_Cmd(0xC0, hi2cIn); // Scan COM pins in a mirrored way (vertically)
//	ssd1306_Send_Cmd(0xC8, hi2cIn); // Scan COM pins normally

	// Define Sequential Addressing Low and High Column Addresses
	ssd1306_Send_Cmd(0xB0, hi2cIn); // Set the Lower Start Column Address (0-7) for Page Addressing Mode
	ssd1306_Send_Cmd(0x00, hi2cIn);
	ssd1306_Send_Cmd(0x10, hi2cIn);

	// Sets Display Start Line. With 0x40, there is no offset, meaning the value at the 0th row of RAM is mapped to COM0, and so on
	ssd1306_Send_Cmd(0x40, hi2cIn);

	// Set Segment Re-map
	ssd1306_Send_Cmd(0xA0, hi2cIn); // Maps Column Address 0 to SEG0 (RESET)

	// Set Color Inversion
	// Uncomment the setting you want to use
//	ssd1306_Send_Cmd(0xA7, hi2cIn); // Uses Inverted Color
	ssd1306_Send_Cmd(0xA6, hi2cIn); // Uses Normal Color

	ssd1306_Send_Cmd(0xA8, hi2cIn); // Set MUX ratio
	ssd1306_Send_Cmd(0x3F, hi2cIn); // Set to 64MUX

	ssd1306_Send_Cmd(0xA4, hi2cIn); // "Enable Display Outputs according to the GGDRAM Contents" (pg 37 from datasheet)
//	ssd1306_Send_Cmd(0xA5, hi2cIn); // Forces the Display to be "ON", regardless of the GGDRAM content

	ssd1306_Send_Cmd(0xD3, hi2cIn); // "Reset" the display offset to have zero offset in either the horizontal or vertical direction
	ssd1306_Send_Cmd(0x00, hi2cIn);

	ssd1306_Send_Cmd(0xD5, hi2cIn); // Set Oscillation Freq
	ssd1306_Send_Cmd(0xF0, hi2cIn);

	ssd1306_Send_Cmd(0xD9, hi2cIn); // Set the SSD1306's Pre-Charge Period
	ssd1306_Send_Cmd(0x22, hi2cIn);

	ssd1306_Send_Cmd(0xDA, hi2cIn); // Set COM Pins Hardware Configuration
	// Configuration settings are within Table 9-1, pg 31 of the datasheet
	ssd1306_Send_Cmd(0x12, hi2cIn); // Alternative COM pin config and Disabled COM Left/Right remap
									// ^^^ Here, the in the sent Byte of [0 0 A5 A4 0 0 1 0], A5 = 0 and A4 = 1
									// This Configuration corresponds with Diagram 7 on pg 42
	ssd1306_Send_Cmd(0xDB, hi2cIn); // Set the VCOMH Pin Deselect Voltage Level
	ssd1306_Send_Cmd(0x20, hi2cIn);

	ssd1306_Send_Cmd(0x8D, hi2cIn); // Enable Charge Pump Regulator (DC-DC Enable)
	ssd1306_Send_Cmd(0x14, hi2cIn);

	// Turn Display On
	ssd1306_SetScreen(1, hi2cIn);

	// Clear Display
	ssd1306_ClearBuffer();

	// Update Screen with Buffer
	ssd1306_UpdateScreen(hi2cIn);

	// Update Library Variables
	ssd1306_Transform_Inst.Initialized = 1;
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR inColor){
	// Buffer Check
	if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
		return;

	if(inColor == White){
		// Edit Pixel in the screen's buffer
		ssd1306_Screen_Buff_Arr_Inst[x + SSD1306_WIDTH*(y / 8)] |= 1 << (y % 8); // This line works because the fractional part of `y` is dropped via truncation
	}
	else{
		ssd1306_Screen_Buff_Arr_Inst[x + SSD1306_WIDTH*(y / 8)] &= ~(1 << (y % 8));
	}
}

// TODO: Draw Bitmap Definition
//void ssd1306
void ssd1306_SetFontCursor(uint8_t x, uint8_t y){
	// Set ssd1306 struct instance cursor placement
	ssd1306_Transform_Inst.CurrentX = x;
	ssd1306_Transform_Inst.CurrentY = y;
}

char ssd1306_WriteChar(char ch, SSD1306_Font fontType){
	// Function Pre-Condition Asserts
	// Todo: Rewrite the checks below using asserts()
	// First check: Make sure the character is valid
	if(ch < 32 || ch > 126)
		return 0;
	// Second check: Make sure the character will be within the necessary space
	if(SSD1306_WIDTH < (ssd1306_Transform_Inst.CurrentX + fontType.width))
		return 0;
	if(SSD1306_HEIGHT < (ssd1306_Transform_Inst.CurrentY + fontType.height))
		return 0;

	// Since I'm not planning on using a proportional font, I will default to the pre-defined width for each character
	const uint8_t charWidth = fontType.width;

	// Todo: Look into this method later to see if it can still work
	// First, search through the font data array for `ch`
	//	uint16_t charArr [charHeight];
	//	memcpy(&charArr, &fontType.data[(ch - 32) * charHeight], charHeight);

		// Second, write the character at the cursor spot in the buffer
	//	for(uint8_t i = 0; i < charWidth; i++){
	//		ssd1306_Screen_Buff_Arr_Inst[ssd1306_Transform_Inst.CurrentX + ssd1306_Transform_Inst.CurrentY * (8 + i)] = charArr[i];
	//	}


//	 Use Repo's method for drawing a  character pixel by pixel
    for(uint32_t i = 0; i < fontType.height; i++) {
    	uint32_t fontCharIndex = ((ch - 32) * fontType.height) + i;
        for(uint32_t j = 0; j < charWidth; j++) {
            if((fontType.data[fontCharIndex] << j) & 0x8000)  {
                ssd1306_DrawPixel(ssd1306_Transform_Inst.CurrentX + j, (ssd1306_Transform_Inst.CurrentY + i), White);
            }
            else {
                ssd1306_DrawPixel(ssd1306_Transform_Inst.CurrentX + j, (ssd1306_Transform_Inst.CurrentY + i), Black);
            }
        }
    }

	// Finally, move the cursor the same amount as the char width
	ssd1306_Transform_Inst.CurrentX += charWidth;
	return ch;
}

char ssd1306_WriteString(char* str, SSD1306_Font fontType){
	uint32_t i = 0;
	char strCh = *str; // Grab first character in the string
	while(strCh != '\0'){
		ssd1306_WriteChar(strCh, fontType);
		strCh = str[++i];
	}
	return strCh; // Should return null char "\0"
}


