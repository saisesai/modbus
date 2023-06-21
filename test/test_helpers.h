#ifndef MODBUS_TEST_HELPERS_H
#define MODBUS_TEST_HELPERS_H

#include "modbus.h"

int slave_on_reply(modbus_slave_t *slave, uint8_t *buf, uint16_t len);

#endif //MODBUS_TEST_HELPERS_H
