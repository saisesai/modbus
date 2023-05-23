cmake_minimum_required(VERSION 3.15)

project(modbus)

add_library(modbus STATIC src/modbus.c)
target_include_directories(modbus PUBLIC include)
target_compile_options(modbus PRIVATE -Wall -Wextra -Wpedantic)
set_property(TARGET modbus PROPERTY C_STANDARD 90)
set_property(TARGET modbus PROPERTY C_STANDARD_REQUIRED ON)

add_executable(
        modbus_test
        test/test_main.c
        test/test_slave_read_reg.c
)
target_link_libraries(modbus_test PRIVATE modbus)
target_compile_options(modbus_test PRIVATE -Wall -Wextra -Wpedantic)
set_property(TARGET modbus_test PROPERTY C_STANDARD 99)
set_property(TARGET modbus_test PROPERTY C_STANDARD_REQUIRED ON)
