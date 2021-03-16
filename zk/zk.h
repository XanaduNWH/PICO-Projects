#include "pico/stdio.h"
#include "common.h"

#define ZK_CS		14

#define SPI_PORT spi1

uint8_t FontBuf[128];

static inline void zkcs_select();
static inline void zkcs_deselect();
void ZK_command(uint8_t dat);
void get_n_bytes_data_from_ROM(uint8_t AddrHigh,uint8_t AddrMid,uint8_t AddrLow,uint8_t *pBuff,uint8_t DataLen);
void Display_GB2312(uint16_t x,uint16_t y,uint8_t zk_num,uint16_t fc,uint16_t bc);
void Display_GB2312_String(uint16_t x,uint16_t y,uint8_t zk_num,uint8_t text[],uint16_t fc,uint16_t bc);
void Display_Asc(uint16_t x,uint16_t y,uint8_t zk_num,uint16_t fc,uint16_t bc);
void Display_Asc_String(uint16_t x,uint16_t y,uint16_t zk_num,uint8_t text[],uint16_t fc,uint16_t bc);
