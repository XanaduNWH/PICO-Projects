#include <stdio.h>
#include <string.h>
#include "common.h"
#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "lcd.h"

void LCD_Writ_Bus(uint8_t dat)
{
	gpio_put(LCD_CS, 0);
	spi_write_blocking(SPI_PORT, &dat, 1);
	gpio_put(LCD_CS, 1);
}

void LCD_WR_REG(uint8_t dat)
{
	gpio_put(LCD_DC, 0);
	LCD_Writ_Bus(dat);
	gpio_put(LCD_DC, 1);
}

void LCD_WR_DATA8(uint8_t dat)
{
	LCD_Writ_Bus(dat);
}

void LCD_WR_DATA(uint16_t dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}

/*
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA(x1+2);
	LCD_WR_DATA(x2+2);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA(y1+1);
	LCD_WR_DATA(y2+2);
	LCD_WR_REG(0x2c);//储存器写
}
*/

void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+80);
		LCD_WR_DATA(y2+80);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+80);
		LCD_WR_DATA(x2+80);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
}

/*
void Lcd_Init(void)
{
	gpio_put(LCD_RES, 0);
	sleep_ms(100);
	gpio_put(LCD_RES, 1);
	sleep_ms(100);
	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	LCD_WR_REG(0x11);//Sleep exit 
	sleep_ms(120);
		
	//ST7735R Frame Rate
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x2D); 

	LCD_WR_REG(0xB2); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x2D); 

	LCD_WR_REG(0xB3); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x2D); 
	
	LCD_WR_REG(0xB4); //Column inversion 
	LCD_WR_DATA8(0x07); 
	
	//ST7735R Power Sequence
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA8(0xA2); 
	LCD_WR_DATA8(0x02); 
	LCD_WR_DATA8(0x84); 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA8(0xC5); 

	LCD_WR_REG(0xC2); 
	LCD_WR_DATA8(0x0A); 
	LCD_WR_DATA8(0x00); 

	LCD_WR_REG(0xC3); 
	LCD_WR_DATA8(0x8A);
	LCD_WR_DATA8(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8A);
	LCD_WR_DATA8(0xEE);

	LCD_WR_REG(0xC5); //VCOM
	LCD_WR_DATA8(0x0E);

	LCD_WR_REG(0x36); //MX, MY, RGB mode
	LCD_WR_DATA8(0x08);
	
	//ST7735R Gamma Sequence
	LCD_WR_REG(0xe0); 
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x1a); 
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x18); 
	LCD_WR_DATA8(0x2f); 
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x20); 
	LCD_WR_DATA8(0x22); 
	LCD_WR_DATA8(0x1f); 
	LCD_WR_DATA8(0x1b); 
	LCD_WR_DATA8(0x23); 
	LCD_WR_DATA8(0x37); 
	LCD_WR_DATA8(0x00); 	
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x02); 
	LCD_WR_DATA8(0x10); 

	LCD_WR_REG(0xe1); 
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x1b); 
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x17); 
	LCD_WR_DATA8(0x33); 
	LCD_WR_DATA8(0x2c); 
	LCD_WR_DATA8(0x29); 
	LCD_WR_DATA8(0x2e); 
	LCD_WR_DATA8(0x30); 
	LCD_WR_DATA8(0x30); 
	LCD_WR_DATA8(0x39); 
	LCD_WR_DATA8(0x3f); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0x10);  
	
	LCD_WR_REG(0x2a);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00+2);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x80+2);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00+3);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x80+3);
	
	LCD_WR_REG(0xF0); //Enable test command  
	LCD_WR_DATA8(0x01); 
	LCD_WR_REG(0xF6); //Disable ram power save mode 
	LCD_WR_DATA8(0x00); 
	
	LCD_WR_REG(0x3A); //65k mode 
	LCD_WR_DATA8(0x05); 
	
	
	LCD_WR_REG(0x29);//Display on

}
*/

/*
void Lcd_Init(void)
{
	gpio_put(LCD_RES, 0);
	sleep_ms(100);
	gpio_put(LCD_RES, 1);
	sleep_ms(100);
	// Start Initial Sequence
	LCD_WR_REG(0x11); //Sleep out
	sleep_ms(120);              //Delay 120ms
	// Start Initial Sequence
	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xB7);
	LCD_WR_DATA8(0x35);

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x19);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x12);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20);

	LCD_WR_REG(0xC6);
	LCD_WR_DATA8(0x0F);

	LCD_WR_REG(0xD0);
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2B);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x4C);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x23);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x51);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x23);
	LCD_WR_REG(0x21);

	LCD_WR_REG(0x29);
}
*/

void Lcd_Init(void)
{
	gpio_put(LCD_RES, 0);
	sleep_ms(100);
	gpio_put(LCD_RES, 1);
	sleep_ms(100);
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); //Sleep out
	sleep_ms(120);              //Delay 120ms
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0xCF);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xC1);
	LCD_WR_DATA8(0X30);
	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x64);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0X12);
	LCD_WR_DATA8(0X81);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x85);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x79);
	LCD_WR_REG(0xCB);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x02);
	LCD_WR_REG(0xF7);
	LCD_WR_DATA8(0x20);
	LCD_WR_REG(0xEA);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC0); //Power control
	LCD_WR_DATA8(0x1D); //VRH[5:0]
	LCD_WR_REG(0xC1); //Power control
	LCD_WR_DATA8(0x12); //SAP[2:0];BT[3:0]
	LCD_WR_REG(0xC5); //VCM control
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3F);
	LCD_WR_REG(0xC7); //VCM control
	LCD_WR_DATA8(0x92);
	LCD_WR_REG(0x3A); // Memory Access Control
	LCD_WR_DATA8(0x55);
	LCD_WR_REG(0x36); // Memory Access Control
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);
	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x12);
	LCD_WR_REG(0xB6); // Display Function Control
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0xA2);

	LCD_WR_REG(0x44);
	LCD_WR_DATA8(0x02);

	LCD_WR_REG(0xF2); // 3Gamma Function Disable
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x26); //Gamma curve selected
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xE0); //Set Gamma
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x22);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x1B);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x48);
	LCD_WR_DATA8(0xB8);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0XE1); //Set Gamma
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x24);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x47);
	LCD_WR_DATA8(0x4B);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x06);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0F);
	LCD_WR_REG(0x29); //Display on
} // ILI9341