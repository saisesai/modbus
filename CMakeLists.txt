cmake_minimum_required(VERSION 3.15)

project(modbus)

add_library(modbus STATIC src/modbus.c)
target_include_directories(modbus PUBLIC include)
target_compile_options(modbus PRIVATE -Wall -Wextra -Wpedantic)
set_property(TARGET modbus PROPERTY C_STANDARD 90)
set_property(TARGET modbus PROPERTY C_STANDARD_REQUIRED ON)

add_subdirectory(third_party/googletest)
target_compile_options(gtest PRIVATE -Wno-undef)
add_executable(
        modbus_test
        test/test_slave_read_reg.cc
)
target_link_libraries(modbus_test modbus gtest gtest_main)
