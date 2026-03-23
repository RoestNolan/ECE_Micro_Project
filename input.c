#include "input.h"
#include "board.h"

/***** Get Raw ADC Value *****/
int read_adc_value(void)
{
    return (*adc_ch0) & ADC_DATA_MASK;
}

/***** Convert POT to range from 0-39 *****/
int adc_to_number(int adc_value)
{
    int num = (adc_value * 40) / 4096;
    if (adc_value == 4095) num = 39;
    if (num < 0) return 0;
    if (num > 39) return 39;
    return num;
}

/***** Return TRUE while KEY0 is being pressed (buttons are active-low) *****/
int read_confirm_button_raw(void)
{
    if (((*key_ptr) & 0x1) == 0)
        return 1;
    else
        return 0;
}

/***** Detect single button-press event so held button is not counted repeatedly *****/
int confirm_button_pressed_edge(void)
{
    static int last_state = 0;
    int current_state = read_confirm_button_raw();
    int pressed = (current_state == 1 && last_state == 0);
    last_state = current_state;
    return pressed;
}

/***** Return TRUE when SW0 is enabled for change-code mode *****/
int read_change_mode_switch(void)
{
    if (((*sw_ptr) & 0x1) != 0)
        return 1;
    else
        return 0;
}
