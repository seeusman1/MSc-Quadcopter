#ifndef __FILTERING_BUTTERWORTH_H
#define __FILTERING_BUTTERWORTH_H
#include <stdint.h>
int32_t dCoefficient1;
int32_t dCoefficient2;
int32_t dCoefficient3;
int32_t dCoefficient4;
int32_t dCoefficient5;
int32_t dGain;
int32_t bw_filter(int32_t input);

#endif