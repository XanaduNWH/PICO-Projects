add_library(common INTERFACE)

cmake_policy(SET CMP0076 NEW)
target_sources(common INTERFACE
	common.h
)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(common INTERFACE pico_base_headers pico_stdlib hardware_spi)
