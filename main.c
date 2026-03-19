/*
 * Digital Combination Lock - ECE3375 Project Prototype
 *
 * Purpose:
 *   Software prototype for a digital combination lock using:
 *   - potentiometer/ADC input for value selection (0-40)
 *   - push button to confirm each digit
 *   - slide switch to enter "change code" mode after unlock
 *   - LEDs for success / error / lockout indication
 *   - 7-segment display for selected value / attempts remaining
 *
 * Notes:
 *   1) This file contains the FULL application logic.
 *   2) The hardware access functions are isolated in one section so you can
 *      replace them with your DE10-Standard / DE1-SoC simulator register code.
 *   3) If your repo already has drivers for ADC, KEYs, SWs, HEX, and LEDs,
 *      connect those functions here instead of rewriting the whole program.
 */

#include <stdint.h>
#include <stdbool.h>

/* =========================================================
   CONFIGURATION
   ========================================================= */

#define CODE_LENGTH          3
#define MAX_DIGIT_VALUE      40
#define MAX_ATTEMPTS         3

/* Adjust these based on loop speed on your platform.
 * This is a simple software timing approach for the prototype.
 */
#define LOCKOUT_TICKS        3000
#define SUCCESS_HOLD_TICKS    800
#define ERROR_HOLD_TICKS      600
#define BLINK_PERIOD_TICKS    120

/* =========================================================
   HARDWARE ABSTRACTION LAYER
   Replace these with your actual board/simulator functions
   ========================================================= */

/*
 * If you already have register definitions in your repo, replace the bodies
 * of the functions below and keep the rest of the program unchanged.
 *
 * Example sources you may already have:
 *   - ADC driver
 *   - KEY/button driver
 *   - SWITCH driver
 *   - HEX display output
 *   - LED output
 */

/* ---------- Optional memory-mapped addresses ----------
 * These are placeholders only.
 * Replace them ONLY if your platform actually uses these addresses.
 *
 * #define LEDR_BASE   ((volatile uint32_t *)0xFF200000)
 * #define SW_BASE     ((volatile uint32_t *)0xFF200040)
 * #define KEY_BASE    ((volatile uint32_t *)0xFF200050)
 * #define HEX3_HEX0   ((volatile uint32_t *)0xFF200020)
 * #define HEX5_HEX4   ((volatile uint32_t *)0xFF200030)
 * #define ADC_BASE    ((volatile uint32_t *)0xFF204000)
 */

/* Stub globals for logic testing without real hardware */
static uint16_t g_stub_adc_raw = 0;
static bool     g_stub_button_pressed = false;
static bool     g_stub_change_switch = false;

/* Write a bit pattern to the LEDs */
static void hw_write_leds(uint32_t pattern)
{
    /* Replace with board register write if available */
    (void)pattern;
    /* Example:
       *LEDR_BASE = pattern;
    */
}

/* Display a number 0-99 on two 7-segment digits.
 * For this project we mostly show 0-40, attempts remaining, or short codes.
 */
static void hw_display_number(int value);

/* Optional: display a 3-digit code entry status, for example selected number and slot */
static void hw_display_entry_status(int selected_value, int digit_index);

/* Read raw ADC value from potentiometer */
static uint16_t hw_read_adc_raw(void)
{
    /* Replace with actual ADC read */
    return g_stub_adc_raw;
}

/* Read push-button state (true while pressed) */
static bool hw_read_confirm_button(void)
{
    /* Replace with actual button read */
    return g_stub_button_pressed;
}

/* Read change-code switch state */
static bool hw_read_change_switch(void)
{
    /* Replace with actual switch read */
    return g_stub_change_switch;
}

/* Simple busy-wait tick hook.
 * On real hardware, you may leave this empty if the main loop is already slow enough,
 * or replace with a timer-based wait.
 */
static void hw_small_delay(void)
{
    volatile int i;
    for (i = 0; i < 2000; ++i) {
        /* busy wait */
    }
}

/* =========================================================
   7-SEG HELPERS
   ========================================================= */

static const uint8_t SEG7_MAP[10] = {
    /* 0-9, common active-low style often used on Altera/Intel boards */
    0x40, /* 0 */
    0x79, /* 1 */
    0x24, /* 2 */
    0x30, /* 3 */
    0x19, /* 4 */
    0x12, /* 5 */
    0x02, /* 6 */
    0x78, /* 7 */
    0x00, /* 8 */
    0x10  /* 9 */
};

/* Some simple letter-ish patterns if needed */
#define SEG7_BLANK 0x7F
#define SEG7_DASH  0x3F
#define SEG7_E     0x06
#define SEG7_L     0x47
#define SEG7_U     0x41
#define SEG7_C     0x46
#define SEG7_O     0x40
#define SEG7_n     0x2B
#define SEG7_P     0x0C

/* Write up to 4 digits as raw segment bytes.
 * Replace register write with your board implementation if needed.
 */
