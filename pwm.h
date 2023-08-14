#ifndef PWM_H
#define PWM_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"



//void LED_Init(void);
void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);
void pwm(void);


#endif /*PWM_H*/