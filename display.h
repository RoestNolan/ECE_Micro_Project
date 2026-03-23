#ifndef DISPLAY_H
#define DISPLAY_H

/****************DISPLAY/OUTPUT METHODS*******************/

/***** Display a decimal number from 0-99 on HEX1-HEX0 *****/
void display_number(int number);

/***** Display current entry slot and selected number during code entry *****/
void display_entry_status(int digit_index, int selected_value);

/***** Display attempts remaining after incorrect code entry *****/
void display_attempts_remaining(int attempts_remaining);

/***** Display success indication on 7-seg after correct code or code change *****/
void display_success_message(void);

/***** Display lockout indication on 7-seg during tamper timeout *****/
void display_lockout_message(void);

/***** Set LED bar to all OFF *****/
void clear_led_bar(void);

/***** Display LED pattern for successful unlock *****/
void show_success_led_pattern(void);

/***** Display LED pattern for incorrect code entry *****/
void show_error_led_pattern(void);

/***** Display steady LED pattern for change-code mode *****/
void show_change_mode_led_pattern(void);

/***** Flash LED bar repeatedly during lockout state *****/
void flash_lockout_led_pattern(void);

#endif
