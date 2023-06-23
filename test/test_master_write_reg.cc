#include "modbus.h"

#include "gtest/gtest.h"

TEST(master_write_reg, common) {
    const uint8_t out0[] = {
            0x11, 0x10,
            0x00, 0x01, 0x00, 0x02,
            0x04, 0x00, 0x0A, 0x01, 0x02,
            0xC6, 0xF0
    };
    uint8_t len0 = 255;
    uint8_t buf0[255] = {0x00};
    uint8_t reg0[4] = {0x00, 0x0A, 0x01, 0x02};
    int rc0 = modbus_master_write_registers_rtu(
            0x11, 0x01, 0x02,
            reg0, buf0, &len0
    );
    ASSERT_EQ(13, rc0);
    ASSERT_EQ(13, len0);
    ASSERT_EQ(0, memcmp(out0, buf0, 13));
}