static void hw_write_hex_raw(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    (void)d0; (void)d1; (void)d2; (void)d3;
    /* Example:
       uint32_t word =
           ((uint32_t)d3 << 24) |
           ((uint32_t)d2 << 16) |
           ((uint32_t)d1 << 8)  |
           ((uint32_t)d0);
       *HEX3_HEX0 = word;
    */
}

static void hw_display_number(int value)
{
    if (value < 0) value = 0;
    if (value > 99) value = 99;

    int ones = value % 10;
    int tens = value / 10;

    uint8_t d0 = SEG7_MAP[ones];
    uint8_t d1 = (tens > 0) ? SEG7_MAP[tens] : SEG7_BLANK;

    /* Right-most two digits show the number */
    hw_write_hex_raw(d0, d1, SEG7_BLANK, SEG7_BLANK);
}

/* Example display:
 * [digit slot][selected value tens][selected value ones]
 * This helps the user know which of the 3 code positions they are entering.
 */
static void hw_display_entry_status(int selected_value, int digit_index)
{
    if (selected_value < 0) selected_value = 0;
    if (selected_value > 99) selected_value = 99;

    int ones = selected_value % 10;
    int tens = selected_value / 10;

    uint8_t slot = SEG7_BLANK;
    if (digit_index >= 0 && digit_index <= 2) {
        slot = SEG7_MAP[digit_index + 1];   /* display 1, 2, or 3 */
    }

    hw_write_hex_raw(
        SEG7_MAP[ones],
        (tens > 0) ? SEG7_MAP[tens] : SEG7_BLANK,
        slot,
        SEG7_BLANK
    );
}

/* =========================================================
   APPLICATION LOGIC
   ========================================================= */

typedef enum {
    STATE_ENTER_CODE = 0,
    STATE_VERIFY_CODE,
    STATE_UNLOCKED,
    STATE_ERROR,
    STATE_LOCKOUT,
    STATE_CHANGE_CODE,
    STATE_SAVE_NEW_CODE
} LockState;

typedef struct {
    int stored_code[CODE_LENGTH];
    int entered_code[CODE_LENGTH];
    int new_code[CODE_LENGTH];

    int current_digit_index;
    int attempts_remaining;

    LockState state;
    int state_timer;

    bool last_button_level;
    bool button_edge_pressed;
} LockSystem;

/* =========================================================
   HELPER FUNCTIONS
   ========================================================= */

/* Scale raw ADC to integer 0..40
 * Assumes 12-bit ADC raw range 0..4095.
 * Adjust if your ADC has a different resolution.
 */
static int adc_to_code_value(uint16_t raw)
{
    /* Rounded scaling into 0..40 inclusive */
    int scaled = (int)((raw * MAX_DIGIT_VALUE + 2047) / 4095);

    if (scaled < 0) scaled = 0;
    if (scaled > MAX_DIGIT_VALUE) scaled = MAX_DIGIT_VALUE;
    return scaled;
}

static void clear_entered_code(int code[CODE_LENGTH])
{
    int i;
    for (i = 0; i < CODE_LENGTH; ++i) {
        code[i] = 0;
    }
}

