/**
 * @file
 *
 * Provide a utility function for setting the i2c address on a bus.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 */

#include "legato.h"
#include "i2cSwitch.h"
// On yocto the file has a different non-standard name.
#ifdef LEGATO_EMBEDDED
#include <linux/i2c-dev-user.h>
#else
#include <linux/i2c-dev.h>
#endif


//--------------------------------------------------------------------------------------------------
/**
 * Get the file handle for the given I2C bus and configure it for access to the given I2C address.
 *
 * @return
 *      - LE_FAULT on failure
 *      - A filehandle to the I2C bus
 *
 * @note
 *      Copied here from gpioExpanderUtils.c.  This function is only required in here because of
 *      the access to the I2C switch (which doesn't belong in here anyway).  This should be removed
 *      later.  Though perhaps this function belongs in a central I2C library since it seems like
 *      it will be required anywhere I2C is used.
 */
//--------------------------------------------------------------------------------------------------
static int I2cAccessBusAddr
(
    uint8_t i2cBus,
    uint8_t i2cAddr
)
{
    const size_t filenameSize = 32;
    char filename[filenameSize];

    snprintf(filename, filenameSize, "/dev/i2c/%d", i2cBus);

    LE_DEBUG("Opening I2C bus: '%s'", filename);
    int fd = open(filename, O_RDWR);
    if (fd < 0 && (errno == ENOENT || errno == ENOTDIR))
    {
        snprintf(filename, filenameSize, "/dev/i2c-%d", i2cBus);
        LE_DEBUG("Opening I2C bus: '%s'", filename);
        fd = open(filename, O_RDWR);
    }

    if (fd < 0)
    {
        if (errno == ENOENT)
        {
            LE_ERROR(
                "Could not open file /dev/i2c-%d or /dev/i2c/%d: %s\n",
                i2cBus,
                i2cBus,
                strerror(ENOENT));
        }
        else
        {
            LE_ERROR("Could not open file %s': %s\n", filename, strerror(errno));
        }

        return LE_FAULT;
    }

    if (ioctl(fd, I2C_SLAVE_FORCE, i2cAddr) < 0)
    {
        LE_ERROR("Could not set address to 0x%02x: %s\n", i2cAddr, strerror(errno));
        return LE_FAULT;
    }

    return fd;
}

le_result_t ConfigureI2cSwitch(
    uint8_t i2cBus,
    uint8_t i2cAddr,
    uint8_t portConfiguration
)
{
    const int i2cdev_fd = I2cAccessBusAddr(i2cBus, i2cAddr);
    if (i2cdev_fd == LE_FAULT)
    {
        LE_ERROR("Failed to open I2C bus %d", i2cBus);
        return LE_FAULT;
    }

    if (i2c_smbus_write_byte(i2cdev_fd, portConfiguration) == -1)
    {
        LE_ERROR("Failed to write to the I2C switch");
        close(i2cdev_fd);
        return LE_FAULT;
    }

    close(i2cdev_fd);
    return LE_OK;
}
