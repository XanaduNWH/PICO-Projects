add_library(sgp30 INTERFACE)

cmake_policy(SET CMP0076 NEW)
target_sources(sgp30 INTERFACE
	sgp30.c
	sgp30.h
)
target_include_directories(sgp30 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(sgp30 INTERFACE common pico_base_headers pico_stdlib hardware_spi)
