#include <stdio.h>
#include <string.h>
#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "lcd.h"
#include "zk.h"

static inline void zkcs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(ZK_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void zkcs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(ZK_CS, 1);
    asm volatile("nop \n nop \n nop");
}

void ZK_command(uint8_t dat)
{
    spi_write_blocking(SPI_PORT, &dat, 1);
}

void get_n_bytes_data_from_ROM(uint8_t AddrHigh,uint8_t AddrMid,uint8_t AddrLow,uint8_t *pBuff,uint8_t DataLen)
{
	zkcs_select();
	ZK_command(0x03);//写指令
	ZK_command(AddrHigh);//写地址高字节
	ZK_command(AddrMid);//写地址中字节
	ZK_command(AddrLow);//写地址低字节
	spi_read_blocking(SPI_PORT, 0, pBuff, DataLen);

	zkcs_deselect();
}

void Display_GB2312(uint16_t x,uint16_t y,uint8_t zk_num,uint16_t fc,uint16_t bc)
{
  uint8_t i,k;
	switch(zk_num)
	{
		case 1:
			 {
		 	   LCD_Address_Set(x,y,x+15,y+11);
			   for(i=0;i<24;i++)
	       {
					for(k=0;k<8;k++)
					{
						if((FontBuf[i]&(0x80>>k))!=0)
						{
							LCD_WR_DATA(fc);
						}
						else
						{
							LCD_WR_DATA(bc);
						}
					}

	       }
			 }break;  // 12*12

		case 2:
			 {
		 	    LCD_Address_Set(x,y,x+15,y+15);
			    for(i=0;i<32;i++)
	       {
						for(k=0;k<8;k++)
						 {
							if((FontBuf[i]&(0x80>>k))!=0)
							 {
								LCD_WR_DATA(fc);
							 }
							else
							 {
								LCD_WR_DATA(bc);
							 }
						 }
	       }
		  }break;     // 15*16

	    case 3:
			 {
		 	    LCD_Address_Set(x,y,x+23,y+23);
			    for(i=0;i<72;i++)
						{
							for(k=0;k<8;k++)
							{
								if((FontBuf[i]&(0x80>>k))!=0)
							  {
									LCD_WR_DATA(fc);
								}
								else
								{
									LCD_WR_DATA(bc);
								}
							}

						 }
		  }break;     // 24*24

	    case 4:
			{
				LCD_Address_Set(x,y,x+31,y+31);
				for(i=0;i<128;i++)
			  {
					for(k=0;k<8;k++)
					{
						if((FontBuf[i]&(0x80>>k))!=0)
						{
							LCD_WR_DATA(fc);
						}
						else
						{
							LCD_WR_DATA(bc);
						}
					}
			  }
		  }break;    // 32*32
	}
}

void Display_GB2312_String(uint16_t x,uint16_t y,uint8_t zk_num,uint8_t text[],uint16_t fc,uint16_t bc)
{
	uint8_t i= 0;
	uint8_t AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	uint32_t FontAddr=0; //字地址
	uint32_t BaseAdd=0; //字库基地址
	uint8_t n,d;// 不同点阵字库的计算变量
	switch(zk_num)
	{
		// n个数d：字间距
		case 1 :  BaseAdd=0x00;    n=24;  d=12; break;  // 12*12
		case 2 :  BaseAdd=0x2C9D0; n=32;  d=16; break;   // 15*16
		case 3 :  BaseAdd=0x68190; n=72;  d=24; break;   // 24*24
		case 4 :  BaseAdd=0xEDF00; n=128; d=32; break;   // 32*32
	}
	while((text[i]>0x00))
	{
		if(((text[i]>=0xA1)&&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
		{
			//国标简体（GB2312）汉字在 字库IC中的地址由以下公式来计算：//
			//Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; 分三部取地址///
			FontAddr = (text[i]- 0xA1)*94;
			FontAddr += (text[i+1]-0xA1);
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);

			AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位//
			AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位//
			AddrLow = FontAddr&0xff;	     //地址的低8位,共24位//
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[]"
			Display_GB2312(x,y,zk_num,fc,bc);//显示一个汉字到LCD上/
		}
		else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
		{
			//国标简体（GB2312） 字库IC中的地址由以下公式来计算：//
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; 分三部取地址//
			FontAddr = (text[i]- 0xB0)*94;
			FontAddr += (text[i+1]-0xA1)+846;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);

			AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位//
			AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位//
			AddrLow = FontAddr&0xff;	     //地址的低8位,共24位//
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[ ]"
			Display_GB2312(x,y,zk_num,fc,bc);//显示一个汉字到LCD上/
		}
		x+=d; //下一个字坐标
		i+=2;  //下个字符
	}
}

