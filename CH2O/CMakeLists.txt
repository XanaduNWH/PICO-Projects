add_executable(ch2o
    main.c
    )

target_link_libraries(ch2o common lcd zk bme280 sgp30 pico_stdlib pico_stdio hardware_rtc hardware_spi hardware_i2c hardware_uart)

pico_enable_stdio_usb(ch2o 1)
pico_enable_stdio_uart(ch2o 0)

pico_add_extra_outputs(ch2o)