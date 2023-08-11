#ifndef FFT_IMP_H
#define FFT_IMP_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "kiss_fft/kiss_fft.h"

#define N 1024 // Number of data points

// Function to calculate FFT results
int fft(int32_t datapoints[N]);

#endif // FFT_IMP_H