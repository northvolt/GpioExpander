#!/usr/bin/env python

import jinja2

code_block_template = """
// GPIO expander #{{expander}} GPIO {{gpio}}
le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_SetInput
(
    mangoh_gpioExp{{expander}}Pin{{gpio}}_Polarity_t polarity
)
{
    return gpioExpander_SetInput(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}], polarity);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_SetPushPullOutput
(
    mangoh_gpioExp{{expander}}Pin{{gpio}}_Polarity_t polarity,
    bool value
)
{
    return gpioExpander_SetPushPullOutput(
        &expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}], polarity, value);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_SetTriStateOutput
(
    mangoh_gpioExp{{expander}}Pin{{gpio}}_Polarity_t polarity
)
{
    return gpioExpander_SetTriStateOutput(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}], polarity);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_SetOpenDrainOutput
(
    mangoh_gpioExp{{expander}}Pin{{gpio}}_Polarity_t polarity,
    bool value
)
{
    return gpioExpander_SetOpenDrainOutput(
        &expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}], polarity, value);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_EnablePullUp
(
    void
)
{
    return gpioExpander_EnablePullUp(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_EnablePullDown
(
    void
)
{
    return gpioExpander_EnablePullDown(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_DisableResistors
(
    void
)
{
    return gpioExpander_DisableResistors(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_Activate
(
    void
)
{
    return gpioExpander_Activate(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_Deactivate
(
    void
)
{
    return gpioExpander_Deactivate(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_SetHighZ
(
    void
)
{
    return gpioExpander_SetHighZ(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

bool mangoh_gpioExp{{expander}}Pin{{gpio}}_Read
(
    void
)
{
    return gpioExpander_Read(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_SetEdgeSense
(
    mangoh_gpioExp{{expander}}Pin{{gpio}}_Edge_t trigger
)
{
    return gpioExpander_SetEdgeSense(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}], trigger);
}

mangoh_gpioExp{{expander}}Pin{{gpio}}_Edge_t mangoh_gpioExp{{expander}}Pin{{gpio}}_GetEdgeSense
(
    void
)
{
    return gpioExpander_GetEdgeSense(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

le_result_t mangoh_gpioExp{{expander}}Pin{{gpio}}_DisableEdgeSense
(
    void
)
{
    return gpioExpander_DisableEdgeSense(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

bool mangoh_gpioExp{{expander}}Pin{{gpio}}_IsOutput
(
    void
)
{
    return gpioExpander_IsOutput(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

bool mangoh_gpioExp{{expander}}Pin{{gpio}}_IsInput
(
    void
)
{
    return gpioExpander_IsInput(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

mangoh_gpioExp{{expander}}Pin{{gpio}}_Polarity_t mangoh_gpioExp{{expander}}Pin{{gpio}}_GetPolarity
(
    void
)
{
    return gpioExpander_GetPolarity(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

bool mangoh_gpioExp{{expander}}Pin{{gpio}}_IsActive
(
    void
)
{
    return gpioExpander_IsActive(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

mangoh_gpioExp{{expander}}Pin{{gpio}}_PullUpDown_t mangoh_gpioExp{{expander}}Pin{{gpio}}_GetPullUpDown
(
    void
)
{
    return gpioExpander_GetPullUpDown(&expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}]);
}

mangoh_gpioExp{{expander}}Pin{{gpio}}_ChangeEventHandlerRef_t mangoh_gpioExp{{expander}}Pin{{gpio}}_AddChangeEventHandler
(
    mangoh_gpioExp{{expander}}Pin{{gpio}}_Edge_t trigger,
    mangoh_gpioExp{{expander}}Pin{{gpio}}_ChangeCallbackFunc_t handlerPtr,
    void *contextPtr,
    int32_t sampleMs
)
{
    return (mangoh_gpioExp{{expander}}Pin{{gpio}}_ChangeEventHandlerRef_t)gpioExpander_AddChangeEventHandler(
        &expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}],
        &handlerRecords[EXPANDER_{{expander}}_INDEX][{{gpio}}],
        trigger,
        handlerPtr,
        contextPtr,
        sampleMs);
}

void mangoh_gpioExp{{expander}}Pin{{gpio}}_RemoveChangeEventHandler
(
    mangoh_gpioExp{{expander}}Pin{{gpio}}_ChangeEventHandlerRef_t ref
)
{
    gpioExpander_RemoveChangeEventHandler(
        &expanderPinSpecs[EXPANDER_{{expander}}_INDEX][{{gpio}}],
        &handlerRecords[EXPANDER_{{expander}}_INDEX][{{gpio}}],
        (gpioExpander_ChangeCallbackRef_t)ref);
}
"""

def generate():
    for expander in range(1, 4):
        for gpio in range(16):
            t = jinja2.Template(code_block_template)
            print(t.render(expander=expander, gpio=gpio))

if __name__ == '__main__':
    generate()

