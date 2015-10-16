/**
 * @file gpio.c
 *
 * Prototype GPIO Expander API interface.
 * The GPIO API implementation of GPIO Expander 1, 2, 3 on MangOH board
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 */

#include <linux/i2c-dev-user.h>
#include "legato.h"
#include "interfaces.h"

//--------------------------------------------------------------------------------------------------
/**
 * i2c slave address on MangOH platform
 */
//--------------------------------------------------------------------------------------------------
#define I2C_SX1509_GPIO_EXPANDER1_ADDR      0x3E
#define I2C_SX1509_GPIO_EXPANDER2_ADDR      0x3F
#define I2C_SX1509_GPIO_EXPANDER3_ADDR      0x70
#define I2C_SWITCH_PCA9548A_ADDR            0x71

//--------------------------------------------------------------------------------------------------
/**
 * gpio expander number on MangOH platform
 */
//--------------------------------------------------------------------------------------------------
enum GpioExpanderNum {
    GPIO_EXPANDER_1 = 1,
    GPIO_EXPANDER_2,
    GPIO_EXPANDER_3,
    MAX_GPIO_EXPANDER_NR = GPIO_EXPANDER_3
};

//--------------------------------------------------------------------------------------------------
/**
 * The SX1509 GPIO Register Address
 */
//--------------------------------------------------------------------------------------------------
typedef enum {
    SX1509_GPIO_RegInputDisableB,
    SX1509_GPIO_RegPullUpB = 0x06,
    SX1509_GPIO_RegPullUpA = 0x07,
    SX1509_GPIO_RegPullDownB = 0x08,
    SX1509_GPIO_RegPullDownA = 0x09,
    SX1509_GPIO_RegOpenDrainB = 0x0A,
    SX1509_GPIO_RegOpenDrainA = 0x0B,
    SX1509_GPIO_RegPolarityB = 0x0C,
    SX1509_GPIO_RegPolarityA = 0x0D,
    SX1509_GPIO_RegDirB = 0x0E,
    SX1509_GPIO_RegDirA = 0x0F,
    SX1509_GPIO_RegDataB = 0x10,
    SX1509_GPIO_RegDataA = 0x11,
    SX1509_GPIO_RegInterruptMaskB = 0x12,
    SX1509_GPIO_RegInterruptMaskA = 0x13,
    SX1509_GPIO_RegSenseHighB = 0x14,
    SX1509_GPIO_RegSenseLowB = 0x15,
    SX1509_GPIO_RegSenseHighA = 0x16,
    SX1509_GPIO_RegSenseLowA = 0x17
}
Sc1509GpioExpanderRegs_t;

//--------------------------------------------------------------------------------------------------
/**
 * The action to disable or enable of GPIO register pullup, pulldown.
 */
//--------------------------------------------------------------------------------------------------
typedef enum {
    GPIO_PULLUP_DOWN_DISABLE,    ///< GPIO pullup/pulldown disable
    GPIO_PULLUP_DOWN_ENABLE      ///< GPIO pullup/pulldown enable
}
GpioPullUpDownAction;

//--------------------------------------------------------------------------------------------------
/**
 * The struct of Expander object
 */
//--------------------------------------------------------------------------------------------------
struct mangoh_gpioExpander_Gpio {
    uint8_t module;                               ///< Expander#1, #2, #3
    mangoh_gpioExpander_PinMode_t mode;           ///< Output, Input or Interrupt mode
    mangoh_gpioExpander_ActiveType_t level;       ///< Active High or Low
    mangoh_gpioExpander_PullUpDownType_t pud;     ///< Pullup or Pulldown type
    uint8_t pinNum;                               ///< pin number
    uint8_t i2cAddr;                              ///< the i2c address, -1 means not I2C 
    uint8_t i2cBus;                               ///< i2c bus
    uint8_t bank;                                 ///< I/O pins: Bank A(1) or B(0)
};

//--------------------------------------------------------------------------------------------------
/**
 * The gpio expander module structure for Expander 1, 2, and 3 object.
 */
