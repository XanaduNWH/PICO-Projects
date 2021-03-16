#include <stdio.h>
#include <string.h>
#include "common.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "lcd.h"
#include "zk.h"
#include "bme280.h"
#include "sgp30.h"

const uint LED_PIN = 25;

uint8_t ch2odata[8],
		line_buf_ch2o[8],
		line_buf_humidity[8],
		line_buf_pressure[8],
		line_buf_temperature[8],
		line_buf_tovc[8],
		line_buf_time[8],
		line_buf_date[16],
		line_buf_year[4],
		line_buf_month[2],
		line_buf_day[2];
uint8_t k;

uint8_t SubmitData(){
	uint8_t i,tempd = 0;
	for(i=1;i<8;i++){
		tempd+=ch2odata[i];
	}
	// printf("%X ",(uint8_t)(~tempd)+1);
	if(ch2odata[8] == (uint8_t)((~tempd)+1)){
		uint16_t ch2obuf = ch2odata[4]*256 + ch2odata[5];
		//printf("|%#X ",ch2odata[5]);
		sprintf(line_buf_ch2o,"%4d",ch2obuf);
	} else {
		sprintf(line_buf_ch2o,"----");
	}
}

// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_PORT)) {
		gpio_put(LED_PIN, 1);
        uint8_t ch = uart_getc(UART_PORT);
		gpio_put(LED_PIN, 0);
		// printf("%X ",ch);
		if(ch==0xFF){
			k=0;
			ch2odata[0]=ch;
		} else {
			k++;
			ch2odata[k]=ch;
			if(k==8){
				SubmitData();
				// printf("\n");
			}
		}
    }

}

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

static void alarm_callback(void) {
	datetime_t t = {0};
	rtc_get_datetime(&t);
	char datetime_buf[256];
	char *datetime_str = &datetime_buf[0];
	datetime_to_str(datetime_str, sizeof(datetime_buf), &t);
	printf("Alarm Fired At %s\n", datetime_str);
	stdio_flush();
}

