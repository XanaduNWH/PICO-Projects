#ifndef __COMMON_H
#define __COMMON_H

#define SPI_PORT spi1
#define I2C_PORT i2c1

#define PIN_MISO	12
#define PIN_SCK		10
#define PIN_MOSI	11

#define UART_PORT uart0
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define TX_PIN		0
#define RX_PIN		1

#define AT24C32_Device_addr 0x57
#define DS3231_Device_addr 0x68

#endif