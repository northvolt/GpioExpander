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
 * GPIO expanders on MangOH platform
 */
//--------------------------------------------------------------------------------------------------
typedef enum {
    GPIO_EXPANDER_1,
    GPIO_EXPANDER_2,
    GPIO_EXPANDER_3,
    NUM_OF_GPIO_EXPANDERS
} GpioExpanderId_t;

typedef enum {
    // GPIO pins [15:8]
    GPIO_EXPANDER_BANK_B,
    // GPIO pins [7:0]
    GPIO_EXPANDER_BANK_A,
} GpioExpanderBank_t;

//--------------------------------------------------------------------------------------------------
/**
 * The SX1509 GPIO Register Address - See datasheet for descriptions
 */
//--------------------------------------------------------------------------------------------------
typedef enum {
    // Device and IO Banks
    SX1509_REG_INPUT_DISABLE_B               = 0x00,
    SX1509_REG_INPUT_DISABLE_A               = 0x01,
    SX1509_REG_LONG_SLEW_B                   = 0x02,
    SX1509_REG_LONG_SLEW_A                   = 0x03,
    SX1509_REG_LOW_DRIVE_B                   = 0x04,
    SX1509_REG_LOW_DRIVE_A                   = 0x05,
    SX1509_REG_PULL_UP_B                     = 0x06,
    SX1509_REG_PULL_UP_A                     = 0x07,
    SX1509_REG_PULL_DOWN_B                   = 0x08,
    SX1509_REG_PULL_DOWN_A                   = 0x09,
    SX1509_REG_OPEN_DRAIN_B                  = 0x0A,
    SX1509_REG_OPEN_DRAIN_A                  = 0x0B,
    SX1509_REG_POLARITY_B                    = 0x0C,
    SX1509_REG_POLARITY_A                    = 0x0D,
    SX1509_REG_DIR_B                         = 0x0E,
    SX1509_REG_DIR_A                         = 0x0F,
    SX1509_REG_DATA_B                        = 0x10,
    SX1509_REG_DATA_A                        = 0x11,
    SX1509_REG_INTERRUPT_MASK_B              = 0x12,
    SX1509_REG_INTERRUPT_MASK_A              = 0x13,
    SX1509_REG_SENSE_HIGH_B                  = 0x14,
    SX1509_REG_SENSE_LOW_B                   = 0x15,
    SX1509_REG_SENSE_HIGH_A                  = 0x16,
    SX1509_REG_SENSE_LOW_A                   = 0x17,
    SX1509_REG_INTERRUPT_SOURCE_B            = 0x18,
    SX1509_REG_INTERRUPT_SOURCE_A            = 0x19,
    SX1509_REG_EVENT_STATUS_B                = 0x1A,
    SX1509_REG_EVENT_STATUS_A                = 0x1B,
    SX1509_REG_LEVEL_SHIFTER_1               = 0x1C,
    SX1509_REG_LEVEL_SHIFTER_2               = 0x1D,
    SX1509_REG_CLOCK                         = 0x1E,
    SX1509_REG_MISC                          = 0x1F,
    SX1509_REG_LED_DRIVER_ENABLE_B           = 0x20,
    SX1509_REG_LED_DRIVER_ENABLE_A           = 0x21,
    // Debounce and Keypad Engine
    SX1509_REG_DEBOUNCE_CONFIG               = 0x22,
    SX1509_REG_DEBOUNCE_ENABLE_B             = 0x23,
    SX1509_REG_DEBOUNCE_ENABLE_A             = 0x24,
    SX1509_REG_KEY_CONFIG_1                  = 0x25,
    SX1509_REG_KEY_CONFIG_2                  = 0x26,
    SX1509_REG_KEY_DATA_1                    = 0x27,
    SX1509_REG_KEY_DATA_2                    = 0x28,
    // LED Driver (PWM, blinking, breathing)
    SX1509_REG_T_ON_0                        = 0x29,
    SX1509_REG_I_ON_0                        = 0x2A,
    SX1509_REG_OFF_0                         = 0x2B,
    SX1509_REG_T_ON_1                        = 0x2C,
    SX1509_REG_I_ON_1                        = 0x2D,
    SX1509_REG_OFF_1                         = 0x2E,
    SX1509_REG_T_ON_2                        = 0x2F,
    SX1509_REG_I_ON_2                        = 0x30,
    SX1509_REG_OFF_2                         = 0x31,
    SX1509_REG_T_ON_3                        = 0x32,
    SX1509_REG_I_ON_3                        = 0x33,
    SX1509_REG_OFF_3                         = 0x34,
    SX1509_REG_T_ON_4                        = 0x35,
    SX1509_REG_I_ON_4                        = 0x36,
    SX1509_REG_OFF_4                         = 0x37,
    SX1509_REG_T_RISE_4                      = 0x38,
    SX1509_REG_T_FALL_4                      = 0x39,
    SX1509_REG_T_ON_5                        = 0x3A,
    SX1509_REG_I_ON_5                        = 0x3B,
    SX1509_REG_OFF_5                         = 0x3C,
    SX1509_REG_T_RISE_5                      = 0x3D,
    SX1509_REG_T_FALL_5                      = 0x3E,
    SX1509_REG_T_ON_6                        = 0x3F,
    SX1509_REG_I_ON_6                        = 0x40,
    SX1509_REG_OFF_6                         = 0x41,
    SX1509_REG_T_RISE_6                      = 0x42,
    SX1509_REG_T_FALL_6                      = 0x43,
    SX1509_REG_T_ON_7                        = 0x44,
    SX1509_REG_I_ON_7                        = 0x45,
    SX1509_REG_OFF_7                         = 0x46,
    SX1509_REG_T_RISE_7                      = 0x47,
    SX1509_REG_T_FALL_7                      = 0x48,
    SX1509_REG_T_ON_8                        = 0x49,
    SX1509_REG_I_ON_8                        = 0x4A,
    SX1509_REG_OFF_8                         = 0x4B,
    SX1509_REG_T_ON_9                        = 0x4C,
    SX1509_REG_I_ON_9                        = 0x4D,
    SX1509_REG_OFF_9                         = 0x4E,
    SX1509_REG_T_ON_10                       = 0x4F,
    SX1509_REG_I_ON_10                       = 0x50,
    SX1509_REG_OFF_10                        = 0x51,
    SX1509_REG_T_ON_11                       = 0x52,
    SX1509_REG_I_ON_11                       = 0x53,
    SX1509_REG_OFF_11                        = 0x54,
    SX1509_REG_T_ON_12                       = 0x55,
    SX1509_REG_I_ON_12                       = 0x56,
    SX1509_REG_OFF_12                        = 0x57,
    SX1509_REG_T_RISE_12                     = 0x58,
    SX1509_REG_T_FALL_12                     = 0x59,
    SX1509_REG_T_ON_13                       = 0x5A,
    SX1509_REG_I_ON_13                       = 0x5B,
    SX1509_REG_OFF_13                        = 0x5C,
    SX1509_REG_T_RISE_13                     = 0x5D,
    SX1509_REG_T_FALL_13                     = 0x5E,
    SX1509_REG_T_ON_14                       = 0x5F,
    SX1509_REG_I_ON_14                       = 0x60,
    SX1509_REG_OFF_14                        = 0x61,
    SX1509_REG_T_RISE_14                     = 0x62,
    SX1509_REG_T_FALL_14                     = 0x63,
    SX1509_REG_T_ON_15                       = 0x64,
    SX1509_REG_I_ON_15                       = 0x65,
    SX1509_REG_OFF_15                        = 0x66,
    SX1509_REG_T_RISE_15                     = 0x67,
    SX1509_REG_T_FALL_15                     = 0x68,
    // Miscellaneous
    SX1509_REG_HIGH_INPUT_B                  = 0x69,
    SX1509_REG_HIGH_INPUT_A                  = 0x6A,
    // Software Reset
    SX1509_REG_RESET                         = 0x7D,
    // Test (not to be written)
    SX1509_REG_TEST_1                        = 0x7E,
    SX1509_REG_TEST_2                        = 0x7F,
} Sc1509GpioExpanderReg_t;

