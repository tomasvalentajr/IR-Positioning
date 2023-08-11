#include "adc.h"

#include "math.h"

#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"




// ADC word length = 24 bit

#define int24(addr)             (*addr << 16 | *(addr + 1) << 8 | *(addr + 2))
#define switch_endian(val)      (((val & 0x00ff) << 8) | ((val & 0xff00) >> 8))

#define PIN_SCLK 10
#define PIN_DIN 11
#define PIN_DOUT 12
#define PIN_CS 13
#define PIN_DRDY 14
#define PIN_RST 15
#define PIN_CLKIN 27

uint8_t tx_data[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t rx_data[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint32_t adc_data[4];
int len = 18;

//static const uint8_t REG_DEVID = 0x00;
//static const uint8_t REG_STATUS = 0x01;
static const uint8_t REG_MODE = 0x02;
static const uint8_t REG_CLOCK = 0x03;

uint16_t ADC_CONFIG_CLOCK = 
        0x00000001  << 8  |   // 15:8     Enable only channel 0
        0b0   << 7  |   // 7        Reserved
        0b0   << 6  |   // 6        Reserved
        0b0   << 5  |   // 5        No TurboMode
        0b010 << 2  |   // 4:2      Oversampling ratio (0b010 = 512)
        0b10;           // 1:0      Power mode (0b10 = high resolution)

uint16_t ADC_CONFIG_MODE = 
        0b00  << 14 |   // 15:14    Reserved
        0b0   << 13 |   // 13       Register Map CRC enable
        0b0   << 12 |   // 12       SPI input CRC enable
        0b0   << 11 |   // 11       SPI and register map CRC type
        0b0   << 10 |   // 10       Reset
        0b01  << 8  |   // 9:8      Data word length selection: 24 bit
        0b000 << 5  |   // 7:5      Reserved
        0b1   << 4  |   // 4        SPI Timeout enable
        0b01  << 2  |   // 3:2      DRDY pin signal source selection - 01b = the first one
        0b0   << 1  |   // 1        DRDY pin state when conversion data is not available
        0b0;            // 0        DRDY signal format when conversion data is available


static void ADC_SetCommand(uint16_t command)
{
    tx_data[0] = command >> 8 & 0xff;
    tx_data[1] = command & 0xff;
}

static void ADC_SetRegisterValue(uint16_t value)
{
    tx_data[3] = value >> 8 & 0xff;
    tx_data[4] = value & 0xff;
}

static uint16_t ADC_GetResponse(void)
{
    return rx_data[0] << 8 | rx_data[1];
}

static void ADC_ExchangeData(int len)
{
    gpio_put(PIN_CS, 0);       // 8,33 ns per instruction vs. 16 ns min delay
    sleep_us(1);
    spi_write_read_blocking(spi1, tx_data, rx_data, len);
    sleep_us(1);
    gpio_put(PIN_CS, 1);
}

bool ADC_ConvertResults(void)
{
    uint8_t* offset = rx_data + 3;
    for (int ch = 0; ch < 4; ch++)
    {
        adc_data[ch] = int24(offset);
        offset +=3;
    }
    return true;
}


void ADC_Init(void)
{
    // Start external clock for ADC
    clock_gpio_init(PIN_CLKIN, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, 15);       // 120 MHz sys clock / 15 = 8 MHz, in recommended range, datasheet pg. 7

    gpio_put(PIN_CS, 1);
    sleep_ms(10);
    ADC_WriteRegister(REG_MODE, ADC_CONFIG_MODE);
    ADC_WriteRegister(REG_CLOCK, ADC_CONFIG_CLOCK);
    sleep_ms(1);

    // Resync by pulsing shorter than t_W(RSL) (2048 * t_clkin) but longer than one clkin (125 ns @ 8 MHz) -> 10 us should be ok
    gpio_put(PIN_CS, 0);
    sleep_us(10);
    gpio_put(PIN_CS, 1);
}

uint16_t ADC_ReadRegister(uint8_t reg)
{
    ADC_SetCommand(COMMAND_READ(reg, 1));

    ADC_ExchangeData(len);

    ADC_SetCommand(COMMAND_NULL);
    ADC_ExchangeData(len);    

    return ADC_GetResponse();
}

bool ADC_WriteRegister(uint8_t reg, uint16_t val)
{
    ADC_SetCommand(COMMAND_WRITE(reg, 1));
    ADC_SetRegisterValue(val);

    ADC_ExchangeData(len);

    return true;
}


bool ADC_CheckData(void)
{
    if (gpio_get(PIN_DRDY)) return false;

    ADC_SetCommand(COMMAND_NULL);
    ADC_ExchangeData(len);
    ADC_ConvertResults();

    return true;
}


uint32_t ADC_GetRaw(size_t ch)
{
    return adc_data[ch];
}

/*double ADC_GetRatio(size_t ch)
{
    return adc_data[ch] / 8388608.0;
}*/

/*double ADC_GetResistance(size_t ch)
{
    double ratio = ADC_GetRatio(ch);
    return 5600.0 * ratio / (4.3 / 1.6 / 0.96 - ratio);
}*/

/*double ADC_GetTemperature(size_t ch)
{
    double q = 1 / B - ADC_GetResistance(ch) / (R_0 * B);

    // pq formula
    double temp = p / -2.0 - sqrt(p * p / 4.0 - q);
    return temp;
}*/
