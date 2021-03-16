cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(projects C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# libs
add_subdirectory(common)
add_subdirectory(lcd)
add_subdirectory(zk)
add_subdirectory(dht)
add_subdirectory(bme280)
add_subdirectory(sgp30)

# apps
add_subdirectory(CH2O)