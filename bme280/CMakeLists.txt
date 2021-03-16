add_library(bme280 INTERFACE)

cmake_policy(SET CMP0076 NEW)
target_sources(bme280 INTERFACE
	bme280.c
	bme280.h
)
target_include_directories(bme280 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(bme280 INTERFACE common pico_base_headers pico_stdlib hardware_spi)
