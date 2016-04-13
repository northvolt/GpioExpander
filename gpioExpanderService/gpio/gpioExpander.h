//--------------------------------------------------------------------------------------------------
/**
 * @file
 *
 * This module contains functions which are the same as those specified by le_gpio.api except that
 * they take an additional gpioExpander_PinSpec_t parameter.  These functions exist so that the
 * implementation of le_gpio.api for the GPIO expander can be as compact as possible.  This is
 * important because each function in the API must be implemented for every GPIO on the expander.
 *
 * <HR>
 * 
 * Copyright (C) Sierra Wireless, Inc. Use of this work is subject to license.
 */
//--------------------------------------------------------------------------------------------------
#ifndef GPIO_EXPANDER_H
#define GPIO_EXPANDER_H

#include "legato.h"

//--------------------------------------------------------------------------------------------------
/**
 * Function pointer type definition for GPIO expander input interrupts.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*gpioExpander_ChangeCallbackFunc_t)
(
    bool state,       ///< The state of the GPIO after the event which triggered the interrupt.
                      ///  true means the GPIO is now active.
    void *contextPtr  ///< A pointer to data that was provided when the event handler was added.
);

//--------------------------------------------------------------------------------------------------
/**
 * The location specification of a GPIO on a GPIO expander
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    uint8_t i2cBus;  ///< I2C bus that the GPIO expander is on
    uint8_t i2cAddr; ///< I2C address of the GPIO expander
    uint8_t pin;     ///< GPIO being referenced.  Should be in the range of 0 to 15.
} gpioExpander_PinSpec_t;

//--------------------------------------------------------------------------------------------------
/**
 * Stores the registered event handler function pointer for a GPIO pin.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    gpioExpander_ChangeCallbackFunc_t handlerPtr; ///< Function to call when a change event occurs
    void *contextPtr;                             ///< Parameter to pass to the handler function
} gpioExpander_HandlerRecord_t;

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
typedef struct gpioExpander_ChangeCallback* gpioExpander_ChangeCallbackRef_t;

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 *
 * @note
 *      This enum must have the values in the same order as defined in le_gpio.api.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    GPIO_EXPANDER_ACTIVE_HIGH,
    GPIO_EXPANDER_ACTIVE_LOW,
} gpioExpander_Polarity_t;

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 *
 * @note
 *      This enum must have the values in the same order as defined in le_gpio.api.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    GPIO_EXPANDER_EDGE_NONE,
    GPIO_EXPANDER_EDGE_RISING,
    GPIO_EXPANDER_EDGE_FALLING,
    GPIO_EXPANDER_EDGE_BOTH,
} gpioExpander_Edge_t;

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 *
 * @note
 *      This enum must have the values in the same order as defined in le_gpio.api.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    GPIO_EXPANDER_PULL_OFF,
    GPIO_EXPANDER_PULL_DOWN,
    GPIO_EXPANDER_PULL_UP,
} gpioExpander_PullUpDown_t;


//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_SetInput
(
    const gpioExpander_PinSpec_t *gpioPin,
    gpioExpander_Polarity_t polarity
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_SetPushPullOutput
(
    const gpioExpander_PinSpec_t *gpioPin,
    gpioExpander_Polarity_t polarity,
    bool value
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_SetTriStateOutput
(
    const gpioExpander_PinSpec_t *gpioPin,
    gpioExpander_Polarity_t polarity
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_SetOpenDrainOutput
(
    const gpioExpander_PinSpec_t *gpioPin,
    gpioExpander_Polarity_t polarity,
    bool value
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_EnablePullUp
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_EnablePullDown
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_DisableResistors
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_Activate
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_Deactivate
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_SetHighZ
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
bool gpioExpander_Read
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
gpioExpander_ChangeCallbackRef_t gpioExpander_AddChangeEventHandler
(
    const gpioExpander_PinSpec_t *gpioPin,
    gpioExpander_HandlerRecord_t *handlerRecord,
    gpioExpander_Edge_t trigger,
    gpioExpander_ChangeCallbackFunc_t handlerPtr,
    void *contextPtr,
    int32_t sampleMs
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
void gpioExpander_RemoveChangeEventHandler
(
    const gpioExpander_PinSpec_t *gpioPin,
    gpioExpander_HandlerRecord_t *handlerRecord,
    gpioExpander_ChangeCallbackRef_t ref
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_SetEdgeSense
(
    const gpioExpander_PinSpec_t *gpioPin,
    gpioExpander_Edge_t trigger
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
gpioExpander_Edge_t gpioExpander_GetEdgeSense
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t gpioExpander_DisableEdgeSense
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
bool gpioExpander_IsOutput
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
bool gpioExpander_IsInput
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
gpioExpander_Polarity_t gpioExpander_GetPolarity
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
bool gpioExpander_IsActive
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Refer to le_gpio.api documentation.
 */
//--------------------------------------------------------------------------------------------------
gpioExpander_PullUpDown_t gpioExpander_GetPullUpDown
(
    const gpioExpander_PinSpec_t *gpioPin
);

//--------------------------------------------------------------------------------------------------
/**
 * Performs a software reset of the specified SX1509 GPIO expander
 *
 * Upon completion, the SX1509 will be in a state equivalent to if the POR pin had been asserted.
 */
//--------------------------------------------------------------------------------------------------
void gpioExpander_Reset
(
    uint8_t expanderI2cBus,
    uint8_t expanderI2cAddress
);

//--------------------------------------------------------------------------------------------------
/**
 * Implements a generic interrupt handler that checks for interrupts in the status register of the
 * GPIO expander and then calls the appropriate handler functions.
 */
//--------------------------------------------------------------------------------------------------
void gpioExpander_GenericInterruptHandler
(
    uint8_t expanderI2cBus,                       ///< I2C bus that the GPIO expander is on
    uint8_t expanderI2cAddress,                   ///< I2C address of the GPIO expander
    const gpioExpander_HandlerRecord_t *handlers  ///< An array of 16 handler records
);


#endif // GPIO_EXPANDER_H
