// Compiler headers
#include <stdio.h>
#include <math.h>
#include "string.h"

// Pico SDK headers
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Project specific headers
#include "adc.h"
#include "pwm.h"
//#include "fft_imp.h"



int main() {
    
    stdio_init_all();
    sleep_ms(1000);
    printf("stdio_init() \n");
    
    LED_Init();
    //FFT_Init();
    
    //For testing, turn both LEDs on, no blinking
    LED_ON();
    printf("LED_ON \n");
    //pwm();
    ADC_Init();
    printf("adc_init() \n");
    

    uint32_t adcData;
    while (1)
    {
       bool check = ADC_CheckData();
        if (check == true) {
        adcData = ADC_GetRaw(0);
        printf("Value: %u\n", adcData, "\n");
       }
       else {
        printf("DRDY false - no new data available \n");
       }
       sleep_ms(1000);
    }
}

// Some compiler versions throw errors like "_xxxxx() not defined". Provide dummy implementation here to silent them.
//void _close() {}
//void _lseek() {}