int main() {
	uint16_t i, j;
	uint16_t color;
	int32_t humidity, pressure, temperature;

	uint8_t sgp30_data[6],sgp30_baseline[6],tmp[6],ds3231_data[19];
	uint8_t at24c32_page_buff[32], sgp30_baseline_buff[8];
	uint16_t timer_count = 0;
	uint8_t ds3231_reg = 0;

	stdio_init_all();

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	// i2c port
	i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

	// SPI port
	spi_init(SPI_PORT, 1000 * 2000 * 10);
	gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
	gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
	gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);

	// uart port
	uart_init(UART_PORT, BAUD_RATE);
	gpio_set_function(TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(RX_PIN, GPIO_FUNC_UART);
	uart_set_hw_flow(UART_PORT, false, false);
	uart_set_format(UART_PORT, DATA_BITS, STOP_BITS, PARITY);
	uart_set_fifo_enabled(UART_PORT, false);
	int UART_IRQ = UART0_IRQ;
	irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
	irq_set_enabled(UART_IRQ, true);
	uart_set_irq_enables(UART_PORT, true, false);

	// lcd gpio
	gpio_init(LCD_CS);
	gpio_set_dir(LCD_CS, GPIO_OUT);
	gpio_put(LCD_CS, 1);
	gpio_init(LCD_RES);
	gpio_set_dir(LCD_RES, GPIO_OUT);
	gpio_init(LCD_DC);
	gpio_set_dir(LCD_DC, GPIO_OUT);

	// zk gpio
	gpio_init(ZK_CS);
	gpio_set_dir(ZK_CS, GPIO_OUT);
	gpio_put(ZK_CS, 1);

	// bme280 gpio
    gpio_init(BME280_CS);
    gpio_set_dir(BME280_CS, GPIO_OUT);
    gpio_put(BME280_CS, 1);

	Lcd_Init();

	i2c_write_blocking(I2C_PORT, DS3231_Device_addr, &ds3231_reg, 1, false);
	i2c_read_blocking(I2C_PORT, DS3231_Device_addr, ds3231_data, 18, false);
	datetime_t ds3231_t = {
		.sec = (ds3231_data[0]>>4)*10 + (ds3231_data[0]&0x0F),
		.min = (ds3231_data[1]>>4)*10 + (ds3231_data[1]&0x0F),
		.hour = (ds3231_data[2]&0x40) == 0x40 ? 						// True:12 Hours False:24Hours
						((ds3231_data[2]&0x20) == 0x20 ?						// True:PM
								((ds3231_data[2]>>4)&0x01)*10 + (ds3231_data[2]&0x0F) + 12:
								((ds3231_data[2]>>4)&0x01)*10 + (ds3231_data[2]&0x0F)):
						((ds3231_data[2]&0x20) == 0x20 ?						// True:20~23
								(ds3231_data[2]&0x0F) + 20:
								((ds3231_data[2]>>4)&0x01)*10 + (ds3231_data[2]&0x0F)),
		.dotw = ds3231_data[3] - 1,
		.day = (ds3231_data[4]>>4)*10 + (ds3231_data[4]&0x0F),
		.year =(ds3231_data[6]>>4)*10 + (ds3231_data[6]&0x0F) + (((ds3231_data[5]&0x80) == 0x80) ? 1900 : 2000),
		.month = ((ds3231_data[5]&0x7F)>>4)*10 + (ds3231_data[5]&0x0F),
	};
	rtc_init();
	rtc_set_datetime(&ds3231_t);

	datetime_t alarm = {
		.year = 2021,
		.month = -1,
		.day = -1,
		.dotw = -1,
		.hour = -1,
		.min = -1,
		.sec = -1
	};

	rtc_set_alarm(&alarm,&alarm_callback);

	read_compensation_parameters();

    write_register(0xF2, 0x1); // Humidity oversampling register - going for x1
    write_register(0xF4, 0x27);// Set rest of oversampling modes and run mode to normal

	i2c_write_blocking(I2C_PORT, SGP30_Device_addr, sgp30_Init_air_quality_cmd, 2, false);
	sleep_ms(10);

	i2c_write_blocking(I2C_PORT, AT24C32_Device_addr, sgp30_baseline_addr, 2, true);
	sleep_ms(10);
	i2c_read_blocking(I2C_PORT, AT24C32_Device_addr, tmp, 6, false);

	for(uint8_t i = 0; i<2;i++){
		sgp30_baseline_buff[i] = sgp30_Set_baseline_cmd[i];
	}
	for(uint8_t i = 2; i<8;i++){
		sgp30_baseline_buff[i] = tmp[i-2];
	}
	i2c_write_blocking(I2C_PORT, SGP30_Device_addr, sgp30_baseline_buff, 8, false);


	color = BRRED;
	// printf("Starting...\n");

	LCD_Address_Set(0, 0, 239, 319);
	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			LCD_WR_DATA(color);
		}
	}

	Display_GB2312_String(0,0,4, "甲醛：",~color, color);
	Display_Asc_String(176,0,6, "PPB",~color, color);
	Display_Asc_String(0,32,6, "TOVC",~color, color);
	Display_GB2312_String(64,32,4, "：",~color, color);
	Display_Asc_String(176,32,6, "PPB",~color, color);
	Display_GB2312_String(0,64,4, "湿度：",~color, color);
	Display_GB2312_String(0,96,4, "温度：",~color, color);
	Display_GB2312_String(0,128,4, "气压：",~color, color);
	Display_GB2312_String(64,160,4, "年",~color, color);
	Display_GB2312_String(128,160,4, "月",~color, color);
	Display_GB2312_String(192,160,4, "日",~color, color);
	while(1) {
/*
		printf("I2C Bus Scan\n");
		printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
		for (int addr = 0; addr < (1 << 7); ++addr) {
			if (addr % 16 == 0) {
				printf("%02x ", addr);
			}

			// Perform a 1-byte dummy read from the probe address. If a slave
			// acknowledges this address, the function returns the number of bytes
			// transferred. If the address byte is ignored, the function returns
			// -1.

			// Skip over any reserved addresses.
			int ret;
			uint8_t rxdata;
			if (reserved_addr(addr))
				ret = PICO_ERROR_GENERIC;
			else
				ret = i2c_read_blocking(I2C_PORT, addr, &rxdata, 1, false);

			printf(ret < 0 ? "." : "@");
			printf(addr % 16 == 15 ? "\n" : "  ");
		}
		printf("Done.\n");
*/

		i2c_write_blocking(I2C_PORT, SGP30_Device_addr, sgp30_Measure_air_quality_cmd, 2, true);
		sleep_ms(20);
		i2c_read_blocking(I2C_PORT, SGP30_Device_addr, sgp30_data, 6, false);

		sprintf(line_buf_tovc,"%4d",sgp30_data[3]*256+sgp30_data[4]);

		bme280_read_raw(&humidity, &pressure, &temperature);
        // These are the raw numbers from the chip, so we need to run through the
        // compensations to get human understandable numbers
        pressure = compensate_pressure(pressure);
        temperature = compensate_temp(temperature);
        humidity = compensate_humidity(humidity);

		sprintf(line_buf_humidity,"%.2f%%",humidity / 1024.0);
		sprintf(line_buf_pressure,"%dPa", pressure);
		sprintf(line_buf_temperature,"%.2fC", temperature / 100.0);
//        printf("\rHumidity = %.2f%%\t", humidity / 1024.0);
//        printf("Pressure = %dPa\t", pressure);
//        printf("Temp. = %.2fC", temperature / 100.0);

		Display_Asc_String(96,0,6,line_buf_ch2o,~color,color);
		Display_Asc_String(96,32,6,line_buf_tovc,~color,color);
		Display_Asc_String(96,64,6,line_buf_humidity,~color,color);
		Display_Asc_String(96,96,6,line_buf_temperature,~color,color);
		Display_Asc_String(96,128,6,line_buf_pressure,~color,color);

		rtc_get_datetime(&ds3231_t);

		sprintf(line_buf_year,"%4d",ds3231_t.year);
		sprintf(line_buf_month,"%2d",ds3231_t.month);
		sprintf(line_buf_day,"%2d",ds3231_t.day);
		sprintf(line_buf_time,"%02d:%02d:%02d",ds3231_t.hour,ds3231_t.min,ds3231_t.sec);
		Display_Asc_String(0,160,6,line_buf_year,~color,color);
		Display_Asc_String(96,160,6,line_buf_month,~color,color);
		Display_Asc_String(160,160,6,line_buf_day,~color,color);
		Display_Asc_String(0,192,6,line_buf_time,~color,color);


		if(timer_count > 1800){
			timer_count = 0;
			i2c_write_blocking(I2C_PORT, SGP30_Device_addr, sgp30_Get_baseline_cmd, 2, true);
			sleep_ms(10);
			i2c_read_blocking(I2C_PORT, SGP30_Device_addr, sgp30_baseline, 6, false);

			for(uint8_t i = 0; i<2;i++){
				at24c32_page_buff[i] = sgp30_baseline_addr[i];
			}
			for(uint8_t i = 2; i<8;i++){
				at24c32_page_buff[i] = sgp30_baseline[i-2];
			}
			i2c_write_blocking(I2C_PORT, AT24C32_Device_addr, at24c32_page_buff, 8, false);
			for(uint8_t i = 0; i<6; i++){
				printf("%#X ",sgp30_baseline[i]);
			}
			printf("\t");
			printf("SGP30 baseline updated.\n");

		}
		timer_count++;

		sleep_ms(900);
	}
}