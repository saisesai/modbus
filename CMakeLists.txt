cmake_minimum_required(VERSION 3.15)

project(modbus)

add_library(modbus STATIC src/modbus.c)
target_include_directories(modbus PUBLIC include)
target_compile_options(
        modbus PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
set_property(TARGET modbus PROPERTY C_STANDARD 90)
set_property(TARGET modbus PROPERTY C_STANDARD_REQUIRED ON)

add_subdirectory(third_party/googletest)
add_executable(
        modbus_test
        test/test_helpers.cc
        test/test_master_read_reg.cc
        test/test_master_write_reg.cc
        test/test_slave_read_reg.cc
        test/test_slave_write_reg.cc
)
target_link_libraries(modbus_test modbus gtest gtest_main)