//--------------------------------------------------------------------------------------------------
static struct mangoh_gpioExpander_Gpio GpioObjModules[MAX_GPIO_EXPANDER_NR];

//--------------------------------------------------------------------------------------------------
/**
 * Get i2c bus file descriptor and set i2c slave address.
 *
 * If parameter 'i2cAddr' is not 0, not necessary to control i2c slave address.
 *
 * @return
 * - LE_FAULT          In case of failure
 * - A positive value  The file descriptor of i2c bus
 */
//--------------------------------------------------------------------------------------------------
static int SetI2cBusAddr
(
    int i2cBus,        ///< [IN] i2c bus number.
    int i2cAddr        ///< [IN] i2c slave device address.
)
{
    int fd;
    size_t size;
    char filename[32];

    size = sizeof(filename);
    snprintf(filename, size, "/dev/i2c/%d", i2cBus);

    LE_DEBUG("Open I2C Bus at, '%s'", filename);
    fd = open(filename, O_RDWR);
    if (fd < 0 && (errno == ENOENT || errno == ENOTDIR)) {
        snprintf(filename, size, "/dev/i2c-%d", i2cBus);
        LE_DEBUG("Try open I2C Bus at, '%s'", filename);
        fd = open(filename, O_RDWR);
    }

    if (fd < 0) {
        if (errno == ENOENT) {
            LE_ERROR("Could not open file /dev/i2c-%d or /dev/i2c/%d: %s\n", i2cBus, i2cBus, strerror(ENOENT));
        } else {
            LE_ERROR("Could not open file %s': %s\n", filename, strerror(errno));
        }

        return LE_FAULT;
    }

    if (i2cAddr) {
        if (ioctl(fd, I2C_SLAVE_FORCE, i2cAddr) < 0) {
            LE_ERROR("Could not set address to 0x%02x: %s\n",i2cAddr, strerror(errno));
            return LE_FAULT;
        }
    }

    return fd;
}

//--------------------------------------------------------------------------------------------------
/**
 * Write a value across the i2c bus.
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t I2cSetAddrValue
(
    int i2cBus,        ///< [IN] i2c bus number.
    int i2cAddr,       ///< [IN] i2c slave device address.
    int regAddr,       ///< [IN] i2c register address.
    int dataValue      ///< [IN] i2c register data value to be written.
)
{
    int i2cdev_fd;

    if ((i2cBus < 0) || (i2cAddr < 0) || (regAddr < 0) || (regAddr > 0xff)) {
        LE_ERROR("%d:%x:%x\n", i2cBus, i2cAddr, regAddr);
        return LE_FAULT;
    }

    i2cdev_fd = SetI2cBusAddr(i2cBus, i2cAddr);
    if (i2cdev_fd == LE_FAULT)
    {
        LE_ERROR("failed to open i2cbus %d\n", i2cBus);
        return LE_FAULT;
    }

    if (i2c_smbus_write_byte_data(i2cdev_fd, regAddr, dataValue) < 0)
    {
        LE_ERROR("failed to write i2c data\n");
        close(i2cdev_fd);
        return LE_FAULT;
    }

    close(i2cdev_fd);
    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Read a value across the i2c bus.
 *
 * @return
 * - LE_FAULT          The function failed.
 * - Positive value    The function succeeded with return value from i2c register
 */
