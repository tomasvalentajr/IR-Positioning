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
#include "fft_imp.h"

int main() {
    stdio_init_all();

    ADC_Init();
    //LED_Init();
    //FFT_Init();
    
    //For testing, turn both LEDs on, no blinking
    LED_ON();


    uint32_t adcData;
    while (1)
    {
       ADC_CheckData();
       if (true) {
       adcData = ADC_GetRaw(0);
       printf("Value: %u\n", adcData);
       }
    }
}

// Some compiler versions throw errors like "_xxxxx() not defined". Provide dummy implementation here to silent them.
void _close() {}
void _lseek() {}