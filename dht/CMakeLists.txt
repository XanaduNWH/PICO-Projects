add_library(dht INTERFACE)

cmake_policy(SET CMP0076 NEW)
target_sources(dht INTERFACE
	dht.c
	dht.h
)
target_include_directories(dht INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(dht INTERFACE pico_stdlib pico_stdio)
