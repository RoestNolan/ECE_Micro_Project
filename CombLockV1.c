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

volatile a9_timer* const a9_ptr = (a9_timer*)PRIVATE_TIMER_BASE;

volatile int* const sw_ptr        = (int*)SW_BASE; //switch pointer
volatile int* const key_ptr       = (int*)KEY_BASE; //button pointer
volatile int* const hex0_hex3_ptr = (int*)HEX0_HEX3_BASE; //hex0-3 pointer
volatile int* const hex4_hex5_ptr = (int*)HEX4_HEX5_BASE; //hex4-5 pointer
/********************************************************************/


/**GPIO CONFIG**/
#define JP1_BASE    0xFF200060 //LED bar base
#define ADC_BASE    0xFF204000 //potentiometer base
#define ADC_DONE_BIT 16      // simulator
// #define ADC_DONE_BIT 15   // hardware
#define LED_MASK    0x3FF        // bits 0-9 led bar
#define ADC_DATA_MASK 0x0FFF     

volatile int * const jp1_data = (int *)JP1_BASE; 
volatile int * const jp1_dir = (int *)(JP1_BASE + 0x04);
volatile int * const adc_ch0 = (int *)(ADC_BASE + 0x00);
/********************************************************************/


/**GPIO CONFIG**
* seg7[10]:
* Lookup table containing the 7-segment encoding patterns for
* digits 0–9.
*
* pack4(h3, h2, h1, h0):
* Combines four 7-segment byte values into a single 32-bit
* integer for writing to the HEX3_HEX0 register.
*
* pack2(h5, h4): * Combines two 7-segment byte values into a single integer
* for writing to the HEX5_HEX4 register.
*
*/
static const unsigned char seg7[10] = {
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


static int pack4(unsigned char h3, unsigned char h2, unsigned char h1, unsigned char h0)
{
    return ((int)h3 << 24) | ((int)h2 << 16) | ((int)h1 << 8) | (int)h0;
}

static int pack2(unsigned char h5, unsigned char h4)
{
    return ((int)h5 << 8) | (int)h4;
}

/********************************************************************/


/****************INPUT READING METHODS*******************/


/***** Get Raw ADC Value *****/
int read_adc_value(void)
{
    return (*adc_ch0) & ADC_DATA_MASK;
}

/***** Convert POT to range from 0-39 *****/
int adc_to_number(int adc_value){

    int num = (adc_value * 40) / 4096;
    if (adc_value == 4095) num = 39;
    if (num <= 0) return 0;
    if (num > 40) return 39;
    return num;
}




/****************DISPLAY/OUTPUT METHODS*******************/

/***** Display number on 7-seg *****/
void display_number(int number)
{
    int tens = number / 10;
    int ones = number % 10;
    unsigned char blank = 0x00;
    unsigned char hex0 = seg7[ones];
    unsigned char hex1 = (number < 10) ? blank : seg7[tens];

    *hex0_hex3_ptr = pack4(blank, blank, hex1, hex0);
}


/***** Display and subtract attempts remaining on 7-seg *****/
volatile int attempts_remaining = 3; // 3 error attempts total

int display_and_decrement_attempts(void)
{
    unsigned char blank = 0x00;

    if (attempts_remaining > 0) attempts_remaining--;

    unsigned char hex0 = seg7[attempts_remaining];

    *hex0_hex3_ptr = pack4(blank, blank, blank, hex0);

    if (attempts_remaining > 0)
        return 1;   // TRUE: attempts remaining
    else
        return 0;   // FALSE: lockout
}












