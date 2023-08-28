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
    //printf("datapoints loaded \n");

    
    // Convert the 24-bit data to 32-bit for kissfft
    kiss_fft_cpx fft_in[N] = {0};
    for (int i = 0; i < N; i++) {
        fft_in[i].r = (kiss_fft_scalar)data[i];
        fft_in[i].i = 0.0;
    }
    //printf("datapoints converted to real and imaginary scalars \n");

    // Create kissfft configuration - N Samples, non-inverted FFT
    kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, NULL, NULL);
    //printf("kissfft configured \n");
    // Perform the FFT
    kiss_fft_cpx fft_out[N] = {0};
    kiss_fft(cfg, fft_in, fft_out);
    //printf("FFT performed \n");
    // Clean up
    kiss_fft_free(cfg);
    // printf("free(cfg) \n");
    
    //Save the magnitudes into an array
    for (int i = 0; i < N; i++) {
        frequencySpectrum[i] = sqrt(fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i);
    }

    // Print the FFT results (magnitude) of the 140th and 212th frequency bin
    /*float mag = sqrt(fft_out[139].r * fft_out[139].r + fft_out[139].i * fft_out[139].i);
    printf("%f", mag);
    printf(" | ");
    mag = sqrt(fft_out[212].r * fft_out[212].r + fft_out[212].i * fft_out[212].i);
    printf("%f\n", mag);*/

    for (int i = 0; i < N; i++) {
        float magnitude[i] = sqrt(fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i);
        if (i == 139)
        {
            printf("%f", magnitude[i]);
            printf(" | ");
        } else if (i == 212)
        {
            printf("%f\n", magnitude[i]);
        }
        //printf("%f\n", magnitude);
    }
    //printf("Results printed \n");
    //8388608 values for 23-Bits - 2^23 
    //float conData[N] = {0};
    //float voltRef = 1.1;
    /*for (int i = 0; i < N; i++) {
        conData[i] = data[i] * voltRef / 8388608;
    }*/
    /*for (int i = 0; i < N; i++) {
        printf("%d\n", data[i]);
    }*/
    return 0;
}