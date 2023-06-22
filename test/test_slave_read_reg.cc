#include "modbus.h"

#include <iostream>

#include "gtest/gtest.h"

#include "test_helpers.h"


TEST(slave_read_reg, invalid_data) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0C};

    int rc = modbus_slave_rtu_handle(&slave, buf, 6);
    EXPECT_EQ(-2, rc);

    rc = modbus_slave_rtu_handle(&slave, buf, 8);
    EXPECT_EQ(-2, rc);
}

TEST(slave_read_reg, not_this_device) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x02;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};

    int rc = modbus_slave_rtu_handle(&slave, buf, 8);
    EXPECT_EQ(-1, rc);
}

TEST(slave_read_reg, wrong_address) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

    int rc = modbus_slave_rtu_handle(&slave, buf, 8);

    EXPECT_EQ(2, rc);
    EXPECT_EQ(0x83, buf[1]);
}

int read_cb_0(modbus_register_t *reg, const uint8_t *buf) {
    (void) buf;
    std::cout << "read register: " << reg->index << std::endl;
    EXPECT_EQ(0x01, reg->index);
    return -1;
}

int read_cb_1(modbus_register_t *reg, const uint8_t *buf) {
    (void) buf;
    std::cout << "read register: " << reg->index << std::endl;
    EXPECT_EQ(0x03, reg->index);
    float &t2 = *((float *) (reg->data));
    t2 += 2.33f;
    return 0;
}

TEST(slave_read_reg, on_read) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;
    slave.on_read = slave_on_receive;
    slave.on_write = slave_on_reply;

    float t1 = 3.14f;
    modbus_register_t t1_reg;
    modbus_register_init(&t1_reg);
    t1_reg.index = 0x01;
    t1_reg.size = 2;
    t1_reg.data = (uint8_t *) &t1;
    t1_reg.on_read = read_cb_0;
    modbus_slave_add_register(&slave, &t1_reg);

    float t2 = 0.0f;
    modbus_register_t t2_reg;
    modbus_register_init(&t2_reg);
    t2_reg.index = 0x03;
    t2_reg.size = 2;
    t2_reg.data = (uint8_t *) &t2;
    t2_reg.on_read = read_cb_1;
    modbus_slave_add_register(&slave, &t2_reg);

    uint8_t buf0[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
    modbus_slave_rtu_handle(&slave, buf0, 8);
    EXPECT_EQ(0x83, buf0[1]);
    EXPECT_EQ(0x04, buf0[2]);

    uint8_t buf1[256] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB};
    modbus_slave_rtu_handle(&slave, buf1, 8);
    EXPECT_EQ(0x03, buf1[1]);
    float t2_out;
    memcpy(&t2_out, &buf1[3], 4);
    EXPECT_EQ(2.33f, t2_out);
}

