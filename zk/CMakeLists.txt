add_library(zk INTERFACE)

cmake_policy(SET CMP0076 NEW)
target_sources(zk INTERFACE
	zk.c
	zk.h
)
target_include_directories(zk INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(zk INTERFACE pico_base_headers pico_stdlib hardware_spi)
