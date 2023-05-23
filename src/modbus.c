#include "modbus.h"

static uint16_t ccitt_table[256] =
        {0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280,
         0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481,
         0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81,
         0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880,
         0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81,
         0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80,
         0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680,
         0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081,
         0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281,
         0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480,
         0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80,
         0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881,
         0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80,
         0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81,
         0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681,
         0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080,
         0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281,
         0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480,
         0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80,
         0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881,
         0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80,
         0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81,
         0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681,
         0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080,
         0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280,
         0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481,
         0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81,
         0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880,
         0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81,
         0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80,
         0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680,
         0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081,
         0x4040,
        };

static uint16_t modbus_crc16(uint8_t *buf, int len) {
    uint16_t crc = 0xffff;
    while (len-- > 0) { crc = ccitt_table[(crc ^ *buf++) & 0xff] ^ (crc >> 8); }
    return crc;
}

float modbus_f32_byte_swap(const uint8_t *buf) {
    float rst;
    ((uint8_t *) &rst)[2] = buf[3];
    ((uint8_t *) &rst)[3] = buf[2];
    ((uint8_t *) &rst)[0] = buf[1];
    ((uint8_t *) &rst)[1] = buf[0];
    return rst;
}

static uint16_t modbus_reg_to_uint16(const uint8_t *buf) {
    uint16_t rst;
    ((uint8_t *) &rst)[0] = buf[1];
    ((uint8_t *) &rst)[1] = buf[0];
    return rst;
}

/*
static void modbus_uint16_to_reg(uint16_t uint16, uint8_t *buf) {
    buf[0] = ((uint8_t *) &uint16)[1];
    buf[1] = ((uint8_t *) &uint16)[0];
} */

int modbus_register_init(modbus_register_t *reg) {
    reg->index = 0;
    reg->size = 0;
    reg->data = NULL;
    reg->next = NULL;
    reg->on_write = NULL;
    return 0;
}

int modbus_slave_init(modbus_slave_t *slave) {
    slave->id = 0;
    modbus_register_init(&slave->register_entry);
    slave->register_len = 0;
    slave->on_reply = NULL;
    return 0;
}

int modbus_slave_add_register(modbus_slave_t *slave, modbus_register_t *reg) {
    /*TODO check address and length*/
    modbus_register_t *reg_last = &slave->register_entry;
    while (reg_last->next != NULL) {
        reg_last = reg_last->next;
    }
    reg_last->next = reg;
    slave->register_len++;
    return 0;
}

int modbus_slave_remove_register(modbus_slave_t *slave, modbus_register_t *reg) {
    modbus_register_t *reg_prev = NULL, *reg_now = NULL;
    for (reg_now = &slave->register_entry; reg_now != NULL; reg_now = reg_now->next) {
        if (reg_now == reg) {
            reg_prev->next = reg_now->next;
            break;
        }
        reg_prev = reg_now;
    }
    return 0;
}

/**
 * @brief modbus find slave register
 * @param slave slave ptr
 * @param addr_start register address start
 * @param reg return found register ptr, if not found do nothing
 * @retval
 *      - 0: register not found \n
 *      - 1: ok
 **/
static int modbus_slave_find_register(modbus_slave_t *slave, uint16_t addr_start, modbus_register_t **reg) {
    int reg_found = 0;
    modbus_register_t *reg_now;
    for (reg_now = &slave->register_entry; reg_now != NULL; reg_now = reg_now->next) {
        if (reg_now->index == addr_start) {
            reg_found = 1;
            *reg = reg_now;
            break;
        }
    }
    return reg_found;
}

/**
 * @brief slave handle rtu exception
 * @param slave slave ptr
 * @param buf buffer ptr
 * @param code exception code \n
 *      - 0x01: function code not supported \n
 *      - 0x02: invalid register address \n
 *      - 0x03: invalid register quantity \n
 *      - 0x04: internal error
 **/
static int modbus_slave_handle_rtu_exception(modbus_slave_t *slave, uint8_t *buf, uint8_t code) {
    uint16_t crc16;
    buf[1] += 0x80;
    buf[2] = code;
    crc16 = modbus_crc16(buf, 3);
    memcpy(&buf[3], &crc16, 2);
    if (slave->on_reply != NULL) {
        slave->on_reply(slave, buf, 5);
    }
    return 0;
}

