#include <stdio.h>
#include <unistd.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <string.h>
#include "pico/binary_info.h"


#define PIN_SCLK 10
#define PIN_DIN 11
#define PIN_DOUT 12
#define PIN_CS 13
#define PIN_DRDY 14
#define PIN_RST 15
#define PIN_CLKIN 27
#define READ_BIT 0x80

//Registers
static const uint8_t REG_MODE = 0x02;
static const uint8_t REG_DEVID = 0x00;
static const uint8_t REG_CLOCK = 0x03;
static const uint8_t REG_STATUS = 0x01;

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}



static void write_register(uint8_t reg, uint32_t data) {
    uint8_t msg[2];
    msg[0] = reg & 0x7f;  // remove read bit as this is a write
    msg[1] = data;
    cs_select();
    spi_write_blocking(spi_default, msg, 2);
    cs_deselect();
    sleep_ms(10);
}

static int read_registers(uint8_t reg, uint8_t *buf, uint16_t len) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.
    reg |= READ_BIT;
    cs_select();
    spi_write_blocking(spi_default, &reg, 1);
    sleep_ms(10);
    spi_read_blocking(spi_default, 0, buf, len);
    cs_deselect();
    sleep_ms(10);
}

void configure_pins() {
    // Initialize the Pico's standard I/O
    stdio_init_all();

    // Initialize SPI with the specified pins
    spi_init(spi0, 11520);  // SPI clock frequency: 5120 Hz (adjust as needed)
    gpio_set_function(PIN_SCLK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_DIN, GPIO_FUNC_SPI);
    gpio_set_function(PIN_DOUT, GPIO_FUNC_SPI);

    // Configure the CS, DRDY, RST, and CLKIN pins
    gpio_init(PIN_CS);
    gpio_init(PIN_DRDY);
    gpio_init(PIN_RST);
    gpio_init(PIN_CLKIN);

    // Set the pins as outputs, except for DRDY (input)
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DRDY, GPIO_IN);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_set_dir(PIN_CLKIN, GPIO_OUT);

    // Set initial states for some pins (as needed)
    gpio_put(PIN_CS, 1);     // CS high (inactive)
    gpio_put(PIN_RST, 1);    // Reset pin high (inactive)
    gpio_put(PIN_CLKIN, 0);  // CLKIN low (use 8,192 MHz frequency)
    printf("Pins Configured");
}

int fft() {
    configure_pins();
    write_register(REG_CLOCK, 0000000100001010); //Set High-Resolution Mode and OSR = 512, Enable ADC Channel 0
    write_register(REG_MODE, 0000000100010100); //DRDY should assert each time, chany channel has data
    gpio_put(PIN_RST, 0);
    sleep_ms(1); //wait for at least 250 ns for RESET
    gpio_put(PIN_RST, 1); //data registers have been reset
    gpio_put(PIN_DRDY, 1); //begin communication, needs /CS on low
    gpio_put(PIN_CS, 0);
    printf("FFT Function started");

    while (1) {


        // Your main program logic can go here
        // Perform SPI transactions with the external ADC

        // Example: Read ADC value
        //gpio_put(PIN_CS, 0); // Activate the ADC chip-select
        // Perform SPI read/write operations as needed
        //gpio_put(PIN_CS, 1); // Deactivate the ADC chip-select
        uint32_t value = gpio_get(PIN_DOUT);
        // Example: Wait for DRDY to become high (indicating data ready)
        printf("ADC Value: %lu\n", value);
        while (!gpio_get(PIN_DRDY)) {
            tight_loop_contents();
        }
        // DRDY is high, indicating data is ready for reading

        sleep_ms(100);  // A delay for demonstration purposes (adjust as needed)
    }

    return 0;
}
