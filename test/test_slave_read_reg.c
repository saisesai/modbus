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
    printf("%s: ", __func__ );

    modbus_slave_t slave;
    modbus_slave_init(&slave);
    slave.id = 0x01;

    uint8_t buf[256] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

    int rc = modbus_slave_handle_rtu(&slave, buf, 8);
    if (rc != 0) {
        printf("expect 0 but: %d\r\n", rc);
        exit(-1);
    }

    if(buf[1] != 0x83) {
        printf("expect 0x83 but: 0x%x", buf[1]);
        exit(-1);
    }

    printf("pass!\r\n");
}
