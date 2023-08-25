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

void getPosition(float* freq) {
    //calculate the relative position of the IR-sources from the relative amplitudes of their frequencies
    int binNum1 = 513; //Frequency bin of the first source
    int binNum2 = 353; //Frequency bin of the second source - subject to change
    float amp1 = 0;
    float amp2 = 0;
    float relDistDiff = 0;
    float coef1 = 1;    //coefficients for calibration of the IR-Sources
    float coef2 = 1;

    amp1 = freq[binNum1];
    amp2 = freq[binNum2];
    if(amp2 <= amp1){
        relDistDiff = sqrt(amp2/amp1);  
        //printf("Das verhältnis zwischen Abständen l1 und l2 beträgt: %f \n", relDistDiff);
        printf("l1 =%f \n", relDistDiff, "*l2");
    }else {
        relDistDiff = sqrt(amp1/amp2);
        //printf("Das verhältnis zwischen Abständen l2 und l1 beträgt: %f \n", relDistDiff);
        printf("l1 =%f \n", 1/relDistDiff, "*l2");
    }   
}



int main() {
    
    stdio_init_all();
    //sleep_ms(1000);
        
    
    //For testing, turn both LEDs on, no blinking
    LED_ON();
    
    //Turn on the IR-Sources with their characteristic frequencies
    pwm();
    sleep_ms(2000);

    //sleep_ms(1000);
    ADC_Init();
    printf("adc_init() \n");
    
    

    int32_t adcData[N] = {0};   //Array for storing raw ADC Data
    float freqBins[N] = {0};    //Array for storing the amplitudes of the frequency spectrum
    while (1)
    {
        //ADC_MonitorData();
        ADC_CollectData(adcData, N); //this function call is ready to be used, when DRDY begins working
        fft(adcData, freqBins);
        sleep_ms(1000);
        //getPosition(freqBins);
    }
    
    
    //printf("pwm() \n");
    ADC_CollectData(adcData, N); //this function call is ready to be used, when DRDY begins working
    fft(adcData, freqBins);
}