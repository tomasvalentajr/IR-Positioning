#include "pwm.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#define PIN_PWM_GP28 28
#define PIN_PWM_GP26 26
// Output PWM signals on pins 26 and 28
void LED_Init() {
    gpio_set_function(26, GPIO_FUNC_PWM);
    gpio_set_function(28, GPIO_FUNC_PWM);
}

void LED_ON() {
    const uint LED1A_PIN = 31;
    const uint LED2A_PIN = 34;
    gpio_init(LED1A_PIN);
    gpio_init(LED2A_PIN);
    gpio_set_dir(LED1A_PIN, GPIO_OUT);
    gpio_set_dir(LED2A_PIN, GPIO_OUT);
    
    gpio_put(LED1A_PIN, 1);
    gpio_put(LED2A_PIN, 1);    
}

void LED_OFF() {
    const uint LED1_PIN = 31;
    const uint LED2_PIN = 34;
    gpio_init(LED1_PIN);
    gpio_init(LED2_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    
    gpio_put(LED1_PIN, 0);
    gpio_put(LED2_PIN, 0);    
}

void pwm() {
    /// \tag::setup_pwm[]
    gpio_set_function(26, GPIO_FUNC_PWM);
    gpio_set_function(28, GPIO_FUNC_PWM);
    // Tell GPIO 26 and 28 they are allocated to the PWM
    gpio_set_function(26, GPIO_FUNC_PWM);
    gpio_set_function(28, GPIO_FUNC_PWM);
    uint32_t clock_freq = 125000000;
    uint32_t pwm_freq_A = 4;
    uint32_t pwm_freq_B = 4;

    uint32_t clock_div_A = clock_freq / pwm_freq_A;
    uint32_t pwm_wrap_A = clock_div_A / 2;

    uint32_t clock_div_B = clock_freq / pwm_freq_B;
    uint32_t pwm_wrap_B = clock_div_B / 2;

    // Find out which PWM slice is connected to GPIO 26 and 28 (it's slice 5 and 6)
    uint slice_num_A = pwm_gpio_to_slice_num(26);
    uint slice_num_B = pwm_gpio_to_slice_num(28);

    // Set period of 4 cycles (0 to 3 inclusive)
    //pwm_set_wrap(slice_num, pwm_wrap);
    // Set channel A output high for one cycle before dropping
    //pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
    // Set initial B output high for three cycles before dropping
    //pwm_set_chan_level(slice_num, PWM_CHAN_B, 3);

    pwm_set_wrap(slice_num_A, clock_div_A);
    pwm_set_clkdiv(slice_num_A, clock_div_A);

    pwm_set_wrap(slice_num_B, clock_div_B);
    pwm_set_clkdiv(slice_num_B, clock_div_B);
    // Set the PWM running
    pwm_set_enabled(slice_num_A, true);
    pwm_set_chan_level(slice_num_A, PWM_CHAN_A, pwm_wrap_A / 2);
    pwm_set_enabled(slice_num_B, true);
    pwm_set_chan_level(slice_num_B, PWM_CHAN_A, pwm_wrap_B / 2);
    /// \end::setup_pwm[]

    // Note we could also use pwm_set_gpio_level(gpio, x) which looks up the
    // correct slice and channel for a given GPIO.
}