//--------------------------------------------------------------------------------------------------
/**
 * The action to disable or enable of GPIO register pullup, pulldown.
 *
 * @note
 *      Do not change the value of these enum definitions because they correspond to register
 *      values defined in the SX1509 datasheet.
 */
//--------------------------------------------------------------------------------------------------
typedef enum {
    GPIO_PULLUP_DOWN_DISABLE = 0, ///< GPIO pullup/pulldown disable
    GPIO_PULLUP_DOWN_ENABLE  = 1  ///< GPIO pullup/pulldown enable
} GpioPullUpDownAction;

//--------------------------------------------------------------------------------------------------
/**
 * The location specification of a GPIO on a GPIO expander
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    uint8_t i2cBus;
    uint8_t i2cAddr;
    GpioExpanderBank_t bank;
    uint8_t offsetInBank;
} GpioSpec_t;

//--------------------------------------------------------------------------------------------------
/**
 * Initializes the values in the given GpioSpec_t based upon the provided define.
 *
 * @return
 *      - LE_OK
 *      - LE_BAD_PARAMETER if define does not correspond to a GPIO
 *
 * @note
 *      Client will be killed if define is invalid.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t IntitializeSpecFromGpioDefine(
    int define,           ///< [IN] PIN_* definition value from mangoh_gpioExpander.api
    GpioSpec_t* gpioSpec  ///< [OUT] GPIO specification that will be written
)
{
    if (define < 0 || define >= (NUM_OF_GPIO_EXPANDERS * 16))
    {
        LE_KILL_CLIENT("GPIO expander definition %d is invalid.", define);
        return LE_BAD_PARAMETER;
    }

    // Currently the I2C bus is always 0, but this may change in the future
    gpioSpec->i2cBus = 0;

    // There are 16 GPIOs per expander
    const uint8_t expander = define / 16;
    gpioSpec->i2cAddr =
        (expander == GPIO_EXPANDER_1) ?
            I2C_SX1509_GPIO_EXPANDER1_ADDR :
            (expander == GPIO_EXPANDER_2) ?
                I2C_SX1509_GPIO_EXPANDER2_ADDR :
                I2C_SX1509_GPIO_EXPANDER3_ADDR;

    // The lowest 4 bits represent the pin number
    const uint8_t pinOnExpander = define & 0xF;

    // If the most significant bit of the pin is set, then this is bank b
    const uint8_t bankPin = (1 << 3);
    gpioSpec->bank = (pinOnExpander & bankPin) ? GPIO_EXPANDER_BANK_B : GPIO_EXPANDER_BANK_A;

    gpioSpec->offsetInBank = pinOnExpander & (~bankPin);

    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Get i2c bus file descriptor and set i2c slave address.
 *
 * TODO: what does this comment line below mean?
 * If parameter 'i2cAddr' is not 0, not necessary to control i2c slave address.
 *
 * @return
 * - LE_FAULT          In case of failure
 * - A positive value  The file descriptor of i2c bus
 */
