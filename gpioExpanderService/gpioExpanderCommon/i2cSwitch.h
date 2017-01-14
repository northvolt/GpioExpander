/**
 * @file
 *
 * Provide a utility function for setting the i2c address on a bus.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 */
#ifndef I2C_SWITCH_H
#define I2C_SWITCH_H

#include "legato.h"

LE_SHARED le_result_t ConfigureI2cSwitch(
    uint8_t i2cBus,
    uint8_t i2cAddr,
    uint8_t portConfiguration
);

#endif // I2C_SWITCH_H
