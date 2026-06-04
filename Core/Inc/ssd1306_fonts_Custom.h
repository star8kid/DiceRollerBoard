/*
 * ssd1306_fonts_Custom.h
 *
 *  Created on: Jun 2, 2026
 *      Author: tonym
 */

#ifndef INC_SSD1306_FONTS_CUSTOM_H_
#define INC_SSD1306_FONTS_CUSTOM_H_

#include "ssd1306_Custom.h"

#ifdef SSD1306_INCLUDE_FONT_6x8
extern const SSD1306_Font Font_6x8;
#endif
#ifdef SSD1306_INCLUDE_FONT_7x10
extern const SSD1306_Font Font_7x10;
#endif
#ifdef SSD1306_INCLUDE_FONT_11x18
extern const SSD1306_Font Font_11x18;
#endif

#endif /* INC_SSD1306_FONTS_CUSTOM_H_ */
