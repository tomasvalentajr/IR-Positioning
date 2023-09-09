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
    int binNum1 = 139; //Frequency bin of the first source
    int binNum2 = 212; //Frequency bin of the second source
    float amp1 = 0;
    float amp2 = 0;
    float distRatio = 0;

    amp1 = freq[binNum1];
    amp2 = freq[binNum2];
    distRatio = sqrt(amp2 / amp1);

    printf("l1/l2 =%f\n", distRatio);
}


int main() {
    stdio_init_all();
          
    //For testing, turn both LEDs on, no blinking
    //LED_ON();
    
    //Turn on the IR-Sources with their characteristic frequencies
    pwm(); 
    sleep_ms(500);

    ADC_Init();
    //printf("adc_init() \n"); //ADC_Init done
    
    int32_t adcData[N] = {0};   //Array for storing raw ADC Data
    float freqBins[N] = {0};    //Array for storing the amplitudes of the frequency spectrum
    while (1)
    {
        //ADC_MonitorData(); //for monitoring the ADC Values without 
        for(int q = 0; q < 20; q++) {
        ADC_CollectData(adcData, N); //collect ADC Values for FFT
        fft(adcData, freqBins); //calculate FFT for the ADC values
        sleep_ms(150); //wait 150ms before making a new measurement
        }
        printf("results printed \n"); //sign, that 20 FFTs have been alculated
        getPosition(freqBins);
    }
}