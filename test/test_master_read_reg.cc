#include "modbus.h"

#include "gtest/gtest.h"

TEST(master_read_reg, holding_register) {
    uint8_t len0 = 255;
    uint8_t buf0[255] = {0x00};
    const uint8_t out0[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
    modbus_master_read_registers_rtu(1, 0, 1, buf0, &len0);
    ASSERT_EQ(8, len0);
    ASSERT_EQ(0, memcmp(buf0, out0, 8));
}

TEST(master_read_reg, input_register) {
    uint8_t len0 = 255;
    uint8_t buf0[255] = {0x00};
    const uint8_t out0[8] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x31, 0xCA};
    modbus_master_read_registers_rtu_ex(
            1,
            MODBUS_READ_INPUT_REGISTERS,
            0, 1,
            buf0, &len0
    );
    ASSERT_EQ(8, len0);
    ASSERT_EQ(0, memcmp(buf0, out0, 8));
}