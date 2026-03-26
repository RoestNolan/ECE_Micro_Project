#ifndef BOARD_H
#define BOARD_H

/****************BASE ADDRESS CONFIGURATION*******************/

#define KEY_BASE 0xFF200050 //button base
#define SW_BASE 0xFF200040 //switch base
#define HEX0_HEX3_BASE 0xFF200020 //7-seg (1,2,3,4)
#define HEX4_HEX5_BASE 0xFF200030 //7-seg (5,6)

/**A9 TIMER CONFIG**/
#define PRIVATE_TIMER_BASE 0xFFFEC600 //A9 Private timer

typedef struct
{
    int load;
    int count;
    int control;
    int status;
} a9_timer;

extern volatile a9_timer* const a9_ptr;
extern volatile int* const sw_ptr;
extern volatile int* const key_ptr;
extern volatile int* const hex0_hex3_ptr;
extern volatile int* const hex4_hex5_ptr;

/**GPIO CONFIG**/
#define LEDR_BASE   0xFF200000 // built-in red LED register
#define ADC_BASE    0xFF204000 // potentiometer base
    // simulator
#define ADC_DONE_BIT 15   // hardware
#define LED_MASK    0x3FF        // bits 0-9 of built-in LEDs
#define ADC_DATA_MASK 0x0FFF

extern volatile int * const ledr_ptr;
extern volatile int * const adc_ch0;

/**GPIO CONFIG**
* seg7[10]:
* Lookup table containing the 7-segment encoding patterns for
* digits 0-9.
*
* pack4(h3, h2, h1, h0):
* Combines four 7-segment byte values into a single 32-bit
* integer for writing to the HEX3_HEX0 register.
*
* pack2(h5, h4):
* Combines two 7-segment byte values into a single integer
* for writing to the HEX5_HEX4 register.
*
*/
extern const unsigned char seg7[10];

/***** Pack HEX3-HEX0 segment bytes into one 32-bit value *****/
int pack4(unsigned char h3, unsigned char h2, unsigned char h1, unsigned char h0);

/***** Pack HEX5-HEX4 segment bytes into one 16-bit value *****/
int pack2(unsigned char h5, unsigned char h4);

/***** Initialize built-in LEDR outputs to OFF *****/
void board_init_gpio(void);

/***** Clear both HEX register groups *****/
void clear_all_hex(void);

/***** Small delay to slow down main loop for stable polling *****/
void small_delay(void);

#endif
