#include "ComboLockV1.h"

/****************SYSTEM INITIALIZATION*******************/

/***** Initialize lock system with default code and reset state *****/
void lock_init(LockSystem *lock)
{
    int i;

    lock->stored_code[0] = 12;
    lock->stored_code[1] = 25;
    lock->stored_code[2] = 7;

    for (i = 0; i < CODE_LENGTH; i++) {
        lock->entered_code[i] = 0;
        lock->new_code[i] = 0;
    }

    lock->digit_index = 0;
    lock->attempts_remaining = MAX_ATTEMPTS;
    lock->state = STATE_ENTER_CODE;
    lock->feedback_counter = 0;
    lock->lockout_counter = 0;

    board_init_gpio();
    clear_led_bar();
    clear_all_hex();
}

/***** Compare two 3-number codes and return TRUE if they match exactly *****/
static int codes_match(const int a[CODE_LENGTH], const int b[CODE_LENGTH])
{
    int i;
    for (i = 0; i < CODE_LENGTH; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

/***** Clear current entered code buffer and reset entry position *****/
static void reset_entered_code(LockSystem *lock)
{
    int i;
    for (i = 0; i < CODE_LENGTH; i++) {
        lock->entered_code[i] = 0;
    }
    lock->digit_index = 0;
}

/***** Clear new code buffer used during password change mode *****/
static void reset_new_code(LockSystem *lock)
{
    int i;
    for (i = 0; i < CODE_LENGTH; i++) {
        lock->new_code[i] = 0;
    }
    lock->digit_index = 0;
}

/***** Save new code into stored code array *****/
static void save_new_code(LockSystem *lock)
{
    int i;
    for (i = 0; i < CODE_LENGTH; i++) {
        lock->stored_code[i] = lock->new_code[i];
    }
}

/***** Begin short success feedback period *****/
static void enter_success_state(LockSystem *lock)
{
    lock->state = STATE_SUCCESS;
    lock->feedback_counter = SUCCESS_FEEDBACK_TICKS;
    show_success_led_pattern();
    display_success_message();
}

/***** Begin short error feedback period *****/
static void enter_error_state(LockSystem *lock)
{
    lock->state = STATE_ERROR;
    lock->feedback_counter = ERROR_FEEDBACK_TICKS;
    show_error_led_pattern();
    display_attempts_remaining(lock->attempts_remaining);
}

/***** Begin timed lockout period after repeated failed attempts *****/
static void enter_lockout_state(LockSystem *lock)
{
    lock->state = STATE_LOCKOUT;
    lock->lockout_counter = LOCKOUT_TICKS;
    display_lockout_message();
}

/****************MAIN STATE MACHINE*******************/

/***** Execute one update of the combination lock state machine *****/
void lock_update(LockSystem *lock)
{
    int selected_value = adc_to_number(read_adc_value());

    switch (lock->state) {

        case STATE_ENTER_CODE:
            clear_led_bar();
            display_entry_status(lock->digit_index, selected_value);

            if (confirm_button_pressed_edge()) {
                lock->entered_code[lock->digit_index] = selected_value;
                lock->digit_index++;

                if (lock->digit_index >= CODE_LENGTH) {
                    lock->state = STATE_VERIFY_CODE;
                }
            }
            break;

        case STATE_VERIFY_CODE:
            if (codes_match(lock->entered_code, lock->stored_code)) {
                enter_success_state(lock);
            }
            else {
                lock->attempts_remaining--;

                if (lock->attempts_remaining <= 0) {
                    enter_lockout_state(lock);
                }
                else {
                    enter_error_state(lock);
                }
            }
            break;

        case STATE_SUCCESS:
            if (lock->feedback_counter > 0) {
                lock->feedback_counter--;
            }
            else {
                lock->attempts_remaining = MAX_ATTEMPTS;

                if (read_change_mode_switch()) {
                    reset_new_code(lock);
                    lock->state = STATE_CHANGE_CODE;
                }
                else {
                    reset_entered_code(lock);
                    clear_led_bar();
                    clear_all_hex();
                    lock->state = STATE_ENTER_CODE;
                }
            }
            break;

        case STATE_ERROR:
            if (lock->feedback_counter > 0) {
                lock->feedback_counter--;
            }
            else {
                reset_entered_code(lock);
                clear_led_bar();
                clear_all_hex();
                lock->state = STATE_ENTER_CODE;
            }
            break;

        case STATE_LOCKOUT:
            flash_lockout_led_pattern();
            display_lockout_message();

            if (lock->lockout_counter > 0) {
                lock->lockout_counter--;
            }
            else {
                lock->attempts_remaining = MAX_ATTEMPTS;
                reset_entered_code(lock);
                clear_led_bar();
                clear_all_hex();
                lock->state = STATE_ENTER_CODE;
            }
            break;

        case STATE_CHANGE_CODE:
            show_change_mode_led_pattern();
            display_entry_status(lock->digit_index, selected_value);

            if (confirm_button_pressed_edge()) {
                lock->new_code[lock->digit_index] = selected_value;
                lock->digit_index++;

                if (lock->digit_index >= CODE_LENGTH) {
                    lock->state = STATE_SAVE_NEW_CODE;
                }
            }
            break;

        case STATE_SAVE_NEW_CODE:
            save_new_code(lock);
            enter_success_state(lock);
            break;

        default:
            lock->state = STATE_ENTER_CODE;
            reset_entered_code(lock);
            clear_led_bar();
            clear_all_hex();
            break;
    }
}

/****************PROGRAM ENTRY*******************/

/***** Main program loop - initialize system then run forever *****/
int main(void)
{
    LockSystem lock;

    lock_init(&lock);

    while (1) {
        lock_update(&lock);
        small_delay();
    }

    return 0;
}
