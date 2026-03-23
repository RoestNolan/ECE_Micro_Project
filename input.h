#ifndef INPUT_H
#define INPUT_H

/****************INPUT READING METHODS*******************/

/***** Get raw 12-bit ADC value from potentiometer channel *****/
int read_adc_value(void);

/***** Convert potentiometer reading into lock number from 0-39 *****/
int adc_to_number(int adc_value);

/***** Return TRUE while KEY0 is physically pressed (active-low input) *****/
int read_confirm_button_raw(void);

/***** Return TRUE once when KEY0 changes from released to pressed *****/
int confirm_button_pressed_edge(void);

/***** Return TRUE when SW0 is ON for change-code mode selection *****/
int read_change_mode_switch(void);

#endif
