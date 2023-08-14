#include "fft_imp.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "kiss_fft/kiss_fft.h"



int fft(int32_t datapoints[N]) {
    // Create an array for your 24-bit data points and Populate the data array with your 24-bit numbers (sample values)
    printf("fft() started \n");
    int32_t data[N];
    for (int i = 0; i < N; i++) {
        data[i] = datapoints[i];
    }
    printf("datapoints loaded \n");

    
    // Convert the 24-bit data to 32-bit for kissfft
    kiss_fft_cpx fft_in[N];
    for (int i = 0; i < N; i++) {
        fft_in[i].r = (kiss_fft_scalar)data[i];
        fft_in[i].i = 0.0;
    }
    printf("datapoints converted to real and imaginary scalars \n");

    // Create kissfft configuration - N Samples, non-inverted FFT
    kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, NULL, NULL);
    printf("kissfft configured \n");
    // Perform the FFT
    kiss_fft_cpx fft_out[N];
    kiss_fft(cfg, fft_in, fft_out);
    printf("FFT preformed \n");
    // Clean up
    kiss_fft_free(cfg);
    printf("free(cfg) \n");
     // Print the FFT results (magnitude)
    for (int i = 0; i < N; i++) {
        float magnitude = sqrt(fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i);
        printf("Frequency bin %d: %f\n", i, magnitude);
    }
    printf("Results printed \n");
    for (int i = 0; i < N; i++) {
        printf("ADC-Data: %d\n", data[i]);
    }
    return 0;
}