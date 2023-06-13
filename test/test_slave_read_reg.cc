#include "modbus.h"

#include "gtest/gtest.h"

TEST(slave_read_reg, invalid_data) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0C};

    int rc = modbus_slave_handle_rtu(&slave, buf, 6);
    EXPECT_EQ(2, rc);

    rc = modbus_slave_handle_rtu(&slave, buf, 8);
    EXPECT_EQ(2, rc);
}

TEST(slave_read_reg, not_this_device) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x02;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};

    int rc = modbus_slave_handle_rtu(&slave, buf, 8);
    EXPECT_EQ(1, rc);
}

TEST(slave_read_reg, wrong_address) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

    int rc = modbus_slave_handle_rtu(&slave, buf, 8);

    EXPECT_EQ(0, rc);
    EXPECT_EQ(0x83, buf[1]);
}

int slave_on_reply(modbus_slave_t *slave, uint8_t *buf, uint16_t len) {
    printf("[%d] -> ", slave->id);
    int i;
    for (i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\r\n");
    return 0;
}

TEST(slave_read_reg, normal) {
    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;
    slave.on_reply = slave_on_reply;

    uint32_t t1 = 0xffcc99ff, t2 = 0xaabbccdd;
    float t3 = 0.314f;

    modbus_register_t t1_reg;
    modbus_register_init(&t1_reg);
    t1_reg.index = 1;
    t1_reg.data = (uint8_t *) &t1;
    t1_reg.size = 2;

    modbus_register_t t2_reg;
    modbus_register_init(&t2_reg);
    t2_reg.index = 3;
    t2_reg.data = (uint8_t *) &t2;
    t2_reg.size = 2;

    modbus_register_t t3_reg;
    modbus_register_init(&t3_reg);
    t3_reg.index = 5;
    t3_reg.data = (uint8_t *) &t3;
    t3_reg.size = 2;

    modbus_slave_add_register(&slave, &t1_reg);
    modbus_slave_add_register(&slave, &t2_reg);
    modbus_slave_add_register(&slave, &t3_reg);

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
    int rc = modbus_slave_handle_rtu(&slave, buf, 8);
    EXPECT_EQ(0x00, rc);
    EXPECT_EQ(0x03, buf[1]);

    uint8_t buf1[256] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB};
    rc = modbus_slave_handle_rtu(&slave, buf1, 8);
    EXPECT_EQ(0x00, rc);
    EXPECT_EQ(0x03, buf1[1]);

    uint8_t buf2[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x04, 0x44, 0x09};
    rc = modbus_slave_handle_rtu(&slave, buf2, 8);
    EXPECT_EQ(0x00, rc);
    EXPECT_EQ(0x03, buf2[1]);

    uint8_t buf3[256] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x02, 0x85, 0xCA};
    rc = modbus_slave_handle_rtu(&slave, buf3, 8);
    EXPECT_EQ(0x00, rc);
    EXPECT_EQ(0x03, buf3[1]);
}

