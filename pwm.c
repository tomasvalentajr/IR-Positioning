#include "pwm.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#define PIN_PWM_GP28 28
#define PIN_PWM_GP26 26
// Output PWM signals on pins 26 and 28


void LED_ON() {
    const uint LED1A_PIN = 26;
    const uint LED2A_PIN = 28;
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    gpio_init(LED1A_PIN);
    gpio_init(LED2A_PIN);
    gpio_set_dir(LED1A_PIN, GPIO_OUT);
    gpio_set_dir(LED2A_PIN, GPIO_OUT);
    
    gpio_put(LED1A_PIN, 1);
    gpio_put(LED2A_PIN, 1);  
    
}


void pwm()
{    
    gpio_set_function(26, GPIO_FUNC_PWM);
    gpio_set_function(28, GPIO_FUNC_PWM);
    uint32_t clock_freq = 125000000;

    uint16_t prescaler = 12500;
    float pwm_freq_A = 363.0; //526 Hz = Bin 142
    float pwm_freq_B = 250.0; //345 Hz = Bin 445

    float clock_div_A = clock_freq / prescaler / pwm_freq_A;
    //uint32_t pwm_wrap_A = clock_div_A / 2;

    float clock_div_B = clock_freq / prescaler / pwm_freq_B;
    //uint32_t pwm_wrap_B = clock_div_B / 2;

    // Find out which PWM slice is connected to GPIO 26 and 28 (it's slice 5 and 6)
    uint slice_num_A = pwm_gpio_to_slice_num(26);
    uint slice_num_B = pwm_gpio_to_slice_num(28);

    pwm_set_wrap(slice_num_A, prescaler);
    pwm_set_clkdiv(slice_num_A, clock_div_A);

    pwm_set_wrap(slice_num_B, prescaler);
    pwm_set_clkdiv(slice_num_B, clock_div_B);

    // Set the PWM running
    pwm_set_enabled(slice_num_A, true);
    pwm_set_chan_level(slice_num_A, PWM_CHAN_A, prescaler/2);
    pwm_set_enabled(slice_num_B, true);
    pwm_set_chan_level(slice_num_B, PWM_CHAN_A, prescaler/2);
}
