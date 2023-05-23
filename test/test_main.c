#include "test_slave_read_reg.h"

int main(void) {
    test_slave_read_reg_invalid_data();
    test_slave_read_reg_not_this_device();
    test_slave_read_reg_wrong_address();
}