//--------------------------------------------------------------------------------------------------
static int I2cGetAddrValue
(
    int i2cBus,        ///< [IN] i2c bus number.
    int i2cAddr,       ///< [IN] i2c slave device address.
    int regAddr        ///< [IN] i2c register address.
)
{
    int val;
    int i2cdev_fd;

    if ((i2cBus < 0) || (i2cAddr < 0) || (regAddr < 0) || (regAddr > 0xff)) {
        LE_ERROR("%d:%x:%x\n", i2cBus, i2cAddr, regAddr);
        return LE_FAULT;
    }

    i2cdev_fd = SetI2cBusAddr(i2cBus, i2cAddr);
    if (i2cdev_fd == LE_FAULT) {
        LE_ERROR("failed to open i2cbus %d\n", i2cBus);
        return LE_FAULT;
    }
    val = i2c_smbus_read_byte_data(i2cdev_fd, regAddr);
    if (val < 0)
    {
        LE_ERROR("failed to read i2c data\n");
        close(i2cdev_fd);
        return LE_FAULT;
    }

    close(i2cdev_fd);
    return val;
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the pin number for this IO and convert it to the bit index in the appropriate bank register.
 *
 * There are two banks of 8 I/O pins: A and B.
 * The lowest bit of the register address (daddr) is the bank select: 0 = B, 1 = A.
 * Bank B: IO[15-8] or Bank A: IO[7-0]
 *
 * @return
 * - A positive value  The bit index at bank A or B
 */
//--------------------------------------------------------------------------------------------------
static uint8_t PinNumToBankIndex(uint8_t num)
{
    uint8_t index;

    index = num < 8 ? num : (num - 8);
    return index;
}

//--------------------------------------------------------------------------------------------------
/**
 * Based on expander object GPIO number, get register bank value
 *
 * @return
 * - LE_FAULT          The function failed.
 * - A boolean value   The bit value of GPIO pin number
 */
//--------------------------------------------------------------------------------------------------
static int GetRegVal
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,    ///< [IN] gpio reference
    uint8_t regAddr                              ///< [IN] i2c register address
)
{
    uint8_t regNew;
    bool bit;

    regNew = (regAddr | gpioRefPtr->bank);

    uint8_t index = PinNumToBankIndex(gpioRefPtr->pinNum);
    uint8_t pinMask = 1 << index;

    // Read the current register values.
    int regVal = I2cGetAddrValue(gpioRefPtr->i2cBus, gpioRefPtr->i2cAddr, regNew);
    if (regVal == LE_FAULT)
    {
        LE_ERROR("I2c get addr 0x%x value failure", gpioRefPtr->i2cAddr);
        return LE_FAULT;
    }

    regVal &= pinMask;
    bit = regVal >> index;

    LE_DEBUG("\tRegister Addr 0x%x: 0x%x...bit:%d", regNew, regVal, bit);

    return bit;
}

//--------------------------------------------------------------------------------------------------
/**
 * Based on expander object GPIO number, set register bank value
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t SetRegVal
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,    ///< [IN] gpio reference
    uint8_t regAddr,                             ///< [IN] i2c register address
    uint8_t value                                ///< [IN] the data value of i2c register address
)
{
    uint8_t regNew;

    regNew = (regAddr | gpioRefPtr->bank);

    uint8_t index = PinNumToBankIndex(gpioRefPtr->pinNum);
    uint8_t pinMask = 1 << index;

    // Read the current register values.
    int regVal = I2cGetAddrValue(gpioRefPtr->i2cBus, gpioRefPtr->i2cAddr, regNew);
    if (regVal == LE_FAULT)
    {
        LE_ERROR("I2c get addr 0x%x value failure", gpioRefPtr->i2cAddr);
        return LE_FAULT;
    }

    regVal &= ~pinMask;
    regVal |= (value << index);
    if (I2cSetAddrValue(gpioRefPtr->i2cBus, gpioRefPtr->i2cAddr, regNew, regVal) != LE_OK)
    {
        LE_ERROR("I2c set address value 0x%x failure", regAddr);
        return LE_FAULT;
    }

    LE_DEBUG("\tRegister Addr 0x%x: 0x%x...", regNew, regVal);

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * setup GPIO Direction INPUT or OUTPUT mode.
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_SetDirectionMode
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,    ///< [IN] gpio reference
    mangoh_gpioExpander_PinMode_t mode           ///< [IN] gpio direction input/output mode 
)
{
    LE_DEBUG("mode:%s", (mode == MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT) ? "Output": "Input");
    if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegDirB, mode) != LE_OK)
    {
        LE_ERROR("Set mode %d failure", mode);
        return LE_FAULT;
    }

    LE_DEBUG("Succesfully setup direction mode");
    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Request GPIO object from GPIO function module and GPIO pin number.
 *
 * @return
 * - A positive value  this will return a newly created gpio reference.
 * - NULL if request failed to provide valid expander and pin number.
 */
