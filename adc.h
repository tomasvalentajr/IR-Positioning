#ifndef ADC_H
#define ADC_H

#include <stdio.h>
#include "pico/stdlib.h"

#define COMMAND_NULL                0b0000
#define COMMAND_RESET               0b10001
#define COMMAND_STANDBY             0b100010
#define COMMAND_WAKEUP              0b110011
#define COMMAND_LOCK                0b10101010101
#define COMMAND_UNLOCK              0b11001010101
#define COMMAND_READ(reg, num)      ((0b101 << 13) | (reg << 7) | (num - 1))
#define COMMAND_WRITE(reg, num)     ((0b011 << 13) | (reg << 7) | (num - 1))


#define RESP_RESET                  0b1111111100101000
#define RESP_STANDBY                0b0000000000100010
#define RESP_WAKEUP                 0b0000000000110011
#define RESP_LOCK                   0b0000010101010101
#define RESP_UNLOCK                 0b0000011001010101


/*#define B           -0.0000005775
#define p           0.0039083 / -0.0000005775       // p = A / B
#define R_0         1000.0*/


void ADC_Init(void);

uint16_t ADC_ReadRegister(uint8_t reg);
bool ADC_WriteRegister(uint8_t reg, uint16_t val);

bool ADC_CheckData(void);

uint32_t ADC_GetRaw(size_t ch);
/*double ADC_GetRatio(size_t ch);
double ADC_GetResistance(size_t ch);
double ADC_GetTemperature(size_t ch);*/

#endif /* ADC_H */
