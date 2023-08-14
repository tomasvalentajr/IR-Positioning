// Compiler headers
#include <stdio.h>
#include <math.h>
#include <time.h>
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
    

    int32_t adcData[256];
    for (int i = 0; i < 256; i++)
    {
        ADC_CheckData();
        //if (check == true) {
        adcData[i] = ADC_GetRaw(0);// + (rand() % 3);
        printf("Value: %u\n", i, adcData[i], "\n");
    //}
       /*else {
        printf("DRDY false - no new data available \n");
       }*/
       sleep_ms(250);
    }
    //adcData = ADC_CollectData(N); //this function call is ready to be used, when DRDY begins working
    fft(adcData);

}

// Some compiler versions throw errors like "_xxxxx() not defined". Provide dummy implementation here to silent them.
//void _close() {}
//void _lseek() {}