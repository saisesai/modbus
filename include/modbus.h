#ifndef MODBUS_H
#define MODBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Converts a Modbus register format buffer to uint16 format.
 *
 * This function converts a Modbus register format buffer, where the value is
 * encoded in little-endian byte order, to uint16 format by performing a byte swap.
 * The resulting value is returned as a uint16_t.
 *
 * @param buf Pointer to the buffer containing the Modbus register format.
 * @return Returns the converted value as a uint16_t.
 */
uint16_t modbus_reg_to_uint16(const uint8_t *buf);

/**
 * @brief Converts a uint16 value to Modbus register format buffer.
 *
 * This function converts a uint16_t value to Modbus register format by encoding
 * it in little-endian byte order and writing the result to the specified buffer.
 *
 * @param uint16 The uint16_t value to be converted.
 * @param buf Pointer to the buffer to store the Modbus register format.
 */
void modbus_uint16_to_reg(uint16_t uint16, uint8_t *buf);

/**
 * @brief Byte-swaps a 32-bit floating-point value encoded in a buffer.
 *
 * This function takes a buffer containing a 32-bit floating-point value encoded
 * in little-endian byte order and performs a byte swap to convert it to big-endian
 * byte order. The resulting value is returned as a float.
 *
 * @param buf Pointer to the buffer containing the encoded value.
 * @return Returns the byte-swapped floating-point value as a float.
 */
float modbus_f32_byte_swap(const uint8_t *buf);

/**
 * @brief Modbus register structure.
 */
struct modbus_register_s;

/**
 * @brief Typedef for modbus register.
 */
typedef struct modbus_register_s modbus_register_t;

/**
 * @brief Modbus register read or write callback prototype.
 * @param reg Pointer to the Modbus register.
 * @param buf Pointer to the data.
 * @return Return value indicating the result of the callback.
 *         A negative value indicates an internal error.
 */
typedef int (*modbus_register_rw_cb)(modbus_register_t *reg, const uint8_t *buf);

/**
 * @brief Modbus register chain list node.
 */
struct modbus_register_s {
    uint16_t index; /**< Starting from 1. */
    uint16_t size; /**< In half word (2 bytes). */
    uint8_t *data; /**< Data pointer. */
    modbus_register_rw_cb on_read; /**< Read callback. Return a negative value to indicate internal error. */
    modbus_register_rw_cb on_write; /**< Write callback. Return a negative value to indicate internal error. */
    struct modbus_register_s *next; /**< Pointer to the next register in the chain. */
};

/**
 * @brief This function is used to initialize a Modbus register chain list node structure.
 * @param reg Pointer to the Modbus register chain list node to be initialized.
 * @return Returns 0 on success, or a negative value if an error occurred.
 */
int modbus_register_init(modbus_register_t *reg);

/**
 * @brief Modbus slave structure.
 */
struct modbus_slave_s;

/**
 * @brief Typedef for modbus slave.
 */
typedef struct modbus_slave_s modbus_slave_t;

/**
 * @brief Modbus receive/reply callback function prototype.
 * @param slave Pointer to the Modbus slave.
 * @param buf Pointer to the data.
 * @param len Length of the data.
 * @return Returns a value indicating the result of the callback.
 *         A negative value indicates an error.
 */
typedef int (*modbus_slave_rw_cb)(modbus_slave_t *slave, uint8_t *buf, uint16_t len);

/**
 * @brief Modbus slave structure.
 */
struct modbus_slave_s {
    uint8_t id; /**< Slave ID. */
    modbus_register_t register_entry; /**< Register chain list entry. */
    uint16_t register_len; /**< Length of the register chan list. */
    modbus_slave_rw_cb on_read; /**< Callback function for slave receive. */
    modbus_slave_rw_cb on_write; /**< Callback function for slave reply. */
};

/**
 * @brief Initializes a Modbus slave structure.
 * @param slave Pointer to the Modbus slave structure to be initialized.
 * @return Returns 0 on success, or a negative value if an error occurred.
 */
int modbus_slave_init(modbus_slave_t *slave);

/**
 * @brief Adds a register to a Modbus slave.
 * @param slave Pointer to the Modbus slave.
 * @param reg Pointer to the register to be added.
 * @return Returns 0 on success, or a negative value if an error occurred.
 */
int modbus_slave_add_register(modbus_slave_t *slave, modbus_register_t *reg);

/**
 * @brief Removes a register from a Modbus slave.
 * @param slave Pointer to the Modbus slave.
 * @param reg Pointer to the register to be removed.
 * @return Returns 0 on success, or a negative value if an error occurred.
 */
int modbus_slave_remove_register(modbus_slave_t *slave, modbus_register_t *reg);

/**
 * @brief This function is used to handle incoming RTU data for a Modbus slave.
 * @param slave Pointer to the Modbus slave.
 * @param buf Pointer to the data buffer.
 * @param len Length of the data in Bytes.
 * @return Returns the result of the handling:
 *         -  0: OK
 *         -  1: Function not supported
 *         -  2: Invalid data address
 *         -  3: Invalid data value
 *         -  4: Internal error
 *         - -1: ADU is not intended for this device
 *         - -2: ADU is not valid (too short or wrong CRC)
 */
int modbus_slave_handle_rtu(modbus_slave_t *slave, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /*MODBUS_H*/
