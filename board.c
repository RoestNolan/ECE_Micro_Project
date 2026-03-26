#include "board.h"

volatile a9_timer* const a9_ptr = (a9_timer*)PRIVATE_TIMER_BASE;

volatile int* const sw_ptr        = (int*)SW_BASE; //switch pointer
volatile int* const key_ptr       = (int*)KEY_BASE; //button pointer
volatile int* const hex0_hex3_ptr = (int*)HEX0_HEX3_BASE; //hex0-3 pointer
volatile int* const hex4_hex5_ptr = (int*)HEX4_HEX5_BASE; //hex4-5 pointer

volatile int * const ledr_ptr = (int *)LEDR_BASE;
volatile int * const adc_ch0 = (int *)(ADC_BASE + 0x00);

static const int BLANK_HEX_GROUP = 0x00000000;

const unsigned char seg7[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

/***** Pack four 7-segment byte values for HEX3-HEX0 output *****/
int pack4(unsigned char h3, unsigned char h2, unsigned char h1, unsigned char h0)
{
    return ((int)h3 << 24) | ((int)h2 << 16) | ((int)h1 << 8) | (int)h0;
}

/***** Pack two 7-segment byte values for HEX5-HEX4 output *****/
int pack2(unsigned char h5, unsigned char h4)
{
    return ((int)h5 << 8) | (int)h4;
}

/***** Clear built-in LEDR outputs on startup *****/
void board_init_gpio(void)
{
    *ledr_ptr = 0;
}

/***** Clear all six HEX displays *****/
void clear_all_hex(void)
{
    *hex0_hex3_ptr = BLANK_HEX_GROUP;
    *hex4_hex5_ptr = 0x0000;
}

/***** Small software delay used to slow polling and output updates *****/
void small_delay(void)
{
    volatile int count;
    for (count = 0; count < 800; count++) {
    }
}
