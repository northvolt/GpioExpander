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


//--------------------------------------------------------------------------------------------------
/**
 * Configure a  GPIO as an input with no pullup/pulldown or an output which is initially low
 */
//--------------------------------------------------------------------------------------------------
static void ConfigureGpio
(
    int gpioDefine,                          ///< GPIO to configure.  Should be one of the
                                             ///< definitions from mangoh_gpioExpander.api
    mangoh_gpioExpander_PinMode_t direction  ///< Configure as input or output

)
{
    if (direction == MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT)
    {
        // Set the output to low before we configure it as output
        LE_FATAL_IF(
            mangoh_gpioExpander_Output(gpioDefine, MANGOH_GPIOEXPANDER_LEVEL_LOW) != LE_OK,
            "Couldn't set output on GPIO with ID=%d",
            gpioDefine);
    }
    else
    { // input
        // An external pullup is on the board so there is no need to use the pullup of the expander
        LE_FATAL_IF(
            mangoh_gpioExpander_SetPullUpDown(
                gpioDefine, MANGOH_GPIOEXPANDER_PULLUPDOWN_TYPE_OFF) != LE_OK,
            "Couldn't set pullup/pulldown on GPIO with ID=%d",
            gpioDefine);
    }
    LE_FATAL_IF(
        mangoh_gpioExpander_SetPolarity(gpioDefine, MANGOH_GPIOEXPANDER_POLARITY_NORMAL) != LE_OK,
        "Couldn't set normal polarity on GPIO with ID=%d",
        gpioDefine);
    LE_FATAL_IF(
        mangoh_gpioExpander_SetDirectionMode(gpioDefine, direction) != LE_OK,
        "Couldn't set direction on GPIO with ID=%d",
        gpioDefine);
}

//--------------------------------------------------------------------------------------------------
/**
 * Checks the level of the card detect GPIO for IoT slots 0, 1, 2
 *
 * @return
 *      A bit mask where bitX is for IoT slotX.  A 1 means an IoT card is present.
 */
//--------------------------------------------------------------------------------------------------
static uint8_t PollCardDetectInputs
(
    void
)
{
    uint8_t detectedMask = 0;
    int inputReading;

    inputReading = mangoh_gpioExpander_Input(MANGOH_GPIOEXPANDER_PIN_CARD_DETECT_IOT0);
    LE_FATAL_IF(inputReading == LE_FAULT, "Failed to read IoT slot 0 card detect input");
    detectedMask |= (inputReading == MANGOH_GPIOEXPANDER_LEVEL_HIGH ? 1 : 0) << 0;

    inputReading = mangoh_gpioExpander_Input(MANGOH_GPIOEXPANDER_PIN_CARD_DETECT_IOT1);
    LE_FATAL_IF(inputReading == LE_FAULT, "Failed to read IoT slot 1 card detect input");
    detectedMask |= (inputReading == MANGOH_GPIOEXPANDER_LEVEL_HIGH ? 1 : 0) << 1;

    inputReading = mangoh_gpioExpander_Input(MANGOH_GPIOEXPANDER_PIN_CARD_DETECT_IOT2);
    LE_FATAL_IF(inputReading == LE_FAULT, "Failed to read IoT slot 2 card detect input");
    detectedMask |= (inputReading == MANGOH_GPIOEXPANDER_LEVEL_HIGH ? 1 : 0) << 2;

    return detectedMask;
}

//--------------------------------------------------------------------------------------------------
/**
 * Turn on or off the IoT card detect LEDs
 */
//--------------------------------------------------------------------------------------------------
static void SetCardDetectLeds
(
    bool enIot0, ///< Enable slot0 card detect LED
    bool enIot1, ///< Enable slot1 card detect LED
    bool enIot2  ///< Enable slot2 card detect LED
)
{
    LE_FATAL_IF(
        mangoh_gpioExpander_Output(
            MANGOH_GPIOEXPANDER_PIN_LED_CARD_DETECT_IOT0,
            enIot0 ? MANGOH_GPIOEXPANDER_LEVEL_HIGH : MANGOH_GPIOEXPANDER_LEVEL_LOW) != LE_OK,
        "Failed to control LED_CARD_DETECT_IOT0");
    LE_FATAL_IF(
        mangoh_gpioExpander_Output(
            MANGOH_GPIOEXPANDER_PIN_LED_CARD_DETECT_IOT1,
            enIot1 ? MANGOH_GPIOEXPANDER_LEVEL_HIGH : MANGOH_GPIOEXPANDER_LEVEL_LOW) != LE_OK,
        "Failed to control LED_CARD_DETECT_IOT1");
    LE_FATAL_IF(
        mangoh_gpioExpander_Output(
            MANGOH_GPIOEXPANDER_PIN_LED_CARD_DETECT_IOT2,
            enIot2 ? MANGOH_GPIOEXPANDER_LEVEL_HIGH : MANGOH_GPIOEXPANDER_LEVEL_LOW) != LE_OK,
        "Failed to control LED_CARD_DETECT_IOT2");
}

COMPONENT_INIT
{
    LE_INFO(
        "This sample app demonstrates GPIO expander functionality by turning on and off the IoT "
        "slot LEDs as a module is inserted or removed\n");

    // Configure card detect pins as inputs
    ConfigureGpio(MANGOH_GPIOEXPANDER_PIN_CARD_DETECT_IOT0, MANGOH_GPIOEXPANDER_PIN_MODE_INPUT);
    ConfigureGpio(MANGOH_GPIOEXPANDER_PIN_CARD_DETECT_IOT1, MANGOH_GPIOEXPANDER_PIN_MODE_INPUT);
    ConfigureGpio(MANGOH_GPIOEXPANDER_PIN_CARD_DETECT_IOT2, MANGOH_GPIOEXPANDER_PIN_MODE_INPUT);

    // Configure card detect LED pins as outputs
    ConfigureGpio(
        MANGOH_GPIOEXPANDER_PIN_LED_CARD_DETECT_IOT0, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT);
    ConfigureGpio(
        MANGOH_GPIOEXPANDER_PIN_LED_CARD_DETECT_IOT1, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT);
    ConfigureGpio(
        MANGOH_GPIOEXPANDER_PIN_LED_CARD_DETECT_IOT2, MANGOH_GPIOEXPANDER_PIN_MODE_OUTPUT);

    while (true)
    {
        const uint8_t slotsConnectedMask = PollCardDetectInputs();
        LE_INFO("IoT slotsConnectedMask is currently 0x%x", slotsConnectedMask);
        SetCardDetectLeds(
            ((slotsConnectedMask >> 0) & 0x1) == 1,
            ((slotsConnectedMask >> 1) & 0x1) == 1,
            ((slotsConnectedMask >> 2) & 0x1) == 1);
        sleep(1); // Wait 1 second before polling again
    }
}