static bool codes_match(const int a[CODE_LENGTH], const int b[CODE_LENGTH])
{
    int i;
    for (i = 0; i < CODE_LENGTH; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

static void copy_code(int dest[CODE_LENGTH], const int src[CODE_LENGTH])
{
    int i;
    for (i = 0; i < CODE_LENGTH; ++i) {
        dest[i] = src[i];
    }
}

static void enter_normal_entry_mode(LockSystem *sys)
{
    sys->state = STATE_ENTER_CODE;
    sys->current_digit_index = 0;
    sys->state_timer = 0;
    clear_entered_code(sys->entered_code);
}

static void enter_change_code_mode(LockSystem *sys)
{
    sys->state = STATE_CHANGE_CODE;
    sys->current_digit_index = 0;
    sys->state_timer = 0;
    clear_entered_code(sys->new_code);
}

static void enter_error_state(LockSystem *sys)
{
    sys->state = STATE_ERROR;
    sys->state_timer = ERROR_HOLD_TICKS;
}

static void enter_success_state(LockSystem *sys)
{
    sys->state = STATE_UNLOCKED;
    sys->state_timer = SUCCESS_HOLD_TICKS;
}

static void enter_lockout_state(LockSystem *sys)
{
    sys->state = STATE_LOCKOUT;
    sys->state_timer = LOCKOUT_TICKS;
}

static void update_button_edge(LockSystem *sys)
{
    bool current = hw_read_confirm_button();

    /* rising-edge detect */
    sys->button_edge_pressed = (current && !sys->last_button_level);
    sys->last_button_level = current;
}

static void show_success_pattern(int tick)
{
    /* Example pattern: alternating few LEDs */
    if ((tick / BLINK_PERIOD_TICKS) % 2 == 0) {
        hw_write_leds(0x155);
    } else {
        hw_write_leds(0x0AA);
    }
}

static void show_error_pattern(int tick)
{
    /* Example: blink low LEDs */
    if ((tick / BLINK_PERIOD_TICKS) % 2 == 0) {
        hw_write_leds(0x00F);
    } else {
        hw_write_leds(0x000);
    }
}

static void show_lockout_pattern(int tick)
{
    /* Example: flash more LEDs during lockout */
    if ((tick / BLINK_PERIOD_TICKS) % 2 == 0) {
        hw_write_leds(0x3FF);
    } else {
        hw_write_leds(0x000);
    }
}

/* =========================================================
   INITIALIZATION
   ========================================================= */

static void lock_init(LockSystem *sys)
{
    int i;

    /* Default stored code for first prototype */
    sys->stored_code[0] = 12;
    sys->stored_code[1] = 25;
    sys->stored_code[2] = 7;

    for (i = 0; i < CODE_LENGTH; ++i) {
        sys->entered_code[i] = 0;
        sys->new_code[i] = 0;
    }

    sys->current_digit_index = 0;
    sys->attempts_remaining = MAX_ATTEMPTS;
    sys->state = STATE_ENTER_CODE;
    sys->state_timer = 0;

    sys->last_button_level = false;
    sys->button_edge_pressed = false;

    hw_write_leds(0);
    hw_display_entry_status(0, 0);
}

/* =========================================================
   MAIN STATE MACHINE STEP
   ========================================================= */

static void lock_step(LockSystem *sys)
{
    uint16_t raw_adc = hw_read_adc_raw();
    int selected_value = adc_to_code_value(raw_adc);

    update_button_edge(sys);

    switch (sys->state) {

        case STATE_ENTER_CODE:
        {
            /* Show the selected number and current entry slot (1/2/3) */
            hw_display_entry_status(selected_value, sys->current_digit_index);

            /* Idle LEDs in normal mode */
            hw_write_leds(0);

            /* When button is pressed, store selected value */
            if (sys->button_edge_pressed) {
                sys->entered_code[sys->current_digit_index] = selected_value;
                sys->current_digit_index++;

                if (sys->current_digit_index >= CODE_LENGTH) {
                    sys->state = STATE_VERIFY_CODE;
                }
            }
            break;
        }

        case STATE_VERIFY_CODE:
        {
            if (codes_match(sys->entered_code, sys->stored_code)) {
                enter_success_state(sys);
            } else {
                sys->attempts_remaining--;

                if (sys->attempts_remaining <= 0) {
                    enter_lockout_state(sys);
                } else {
                    enter_error_state(sys);
                }
            }
            break;
        }

        case STATE_UNLOCKED:
        {
            /* Show success feedback */
            show_success_pattern(sys->state_timer);
            hw_write_leds(((sys->state_timer / BLINK_PERIOD_TICKS) % 2) ? 0x3F : 0x0F);

            /* You mentioned a successful output plus optional change-code mode.
             * For prototype purposes:
             * - display 0 (or any user info code you want)
             * - if switch is on by the time timer expires, enter change mode
             */
            hw_display_number(0);

            if (sys->state_timer > 0) {
                sys->state_timer--;
            } else {
                if (hw_read_change_switch()) {
                    enter_change_code_mode(sys);
                } else {
                    sys->attempts_remaining = MAX_ATTEMPTS;
                    enter_normal_entry_mode(sys);
                }
            }
            break;
        }

        case STATE_ERROR:
        {
            /* Show attempts remaining on 7-seg */
            show_error_pattern(sys->state_timer);
            hw_display_number(sys->attempts_remaining);

            if (sys->state_timer > 0) {
                sys->state_timer--;
            } else {
                enter_normal_entry_mode(sys);
            }
            break;
        }

        case STATE_LOCKOUT:
        {
            /* Block all user input until timer expires */
            show_lockout_pattern(sys->state_timer);

            /* Display something simple like "00" or remaining pseudo-countdown.
             * Here we show 0 during lockout.
             */
            hw_display_number(0);

            if (sys->state_timer > 0) {
                sys->state_timer--;
            } else {
                sys->attempts_remaining = MAX_ATTEMPTS;
                enter_normal_entry_mode(sys);
            }
            break;
        }

        case STATE_CHANGE_CODE:
        {
            /* User is entering a NEW 3-number code using same procedure */
            hw_display_entry_status(selected_value, sys->current_digit_index);

            /* Distinct LED pattern in change mode */
            hw_write_leds(0x030);

            if (sys->button_edge_pressed) {
                sys->new_code[sys->current_digit_index] = selected_value;
                sys->current_digit_index++;

                if (sys->current_digit_index >= CODE_LENGTH) {
                    sys->state = STATE_SAVE_NEW_CODE;
                }
            }
            break;
        }

        case STATE_SAVE_NEW_CODE:
        {
            /* Commit the new code */
            copy_code(sys->stored_code, sys->new_code);

            /* Give confirmation feedback */
            enter_success_state(sys);

            /* After success timer expires, return to normal mode */
            break;
        }

        default:
        {
            /* Recovery fallback */
            sys->attempts_remaining = MAX_ATTEMPTS;
            enter_normal_entry_mode(sys);
            break;
        }
    }
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    LockSystem lock;

    lock_init(&lock);

    while (1) {
        lock_step(&lock);
        hw_small_delay();
    }

    return 0;
}