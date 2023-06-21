#include "test_helpers.h"

int slave_on_reply(modbus_slave_t *slave, uint8_t *buf, uint16_t len) {
    printf("[%d] -> ", slave->id);
    int i;
    for (i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\r\n");
    return 0;
}