add_library(lcd INTERFACE)

cmake_policy(SET CMP0076 NEW)
target_sources(lcd INTERFACE
	lcd.c
	lcd.h
)
target_include_directories(lcd INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(lcd INTERFACE common pico_base_headers pico_stdlib hardware_spi)
