#ifndef COMBOLOCKV1_H
#define COMBOLOCKV1_H

#include "board.h"
#include "input.h"
#include "display.h"

/****************PROJECT CONFIGURATION*******************/

#define CODE_LENGTH 3
#define MAX_ATTEMPTS 3

/*
 * The project write-up describes each code number as ranging from 0-40.
 * This implementation uses 0-39 because the existing ComboLockV1.c
 * groundwork already maps the ADC that way.
 */
#define MAX_CODE_VALUE 39

#define SUCCESS_FEEDBACK_TICKS 200000
#define ERROR_FEEDBACK_TICKS   200000
#define LOCKOUT_TICKS          800000

typedef enum {
    STATE_ENTER_CODE,
    STATE_VERIFY_CODE,
    STATE_SUCCESS,
    STATE_ERROR,
    STATE_LOCKOUT,
    STATE_CHANGE_CODE,
    STATE_SAVE_NEW_CODE
} LockState;

typedef struct {
    int stored_code[CODE_LENGTH];
    int entered_code[CODE_LENGTH];
    int new_code[CODE_LENGTH];

    int digit_index;
    int attempts_remaining;

    int feedback_counter;
    int lockout_counter;

    LockState state;
} LockSystem;

/***** Initialize lock structure and board I/O *****/
void lock_init(LockSystem *lock);

/***** Execute one update of the main state machine *****/
void lock_update(LockSystem *lock);

#endif
