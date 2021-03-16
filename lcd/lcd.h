#include "pico/stdio.h"

#define LCD_CS		13
#define LCD_DC		15
#define LCD_RES		16

#define USE_HORIZONTAL 0

#define BRRED 0XFC07

void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void Lcd_Init(void);
