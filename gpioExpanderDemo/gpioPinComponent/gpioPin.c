/**
 * @file gpioPin.c
 *
 * This is sample Legato GPIO app by using mangoh_gpioExpander.api on mangOH project.
 *
 * It includes functions to enable/disable IOT card LED1, LED2, LED3, and LED4.
 * Those IOT LEDs will be on or off one by one
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless, Inc. Use of this work is subject to license.
 */

/* Legato Framework */
#include "legato.h"
#include "interfaces.h"

// -------------------------------------------------------------------------------------------------
/**
 *  Control GPIO Expander IOT LED on/off with seconds interval.
 *
 *  User can modify the interval value as like.
 */
// -------------------------------------------------------------------------------------------------
#define LED_ONOFF_TIME_INTERVAL    5

// -------------------------------------------------------------------------------------------------
/**
 *  Control GPIO Expander IOT LED1, LED2, LED3, LED4 one by one with seconds interval.
 *
 *  User can modify the interval value  as like.
 */
// -------------------------------------------------------------------------------------------------
#define LED_TIME_INTERVAL    2

// -------------------------------------------------------------------------------------------------
/**
 *  Function to control GPIO Expander #1, #2, #3 by using major GPIO api.
 *
 */
// -------------------------------------------------------------------------------------------------
static void GpioPinCtl
(
    uint8_t module,                               ///< [IN] The GPIO Expander number, 1, 2, or 3.
    uint32_t gpioNum,                             ///< [IN] The GPIO number.
    mangoh_gpioExpander_PinMode_t direction,      ///< [IN] Direction mode, OUTPUT or INPUT.
    mangoh_gpioExpander_ActiveType_t level        ///< [IN] Active HIGH or Active LOW.
)
{
    mangoh_gpioExpander_GpioRef_t gpioRef;
    int val;

    gpioRef = mangoh_gpioExpander_Request(module, gpioNum);
    if (gpioRef == NULL)
    {
        LE_ERROR("Gpio Expander request module:%d gpio:%dfailed\n", module, gpioNum);
        return;
    }
    mangoh_gpioExpander_SetDirectionMode(gpioRef, direction);
    mangoh_gpioExpander_SetPolarity(gpioRef, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_HIGH);
    mangoh_gpioExpander_Output(gpioRef, level);

    // e.g. Input
    val = mangoh_gpioExpander_Input(gpioRef);
    LE_INFO("Input val = %d\n", val);

    //e.g. pullup, pulldown off
    mangoh_gpioExpander_SetPullUpDown(gpioRef, MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_OFF);

    //e.g. Release object
    mangoh_gpioExpander_Release(gpioRef);
}

// -------------------------------------------------------------------------------------------------
/**
 *  Turn on LED1, LED2, LED3, LED4 IOT card in IOT2 slot.
 */
// -------------------------------------------------------------------------------------------------
static void GpioPinIot2CardLedOn()
{
    // LED1 On
    GpioPinCtl(2, 7, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_HIGH);
    sleep(LED_TIME_INTERVAL);
    // LED2 On
    GpioPinCtl(2, 6, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_HIGH);
    sleep(LED_TIME_INTERVAL);
    // LED3 On
    GpioPinCtl(2, 8, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_HIGH);
    sleep(LED_TIME_INTERVAL);
    // LED4 On
    GpioPinCtl(3, 7, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_HIGH);
}

// -------------------------------------------------------------------------------------------------
/**
 *  Turn off LED1, LED2, LED3, LED4 IOT card in IOT2 slot.
 */
// -------------------------------------------------------------------------------------------------
static void GpioPinIot2CardLedOff()
{
    // LED1 Off
    GpioPinCtl(2, 7, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_LOW);
    sleep(LED_TIME_INTERVAL);
    // LED2 Off
    GpioPinCtl(2, 6, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_LOW);
    sleep(LED_TIME_INTERVAL);
    // LED3 Off
    GpioPinCtl(2, 8, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_LOW);
    sleep(LED_TIME_INTERVAL);
    // LED4 Off
    GpioPinCtl(3, 7, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT, MANGOH_GPIOEXPANDER_ACTIVE_TYPE_LOW);
}

COMPONENT_INIT
{
    LE_INFO("This is sample Legato GPIO Expander app by using mangoh_gpioExpander.api on mangOH project\n");

    while(1) {
        LE_INFO("GPIO Expander#2 IOT2 GPIO All LEDs Off\n");
        GpioPinIot2CardLedOff();
        sleep(LED_ONOFF_TIME_INTERVAL);

        LE_INFO("GPIO Expander#2 IOT2 GPIO All LEDs ON\n");
        GpioPinIot2CardLedOn();
        sleep(LED_ONOFF_TIME_INTERVAL);
    }
    return;
}