static int modbus_slave_handle_rtu_fc03(modbus_slave_t *slave, uint8_t *buf) {
    int reg_found;
    uint16_t addr_start, reg_quantity, copied = 0, crc16;
    modbus_register_t *reg_now;
    /*get addr and quantity*/
    addr_start = modbus_reg_to_uint16(&buf[2]);
    reg_quantity = modbus_reg_to_uint16(&buf[4]);
    /*find and starting register*/
    reg_found = modbus_slave_find_register(slave, addr_start + 1, &reg_now);
    if (!reg_found) {
        modbus_slave_handle_rtu_exception(slave, buf, 0x02);
        return 0x02;
    }
    /*copy registers*/
    while (1) {
        if (copied == reg_quantity * 2) {
            break;
        }
        if (reg_now == NULL
            || (reg_now->index - 1) * 2 != copied + addr_start * 2
            || copied > reg_quantity * 2) {
            modbus_slave_handle_rtu_exception(slave, buf, 0x03);
            return 0x03;
        }
        memcpy(&buf[copied + 3], reg_now->data, reg_now->size * 2);
        copied += reg_now->size * 2;
        reg_now = reg_now->next;
    }
    buf[2] = copied;
    crc16 = modbus_crc16(buf, copied + 3);
    memcpy(&buf[copied + 3], &crc16, 2);
    if (slave->on_reply != NULL) {
        slave->on_reply(slave, buf, copied + 3 + 2);
    }
    return 0;
}

static int modbus_slave_handle_rtu_fc10(modbus_slave_t *slave, uint8_t *buf) {
    int reg_found;
    uint8_t byte_count, copied = 0;
    uint16_t addr_start, reg_quantity, crc16;
    modbus_register_t *reg_now;
    /*extract info from buffer*/
    addr_start = modbus_reg_to_uint16(&buf[2]);
    reg_quantity = modbus_reg_to_uint16(&buf[4]);
    byte_count = buf[6];
    /*check starting address*/
    reg_found = modbus_slave_find_register(slave, addr_start + 1, &reg_now);
    if (!reg_found) {
        modbus_slave_handle_rtu_exception(slave, buf, 0x02);
        return 0x02;
    }
    /*do on write callback and copy value to register*/
    while (1) {
        if (copied == byte_count) {
            break;
        }
        if (reg_quantity * 2 != byte_count
            || reg_now == NULL
            || (reg_now->index - 1) * 2 != copied + addr_start * 2
            || byte_count < copied + reg_now->size) {
            modbus_slave_handle_rtu_exception(slave, buf, 0x03);
            return 0x03;
        }
        if (reg_now->on_write != NULL) {
            if (reg_now->on_write(reg_now, &buf[copied + 7]) < 0) {
                /*internal error*/
                modbus_slave_handle_rtu_exception(slave, buf, 0x04);
                return 0x04;
            }
            copied += reg_now->size * 2;
            reg_now = reg_now->next;
        } else {
            modbus_slave_handle_rtu_exception(slave, buf, 0x01);
            return 0x01;
        }
    }
    /*success*/
    crc16 = modbus_crc16(buf, 6);
    memcpy(&buf[6], &crc16, 2);
    if (slave->on_reply != NULL) {
        slave->on_reply(slave, buf, 8);
    }
    return 0;
}

int modbus_slave_handle_rtu(modbus_slave_t *slave, uint8_t *buf, uint16_t len) {
    uint16_t crc16;
    /*check data integrity*/
    if (len < 8) return 2;
    /*check id*/
    if (buf[0] != slave->id) return 1;
    /*check crc*/
    crc16 = modbus_crc16(buf, len - 2);
    if (0 != memcmp(&buf[len - 2], &crc16, 2)) return 2;
    /*handle request*/
    switch (buf[1]) {
        case 0x03: {
            /*read holding registers*/
            modbus_slave_handle_rtu_fc03(slave, buf);
            break;
        }
        case 0x10: {
            /*write multiple registers*/
            modbus_slave_handle_rtu_fc10(slave, buf);
            break;
        }
        default: {
            /*function code not supported*/
            modbus_slave_handle_rtu_exception(slave, buf, 0x01);
        }
    }
    return 0;
}
