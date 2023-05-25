#include "test_slave_read_reg.h"

#include "modbus.h"

#include <stdio.h>
#include <stdlib.h>

void test_slave_read_reg_invalid_data(void) {
    printf("%s: ", __func__);

    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0C};

    int rc = modbus_slave_handle_rtu(&slave, buf, 6);
    if (rc != 2) {
        printf("expect 2 but: %d\r\n", rc);
        exit(-1);
    }

    rc = modbus_slave_handle_rtu(&slave, buf, 8);
    if (rc != 2) {
        printf("expect 2 but: %d\r\n", rc);
        exit(-1);
    }

    printf("pass!\r\n");
}

void test_slave_read_reg_not_this_device(void) {
    printf("%s: ", __func__);

    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x02;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};

    int rc = modbus_slave_handle_rtu(&slave, buf, 8);
    if (rc != 1) {
        printf("expect 1 but: %d\r\n", rc);
        exit(-1);
    }

    printf("pass!\r\n");
}

void test_slave_read_reg_wrong_address(void) {
    printf("%s: ", __func__);

    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

    int rc = modbus_slave_handle_rtu(&slave, buf, 8);
    if (rc != 0) {
        printf("expect 0 but: %d\r\n", rc);
        exit(-1);
    }

    if (buf[1] != 0x83) {
        printf("expect 0x83 but: 0x%x", buf[1]);
        exit(-1);
    }

    printf("pass!\r\n");
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

void test_slave_read_reg_normal(void) {
    printf("%s: \r\n", __func__);

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
    if(rc != 0x00 || buf[1] != 0x03) {
        printf("wrong rc or fc: %d %d\r\n", rc, buf[1]);
        exit(-1);
    }

    uint8_t buf1[256] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB};
    rc = modbus_slave_handle_rtu(&slave, buf1, 8);
    if(rc != 0x00 || buf1[1] != 0x03) {
        printf("wrong rc or fc: %d %d\r\n", rc, buf1[1]);
        exit(-1);
    }

    uint8_t buf2[256] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x04, 0x44, 0x09};
    rc = modbus_slave_handle_rtu(&slave, buf2, 8);
    if(rc != 0x00 || buf2[1] != 0x03) {
        printf("wrong rc or fc: %d %d\r\n", rc, buf2[1]);
        exit(-1);
    }

    uint8_t buf3[256] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x02, 0x85, 0xCA};
    rc = modbus_slave_handle_rtu(&slave, buf3, 8);
    if(rc != 0x00 || buf3[1] != 0x03) {
        printf("wrong rc or fc: %d %d\r\n", rc, buf3[1]);
        exit(-1);
    }

    printf("pass!\r\n");
}