//--------------------------------------------------------------------------------------------------
mangoh_gpioExpander_GpioRef_t mangoh_gpioExpander_Request
(
    uint8_t module,
        ///< [IN]
        ///< The module must be GPIO Expander 1, 2, or 3

    uint8_t pinNum
        ///< [IN]
        ///< GPIO pin number of use
)
{
    if (module > MAX_GPIO_EXPANDER_NR || module < GPIO_EXPANDER_1)
    {
        LE_KILL_CLIENT("Supplied bad (%d) GPIO Expander number", module);
        return NULL;
    }

    if (pinNum > 15)
    {
        LE_KILL_CLIENT("Supplied bad (%d) GPIO Pin number", pinNum);
        return NULL;
    }

    mangoh_gpioExpander_GpioRef_t gpioRefPtr = &GpioObjModules[module - 1];

    gpioRefPtr->module = module;
    gpioRefPtr->pinNum = pinNum;

    if (module == GPIO_EXPANDER_1)
    {
        gpioRefPtr->i2cAddr = I2C_SX1509_GPIO_EXPANDER1_ADDR;
    }
    else if(module == GPIO_EXPANDER_2)
    {
        gpioRefPtr->i2cAddr = I2C_SX1509_GPIO_EXPANDER2_ADDR;
    }
    else if(module == GPIO_EXPANDER_3)
    {
        gpioRefPtr->i2cAddr = I2C_SX1509_GPIO_EXPANDER3_ADDR;
    }
    gpioRefPtr->i2cBus = 0;

    gpioRefPtr->bank = 1; // 1 = A
    if (pinNum > 7)
    {
        gpioRefPtr->bank = 0; // 0 = B
    }

    LE_DEBUG("expander#:%d gpio pin:%d, i2cAddr:0x%x", gpioRefPtr->module, gpioRefPtr->pinNum, gpioRefPtr->i2cAddr);

    return gpioRefPtr;
}

//--------------------------------------------------------------------------------------------------
/**
 * Release GPIO Module object
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_Release
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr    ///< [IN] gpio module object reference
)
{
    gpioRefPtr->pinNum = -1;

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * setup GPIO pullup or pulldown disable/enable.
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_SetPullUpDown
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,    ///< [IN] gpio module object reference
    mangoh_gpioExpander_PullUpDownType_t pud     ///< [IN] pull up, pull down type
)
{
    if (pud == MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_OFF) {
        LE_DEBUG("Pulldown/pullup type OFF");
        if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegPullUpB, GPIO_PULLUP_DOWN_DISABLE) != LE_OK)
        {
            LE_ERROR("Set pullup disable failure");
            return LE_FAULT;
        }
        if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegPullDownB, GPIO_PULLUP_DOWN_DISABLE) != LE_OK)
        {
            LE_ERROR("Set pulldown disable failure");
            return LE_FAULT;
        }
    } else if (pud == MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_DOWN) {
        LE_DEBUG("Pulldown type enable");
        if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegPullUpB, GPIO_PULLUP_DOWN_DISABLE) != LE_OK)
        {
            LE_ERROR("Set pullup disable failure");
            return LE_FAULT;
        }
        if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegPullDownB, GPIO_PULLUP_DOWN_ENABLE) != LE_OK)
        {
            LE_ERROR("Set pulldown enable failure");
            return LE_FAULT;
        }
    } else if (pud == MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_UP) {
        LE_DEBUG("Pullup type enable");
        if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegPullUpB, GPIO_PULLUP_DOWN_ENABLE) != LE_OK)
        {
            LE_ERROR("Set pullup enable failure");
            return LE_FAULT;
        }
        if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegPullDownB, GPIO_PULLUP_DOWN_DISABLE) != LE_OK)
        {
            LE_ERROR("Set pulldown disable failure");
            return LE_FAULT;
        }
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * setup GPIO OpenDrain.
 *
 * Enables open drain operation for each output-configured IO.
 *
 * Output pins can be driven in two different modes:
 * - Regular push-pull operation: A transistor connects to high, and a transistor connects to low 
 *   (only one is operated at a time)
 * - Open drain operation:  A transistor connects to low and nothing else
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_SetOpenDrain
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,            ///< [IN] gpio module object reference
    mangoh_gpioExpander_OpenDrainOperation_t drainOp     ///< [IN] The operation of GPIO open drain
)
{
    LE_DEBUG("enable open drain:%s", (drainOp==1) ? "Open drain operation": "Regular push-pull operation");
    if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegOpenDrainB, drainOp) != LE_OK)
    {
        LE_ERROR("Set opendrain %d failure", drainOp);
        return LE_FAULT;
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * setup GPIO polarity.
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_SetPolarity
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,           ///< [IN] gpio module object reference
    mangoh_gpioExpander_ActiveType_t level              ///< [IN] Active-high or active-low
)
{
    LE_DEBUG("level:%s", (level==1) ? "high": "low");
    if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegPolarityB, level) != LE_OK)
    {
        LE_ERROR("Set polarity %d failure", level);
        return LE_FAULT;
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * write value to GPIO output mode, low or high
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_Output
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,           ///< [IN] gpio module object reference
    mangoh_gpioExpander_ActiveType_t level              ///< [IN] Active-high or active-low
)
{
    LE_DEBUG("active:%s", (level==1) ? "high": "low");
    if (SetRegVal(gpioRefPtr, SX1509_GPIO_RegDataB, level) != LE_OK)
    {
        LE_ERROR("Set output %d failure", level);
        return LE_FAULT;
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * read value from GPIO input mode.
 *
 * @return
 *      An active type, the status of pin: HIGH or LOW
 */
