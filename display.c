#include "display.h"
#include "board.h"

static int lockout_flash_state = 0;

/***** Display number on 7-seg *****/
void display_number(int number)
{
    int tens;
    int ones;
    unsigned char blank = 0x00;
    unsigned char hex0;
    unsigned char hex1;

    if (number < 0) number = 0;
    if (number > 99) number = 99;

    tens = number / 10;
    ones = number % 10;

    hex0 = seg7[ones];
    hex1 = (number < 10) ? blank : seg7[tens];

    *hex0_hex3_ptr = pack4(blank, blank, hex1, hex0);
}

/***** Display selected value and current digit slot while user enters code *****/
void display_entry_status(int digit_index, int selected_value)
{
    unsigned char blank = 0x00;
    unsigned char slot = blank;
    unsigned char hex0;
    unsigned char hex1;
    int tens;
    int ones;

    if (selected_value < 0) selected_value = 0;
    if (selected_value > 99) selected_value = 99;

    if (digit_index >= 0 && digit_index <= 2)
        slot = seg7[digit_index + 1];

    tens = selected_value / 10;
    ones = selected_value % 10;

    hex0 = seg7[ones];
    hex1 = (selected_value < 10) ? blank : seg7[tens];

    *hex0_hex3_ptr = pack4(blank, slot, hex1, hex0);
}

/***** Display remaining attempts on HEX0 after wrong code entry *****/
void display_attempts_remaining(int attempts_remaining)
{
    unsigned char blank = 0x00;

    if (attempts_remaining < 0) attempts_remaining = 0;
    if (attempts_remaining > 9) attempts_remaining = 9;

    *hex0_hex3_ptr = pack4(blank, blank, blank, seg7[attempts_remaining]);
}

/***** Display simple success message using upper HEX digits and lower digits cleared *****/
void display_success_message(void)
{
    unsigned char blank = 0x00;
    unsigned char s_like = 0x6D;
    unsigned char u_like = 0x3E;
    unsigned char c_like = 0x39;

    *hex0_hex3_ptr = pack4(blank, blank, blank, blank);
    *hex4_hex5_ptr = pack2(u_like, s_like);

    /* Place second C on HEX3 for a simple SUC-style indication */
    *hex0_hex3_ptr = pack4(blank, c_like, blank, blank);
}

/***** Display lockout message using two upper HEX digits *****/
void display_lockout_message(void)
{
    unsigned char l_like = 0x38;
    unsigned char o_like = 0x3F;

    *hex0_hex3_ptr = pack4(0x00, 0x00, 0x00, 0x00);
    *hex4_hex5_ptr = pack2(o_like, l_like);
}

/***** Turn all LED bar outputs OFF *****/
void clear_led_bar(void)
{
    *ledr_ptr = 0x000;
}

/***** Turn all LED bar outputs ON to indicate success *****/
void show_success_led_pattern(void)
{
    *ledr_ptr = 0x3FF;
}

/***** Turn on lower LEDs only to indicate incorrect code *****/
void show_error_led_pattern(void)
{
    *ledr_ptr = 0x00F;
}

/***** Show alternating mid-range LEDs to indicate change-code mode *****/
void show_change_mode_led_pattern(void)
{
    *ledr_ptr = 0x0F0;
}

/***** Flash full LED bar on and off during timed lockout *****/
void flash_lockout_led_pattern(void)
{
    if (lockout_flash_state == 0) {
        *ledr_ptr = 0x3FF;
        lockout_flash_state = 1;
    }
    else {
        *ledr_ptr = 0x000;
        lockout_flash_state = 0;
    }
}