void Display_Asc(uint16_t x,uint16_t y,uint8_t zk_num,uint16_t fc,uint16_t bc)
{
	unsigned char i,k;
	switch(zk_num)
	{
		case 1:   
		{
			LCD_Address_Set(x,y,x+7,y+7);
			for(i=0;i<7;i++)		 
			{
				for(k=0;k<8;k++)
				{ 		     
					if((FontBuf[i]&(0x80>>k))!=0)
					{
							LCD_WR_DATA(fc);
					} 
					else
					{
						LCD_WR_DATA(bc);
					}   
				}

			 }
		 }break;//5x7 ASCII

	 	case 2:   	
		{
		 	LCD_Address_Set(x,y,x+7,y+7);
			for(i=0;i<8;i++)		 
			{
				for(k=0;k<8;k++)
				{ 		     
					if((FontBuf[i]&(0x80>>k))!=0)
					{
						LCD_WR_DATA(fc);
					} 
					else
					{
						LCD_WR_DATA(bc);
					}   
				}
			}
		}break;   //	  7x8 ASCII

	   case 3:   
		 {
			 LCD_Address_Set(x,y,x+7,y+11);
			 for(i=0;i<12;i++)		 
			 {
				 for(k=0;k<8;k++)
				 { 		     
					 if((FontBuf[i]&(0x80>>k))!=0)
					 {
						 LCD_WR_DATA(fc);
					 } 
					 else
					 {
						LCD_WR_DATA(bc);
					 }   
				 }
			 }
		 }break;  //  6x12 ASCII

		 case 4:  
		 {
			 LCD_Address_Set(x,y,x+7,y+15);
			 for(i=0;i<16;i++)		 
			 {
				 for(k=0;k<8;k++)
				 { 		     
					 if((FontBuf[i]&(0x80>>k))!=0)
					 {
						 LCD_WR_DATA(fc);
					 } 
					 else
					 {
						 LCD_WR_DATA(bc);
					 }   
				 }
			 }
		 }break;     //  8x16 ASCII

	    case 5:  
			{
				LCD_Address_Set(x,y,x+15,y+24);
			  for(i=0;i<48;i++)		 
				{
					for(k=0;k<8;k++)
					{ 		     
						if((FontBuf[i]&(0x80>>k))!=0)
						{
							LCD_WR_DATA(fc);
						} 
						else
					  {
							LCD_WR_DATA(bc);
						}   
					}
				}
			}break;    //  12x24 ASCII

	    case 6:  	 
			{
				LCD_Address_Set(x,y,x+15,y+31);
				for(i=0;i<64;i++)		 
				{
					for(k=0;k<8;k++)
					{ 		     
						if((FontBuf[i]&(0x80>>k))!=0)
						{
							LCD_WR_DATA(fc);
						} 
						else
						{
							LCD_WR_DATA(bc);
						}   
					}
				}
			}break;   //  16x32 ASCII
	}
}

void Display_Asc_String(uint16_t x,uint16_t y,uint16_t zk_num,uint8_t text[],uint16_t fc,uint16_t bc)
{
	uint8_t i= 0;
	uint8_t AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	uint32_t FontAddr=0; //字地址
	uint32_t BaseAdd=0; //字库基地址	
  	uint8_t n,d;// 不同点阵字库的计算变量
	switch(zk_num)
	{
		//n个数，d:字间距
		case 1:  BaseAdd=0x1DDF80; n=8;  d=6;  break;	 //	  5x7 ASCII
		case 2:  BaseAdd=0x1DE280; n=8;  d=8;  break;	 //   7x8 ASCII
		case 3:  BaseAdd=0x1DBE00; n=12; d=6;  break;	 //  6x12 ASCII
		case 4:  BaseAdd=0x1DD780; n=16; d=8;  break;	 //  8x16 ASCII
		case 5:  BaseAdd=0x1DFF00; n=48; d=12; break;	 //  12x24 ASCII
	 	case 6:  BaseAdd=0x1E5A50; n=64; d=16; break;	 //  16x32 ASCII
	}
	while((text[i]>0x00))
	{	
	  if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
		  FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_Asc(x,y,zk_num,fc,bc);/*显示一个ascii到LCD上 */
		}
    i++;  //下个数据
		x+=d;//下一个字坐标 
	}
}