//--------------------------------------------------------------------------------------------------
static int SetI2cBusAddr
(
    uint8_t i2cBus, ///< [IN] i2c bus number
    uint8_t i2cAddr ///< [IN] i2c slave device address
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
            LE_ERROR(
                "Could not open file /dev/i2c-%d or /dev/i2c/%d: %s\n",
                i2cBus,
                i2cBus,
                strerror(ENOENT));
        } else {
            LE_ERROR("Could not open file %s': %s\n", filename, strerror(errno));
        }

        return LE_FAULT;
    }

    if (i2cAddr) {
        if (ioctl(fd, I2C_SLAVE_FORCE, i2cAddr) < 0) {
            LE_ERROR("Could not set address to 0x%02x: %s\n", i2cAddr, strerror(errno));
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
    uint8_t i2cBus,   ///< [IN] i2c bus number
    uint8_t i2cAddr,  ///< [IN] i2c slave device address
    uint8_t regAddr,  ///< [IN] i2c register address
    uint8_t dataValue ///< [IN] i2c register data value to be written
)
{
    int i2cdev_fd;

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
    uint8_t i2cBus,  ///< [IN] i2c bus number.
    uint8_t i2cAddr, ///< [IN] i2c slave device address.
    uint8_t regAddr  ///< [IN] i2c register address.
)
{
    int result;
    int i2cdev_fd;

    i2cdev_fd = SetI2cBusAddr(i2cBus, i2cAddr);
    if (i2cdev_fd == LE_FAULT) {
        LE_ERROR("failed to open i2cbus %d\n", i2cBus);
        return LE_FAULT;
    }
    result = i2c_smbus_read_byte_data(i2cdev_fd, regAddr);
    if (result < 0)
    {
        result = LE_FAULT;
        LE_ERROR("failed to read i2c data\n");
    }

    close(i2cdev_fd);
    return result;
}

//--------------------------------------------------------------------------------------------------
/**
 * Performs a read-modify-write to write a single bit within the specified register.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
static le_result_t SetRegisterBit
(
    uint8_t i2cBus,    ///< [IN] i2c bus number.
    uint8_t i2cAddr,   ///< [IN] i2c slave device address.
    uint8_t regAddr,   ///< [IN] i2c register address.
    uint8_t bitOffset, ///< [IN] position of bit to set
    bool bitSet        ///< [IN] if true, set the bit, else clear the bit
)
{
    int regVal = I2cGetAddrValue(i2cBus, i2cAddr, regAddr);
    le_result_t result;
    if (regVal < 0)
    {
        result = LE_FAULT;
    }
    else
    {
        const uint8_t bitMask = (1 << bitOffset);
        regVal &= ~bitMask;
        if (bitSet)
        {
            regVal |= bitMask;
        }

        result = I2cSetAddrValue(i2cBus, i2cAddr, regAddr, regVal);
        if (result != LE_OK)
        {
            result = LE_FAULT;
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a single bit within the specified register.
 *
 * @return
 *      - 0: The bit is clear
 *      - 1: The bit is set
 *      - LE_FAULT: Could not read the bit
 */
//--------------------------------------------------------------------------------------------------
static int GetRegisterBit
(
    uint8_t i2cBus,    ///< [IN] i2c bus number.
    uint8_t i2cAddr,   ///< [IN] i2c slave device address.
    uint8_t regAddr,   ///< [IN] i2c register address.
    uint8_t bitOffset  ///< [IN] position of the bit to get
)
{
    int regVal = I2cGetAddrValue(i2cBus, i2cAddr, regAddr);
    int result = (regVal < 0) ? LE_FAULT : ((regVal >> bitOffset) & 0x1);

    return result;
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
    int gpioExpanderPinDefine,          ///< [IN] GPIO to set direction on
    mangoh_gpioExpander_PinMode_t mode  ///< [IN] GPIO direction input/output mode
)
{
    LE_DEBUG("direction: %s", (mode == MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT) ? "Output": "Input");
    GpioSpec_t gpioSpec;
    if (IntitializeSpecFromGpioDefine(gpioExpanderPinDefine, &gpioSpec) != LE_OK)
    {
        return LE_FAULT;
    }
    le_result_t r = SetRegisterBit(
        gpioSpec.i2cBus,
        gpioSpec.i2cAddr,
        (gpioSpec.bank == GPIO_EXPANDER_BANK_A) ? SX1509_REG_DIR_A : SX1509_REG_DIR_B,
        gpioSpec.offsetInBank,
        (mode == MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT) ? 0 : 1);

    if (r != LE_OK)
    {
        LE_ERROR("Set mode %d failure", mode);
        return LE_FAULT;
    }

    LE_DEBUG("Succesfully setup direction mode");
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
    int gpioExpanderPinDefine,                ///< [IN] GPIO to set pull direction on
    mangoh_gpioExpander_PullUpDownType_t pud  ///< [IN] pull up, pull down type
)
{
    GpioPullUpDownAction pullUpVal = GPIO_PULLUP_DOWN_DISABLE;
    GpioPullUpDownAction pullDownVal = GPIO_PULLUP_DOWN_DISABLE;
    switch (pud)
    {
        case MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_OFF:
            break;

        case MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_UP:
            pullUpVal = GPIO_PULLUP_DOWN_ENABLE;
            break;

        case MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_DOWN:
            pullDownVal = GPIO_PULLUP_DOWN_ENABLE;
            break;
    }

    GpioSpec_t gpioSpec;
    if (IntitializeSpecFromGpioDefine(gpioExpanderPinDefine, &gpioSpec) != LE_OK)
    {
        return LE_FAULT;
    }

    Sc1509GpioExpanderReg_t pullUpReg;
    Sc1509GpioExpanderReg_t pullDownReg;
    if (gpioSpec.bank == GPIO_EXPANDER_BANK_A)
    {
        pullUpReg   = SX1509_REG_PULL_UP_A;
        pullDownReg = SX1509_REG_PULL_DOWN_A;
    }
    else
    {
        pullUpReg   = SX1509_REG_PULL_UP_B;
        pullDownReg = SX1509_REG_PULL_DOWN_B;
    }
    const le_result_t pullUpResult = SetRegisterBit(
        gpioSpec.i2cBus,
        gpioSpec.i2cAddr,
        pullUpReg,
        gpioSpec.offsetInBank,
        pullUpVal);
    const le_result_t pullDownResult = SetRegisterBit(
        gpioSpec.i2cBus,
        gpioSpec.i2cAddr,
        pullDownReg,
        gpioSpec.offsetInBank,
        pullDownVal);

    if (pullUpResult != LE_OK || pullDownResult != LE_OK)
    {
        LE_ERROR("Failure configuring pullup/pulldown resistors.");
        return LE_FAULT;
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
    int gpioExpanderPinDefine,                  ///< [IN] GPIO to control open drain setting on
    mangoh_gpioExpander_OpenDrainOperation_t drainOp ///< [IN] The operation of GPIO open drain
)
{
    LE_DEBUG(
        "enable open drain: %s",
        (drainOp == MANGOH_GPIOEXPANDER_OPEN_DRAIN_OP) ?
            "Open drain operation" :
            "Regular push-pull operation");
    GpioSpec_t gpioSpec;
    if (IntitializeSpecFromGpioDefine(gpioExpanderPinDefine, &gpioSpec) != LE_OK)
    {
        return LE_FAULT;
    }
    le_result_t r = SetRegisterBit(
        gpioSpec.i2cBus,
        gpioSpec.i2cAddr,
        (gpioSpec.bank == GPIO_EXPANDER_BANK_A) ?
            SX1509_REG_OPEN_DRAIN_A : SX1509_REG_OPEN_DRAIN_B,
        gpioSpec.offsetInBank,
        (drainOp == MANGOH_GPIOEXPANDER_OPEN_DRAIN_OP) ? 1 : 0);
    if (r != LE_OK)
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
    int gpioExpanderPinDefine,               ///< [IN] GPIO to set polarity on
    mangoh_gpioExpander_Polarity_t polarity  ///< [IN] Normal or inverted
)
{
    LE_DEBUG(
        "polarity: %s", (polarity == MANGOH_GPIOEXPANDER_POLARITY_NORMAL) ? "normal" : "inverted");
    GpioSpec_t gpioSpec;
    if (IntitializeSpecFromGpioDefine(gpioExpanderPinDefine, &gpioSpec) != LE_OK)
    {
        return LE_FAULT;
    }
    // For the SX1509, 0 means normal polarity and 1 means inverted
    le_result_t r = SetRegisterBit(
        gpioSpec.i2cBus,
        gpioSpec.i2cAddr,
        (gpioSpec.bank == GPIO_EXPANDER_BANK_A) ? SX1509_REG_POLARITY_A : SX1509_REG_POLARITY_B,
        gpioSpec.offsetInBank,
        (polarity == MANGOH_GPIOEXPANDER_POLARITY_NORMAL) ? 0 : 1);
    if (r != LE_OK)
    {
        LE_ERROR("Set polarity %d failure", polarity);
        return LE_FAULT;
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * write value to GPIO output mode, low or high
 *
 * @return
 *      - LE_FAULT on failure
 *      - LE_OK on success
 */
//--------------------------------------------------------------------------------------------------
le_result_t mangoh_gpioExpander_Output
(
    int gpioExpanderPinDefine,         ///< [IN] GPIO to set output value for
    mangoh_gpioExpander_Level_t level  ///< [IN] Output low or high
)
{
    LE_DEBUG("output: %s", (level == MANGOH_GPIOEXPANDER_LEVEL_HIGH) ? "high": "low");
    GpioSpec_t gpioSpec;
    if (IntitializeSpecFromGpioDefine(gpioExpanderPinDefine, &gpioSpec) != LE_OK)
    {
        return LE_FAULT;
    }
    le_result_t r = SetRegisterBit(
        gpioSpec.i2cBus,
        gpioSpec.i2cAddr,
        (gpioSpec.bank == GPIO_EXPANDER_BANK_A) ? SX1509_REG_DATA_A : SX1509_REG_DATA_B,
        gpioSpec.offsetInBank,
        (level == MANGOH_GPIOEXPANDER_LEVEL_LOW) ? 0 : 1);
    if (r != LE_OK)
    {
        LE_ERROR("Set output %d failure", level);
        return LE_FAULT;
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Read value from GPIO input.
 *
 * @return
 *      - A mangoh_gpioExpander_Level_t value
 *      - LE_FAULT
 */
//--------------------------------------------------------------------------------------------------
int mangoh_gpioExpander_Input
(
    int gpioExpanderPinDefine  ///< [IN] GPIO to get input value for
)
{
    GpioSpec_t gpioSpec;
    if (IntitializeSpecFromGpioDefine(gpioExpanderPinDefine, &gpioSpec) != LE_OK)
    {
        return LE_FAULT;
    }
    int r = GetRegisterBit(
        gpioSpec.i2cBus,
        gpioSpec.i2cAddr,
        (gpioSpec.bank == GPIO_EXPANDER_BANK_A) ? SX1509_REG_DATA_A : SX1509_REG_DATA_B,
        gpioSpec.offsetInBank);

    if (r < 0)
    {
        return LE_FAULT;
    }

    const mangoh_gpioExpander_Level_t level =
        (r == 0) ? MANGOH_GPIOEXPANDER_LEVEL_LOW : MANGOH_GPIOEXPANDER_LEVEL_HIGH;
    LE_DEBUG(
        "read input: %s", (level == MANGOH_GPIOEXPANDER_LEVEL_LOW) ? "low" : "high");

    return level;
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
    int gpioExpanderPinDefine,                  ///< [IN] GPIO to set edge sensitivity for
    mangoh_gpioExpander_EdgeSensitivityMode_t edge      ///< [IN] The mode of GPIO Edge Sensitivity
)
{
    // TODO: implement
    return LE_FAULT;
}

//--------------------------------------------------------------------------------------------------
/**
 * The place where the component starts up.  All initialization happens here.
 *
 * @todo
 *      This is not the right place to be controlling the I2C switch.  This needs to be managed
 *      centrally.
 */
//--------------------------------------------------------------------------------------------------
COMPONENT_INIT
{
    LE_DEBUG("Enabling PCA9548A I2C switch...");
    const int i2cBus = 0;
    const int i2cdev_fd = SetI2cBusAddr(i2cBus, I2C_SWITCH_PCA9548A_ADDR);
    LE_FATAL_IF(i2cdev_fd == LE_FAULT, "failed to open i2cbus %d\n", i2cBus);

    const uint8_t enableAllPorts = 0xff;
    LE_FATAL_IF(i2c_smbus_write_byte(i2cdev_fd, enableAllPorts) == -1, "failed to write i2c data");

    close(i2cdev_fd);
}
