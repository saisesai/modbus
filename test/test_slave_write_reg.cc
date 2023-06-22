#include "modbus.h"

#include "gtest/gtest.h"

#include "test_helpers.h"

TEST(slave_write, write_one_register) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;
    slave.on_write = slave_on_reply;

    uint32_t data = 0;
    modbus_register_t reg;
    modbus_register_init(&reg);
    reg.index = 1;
    reg.size = 2;
    reg.data = (uint8_t *) &data;
    modbus_slave_add_register(&slave, &reg);

    uint8_t buf[256] = {
            0x01, 0x10,
            0x00, 0x00, 0x00, 0x02,
            0x04, 0x44, 0x33, 0x22, 0x11,
            0xCF, 0xFC
    };

    modbus_slave_handle_rtu(&slave, buf, 13);
    ASSERT_EQ(0x11223344, data);
}

TEST(slave_write, write_multi_registers) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;
    slave.on_write = slave_on_reply;

    uint32_t data_u32 = 0;
    modbus_register_t reg_u32;
    modbus_register_init(&reg_u32);
    reg_u32.index = 1;
    reg_u32.size = 2;
    reg_u32.data = (uint8_t *) &data_u32;
    modbus_slave_add_register(&slave, &reg_u32);

    float data_f32 = 0.0f;
    modbus_register_t reg_f32;
    modbus_register_init(&reg_f32);
    reg_f32.index = 3;
    reg_f32.size = 2;
    reg_f32.data = (uint8_t *) &data_f32;
    modbus_slave_add_register(&slave, &reg_f32);

    uint8_t buf_u32[256] = {
            0x01, 0x10,
            0x00, 0x00, 0x00, 0x02,
            0x04, 0x44, 0x33, 0x22, 0x11,
            0xCF, 0xFC
    };
    modbus_slave_handle_rtu(&slave, buf_u32, 13);
    ASSERT_EQ(0x11223344, data_u32);

    uint8_t buf_f32[256] = {
            0x01, 0x10,
            0x00, 0x02, 0x00, 0x02,
            0x04, 0xC3, 0xF5, 0x48, 0x40,
            0x69, 0xF0
    };
    modbus_slave_handle_rtu(&slave, buf_f32, 13);
    ASSERT_EQ(3.14f, data_f32);
}