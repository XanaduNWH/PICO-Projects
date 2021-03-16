#include <stdlib.h>

#define SGP30_Device_addr 0x58

// device command
uint8_t sgp30_Init_air_quality_cmd[] = {0x20,0x03};
uint8_t sgp30_Measure_air_quality_cmd[] = {0x20,0x08};
uint8_t sgp30_Get_baseline_cmd[] = {0x20,0x15};
uint8_t sgp30_Set_baseline_cmd[] = {0x20,0x1E};
uint8_t sgp30_Set_humidity_cmd[] = {0x20,0x61};
uint8_t sgp30_Measure_test_cmd[] = {0x20,0x32};
uint8_t sgp30_Get_feature_set_version_cmd[] = {0x20,0x2f};
uint8_t sgp30_Measure_raw_signals_cmd[] = {0x20,0x50};
uint8_t sgp30_Get_serial_ID_cmd[] = {0x36,0x82};

// data save to at24c32
uint8_t sgp30_baseline_addr[] = {0x00,0x00};

void SGP30_Init(void);