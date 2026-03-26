Combo Lock Project - Refined Modular Version
===========================================

Files included:
- ComboLockV1.c     : main program and finite state machine
- ComboLockV1.h     : lock configuration and state definitions
- board.c / board.h : memory-mapped board addresses and low-level helpers
- input.c / input.h : ADC, button, and switch input processing
- display.c / display.h : 7-seg and LED output functions

Build idea:
Use your ARM/Monitor Program project and add all .c and .h files.
One build should compile and link all C files into a single executable.

Suggested compile command shape:
arm-altera-eabi-gcc ComboLockV1.c board.c input.c display.c -o ComboLockV1.elf

Project behavior implemented:
- 3-number combination lock
- potentiometer selects each number (0-39)
- KEY0 confirms selected number
- correct code -> success indication
- wrong code -> attempts remaining displayed
- 3 failed attempts -> timed lockout with flashing LED bar
- SW0 after successful unlock -> change-code mode

Notes:
- This code uses the exact addresses from the provided ComboLockV1.c groundwork.
- The ADC mapping is kept at 0-39 to remain consistent with the original file.
- If your team decides to strictly use 0-40, update adc_to_number() and related comments together.
