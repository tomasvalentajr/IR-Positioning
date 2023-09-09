#include "fft_imp.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "kiss_fft/kiss_fft.h"



int fft(int32_t* datapoints, float* frequencySpectrum) {
    // Create an array for your 24-bit data points and Populate the data array with your 24-bit numbers (sample values)
    //printf("fft() started \n");
    int32_t data[N] = {0};
    for (int i = 0; i < N; i++) {
        data[i] = datapoints[i];
    }
    
    // Convert the 24-bit data to 32-bit for kissfft
    kiss_fft_cpx fft_in[N] = {0};
    for (int i = 0; i < N; i++) {
        fft_in[i].r = (kiss_fft_scalar)data[i];
        fft_in[i].i = 0.0;
    }

    // Create kissfft configuration - N Samples, non-inverted FFT
    kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, NULL, NULL);
    
    // Perform the FFT
    kiss_fft_cpx fft_out[N] = {0};
    kiss_fft(cfg, fft_in, fft_out);
    
    // Clean up
    kiss_fft_free(cfg);
    
    
    //Save the magnitudes into an array for calculating the position
    for (int i = 0; i < N; i++) {
        frequencySpectrum[i] = sqrt(fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i);
    }


    for (int i = 0; i < N; i++) {
        float magnitude = sqrt(fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i);
        if (i == 139)
        {
            printf("%f", magnitude);
            printf(" | ");
        } else if (i == 212)
        {
            printf("%f\n", magnitude);
        }
    }
    
    //8388608 values for 23-Bits - 2^23 
    
    //prints used ADC Values
    /*for (int i = 0; i < N; i++) {
        printf("%d\n", data[i]);
    }*/

    return 0;
}