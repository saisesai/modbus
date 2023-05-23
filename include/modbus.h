#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

float modbus_f32_byte_swap(const uint8_t *buf);

struct modbus_register_s;
typedef struct modbus_register_s modbus_register_t;

/*return negative value to make internal error reply*/
typedef int (*modbus_register_write_cb)(modbus_register_t *reg, uint8_t *buf);

/*modbus register chain list node*/
struct modbus_register_s {
    uint16_t index; /*start from 1*/
    uint16_t size; /*in half word*/
    uint8_t *data; /*data ptr*/
    modbus_register_write_cb on_write;
    struct modbus_register_s *next;
};

/*init modbus register chain list node struct*/
int modbus_register_init(modbus_register_t *reg);

struct modbus_slave_s;
typedef struct modbus_slave_s modbus_slave_t;

/*on modbus reply callback*/
typedef int (*modbus_on_reply_cb)(modbus_slave_t *slave, uint8_t *buf, uint16_t len);

/*modbus slave*/
struct modbus_slave_s {
    uint8_t id;
    modbus_register_t register_entry;
    uint16_t register_len;
    modbus_on_reply_cb on_reply;
};

/*init modbus slave struct*/
int modbus_slave_init(modbus_slave_t *slave);

/*add register to slave*/
int modbus_slave_add_register(modbus_slave_t *slave, modbus_register_t *reg);

/*remove register from slave*/
int modbus_slave_remove_register(modbus_slave_t *slave, modbus_register_t *reg);

/**
 * @brief slave handle incoming rtu data
 * @param slave slave ptr
 * @param buf data buffer ptr
 * @param len data length
 * @retval
 *      - 0: ok \n
 *      - 1: adu is not for this device \n
 *      - 2: adu is not valid (too short or wrong crc)
 **/
int modbus_slave_handle_rtu(modbus_slave_t *slave, uint8_t *buf, uint16_t len);

#endif /*MODBUS_H*/