//--------------------------------------------------------------------------------------------------
mangoh_gpioExpander_ActiveType_t mangoh_gpioExpander_Input
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr            ///< [IN] gpio module object reference
)
{
    mangoh_gpioExpander_ActiveType_t type;

    type = (mangoh_gpioExpander_ActiveType_t)GetRegVal(gpioRefPtr, SX1509_GPIO_RegDataB);
    LE_DEBUG("read active type:%s", (type==1) ? "high": "low");

    return type;
}

//--------------------------------------------------------------------------------------------------
/**
 * Rising or Falling of Edge sensitivity
 *
 * @return
 * - LE_FAULT          The function failed.
 * - LE_OK             The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_SetEdgeSense
(
    mangoh_gpioExpander_GpioRef_t gpioRefPtr,           ///< [IN] gpio module object reference
        ///< [IN]
        ///< GPIO module object reference

    mangoh_gpioExpander_EdgeSensivityMode_t edge        ///< [IN] The mode of GPIO Edge Sensivity.
        ///< [IN]
)
{
    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * The place where the component starts up.  All initialization happens here.
 */
//--------------------------------------------------------------------------------------------------
COMPONENT_INIT
{
    int file;
    int i2c_addr;
    int daddr;
    int data;
    int i;
    const int i2cbus = 0;

    file = SetI2cBusAddr(i2cbus, 0);
    if (file == LE_FAULT)
    {
        LE_FATAL("Failed to set i2c bus address");
    }

    // Enable the I2C switch.
    LE_DEBUG("Enabling PCA9548A I2C switch...");
    i2c_addr = I2C_SWITCH_PCA9548A_ADDR;
    daddr = 0xf9; //1111 1111 (enable all I2C channels)
    data = -1;
    if (I2cSetAddrValue(i2cbus, i2c_addr, daddr, data) == LE_FAULT)
    {
        LE_FATAL("Failed to enable PCA9548A I2C switch");
    }
    else
    {
        LE_DEBUG("PCA9548A I2C switch enabled.");
    }
    // wait to make sure the i2c switch has been enabled
    sleep(1);

    // -1: initialzie the pin number of gpio module, the module is not initialized
    for (i = 0; i < MAX_GPIO_EXPANDER_NR; i++)
        GpioObjModules[i].pinNum = -1;

    close(file);
}
