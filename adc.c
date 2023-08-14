#include "adc.h"
#include "fft_imp.h"

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

uint8_t tx_data[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t rx_data[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint32_t adc_data[4];
uint32_t ADC_CollData[N];
int len = 24;

//static const uint8_t REG_DEVID = 0x00;
//static const uint8_t REG_STATUS = 0x01;
static const uint8_t REG_MODE = 0x02;
static const uint8_t REG_CLOCK = 0x03;
static const uint8_t REG_STATUS = 0x01;
static const uint8_t REG_THRSHLD_LSB = 0x08;
static const uint8_t REG_CH0_CFG = 0x09;

uint16_t ADC_CONFIG_CLOCK = 
        0b0000 << 12  |   // 15:12     Reserved
        0b1111 << 8  |   // 11:8    Enable all channels
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
        0b0   << 4  |   // 4        SPI Timeout enable
        0b01  << 2  |   // 3:2      DRDY pin signal source selection - 01b = the first one
        0b0   << 1  |   // 1        DRDY pin state when conversion data is not available
        0b0;            // 0        DRDY signal format when conversion data is available

uint16_t ADC_CONFIG_THRSHLD_LSB = 
        0b00000000  << 8 |   // 15:8    Reserved
        0b0000 << 4  |   // 7:4      Reserved
        0b0000;          // 0        DC block filter disabled

uint16_t ADC_CONFIG_CH0_CFG = 
        0x0000000000  << 6  |   // 15:6     Chanel 0 phase delay
        0b000   << 3  |   // 5:3        Reserved
        0b1 << 2  |   // 2      Disable DC Block filter
        0b10;           // 1:0      Chanel 0 input selection - positive DC test signal

static void ADC_SetCommand(uint16_t command)
{
    printf("ADC_SetCommand started \n");
    tx_data[0] = command >> 8 & 0xff;
    tx_data[1] = command & 0xff;
    printf("Command set \n");
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
    printf("ADC_ExchangeData started \n");
    gpio_put(PIN_CS, 0);       // 8,33 ns per instruction vs. 16 ns min delay
    sleep_us(1);
    printf("PIN_CS set 0 \n");
    spi_write_read_blocking(spi1, tx_data, rx_data, len);
    printf("Data exchanged \n");
    sleep_us(1);
    gpio_put(PIN_CS, 1);
    printf("PIN_CS set 1 \n");
}

bool ADC_ConvertResults(void)
{
    printf("ADC_ConvertResults started \n");
    uint8_t* offset = rx_data + 3;
    for (int ch = 0; ch < 4; ch++)
    {
        adc_data[ch] = int24(offset);
        offset +=3;
        printf("Channel %d", ch, "included \n");
    }
    printf("All Channels included \n");
    return true;
}


void ADC_Init(void)
{
    spi_init(spi1, 11520); 
    // Start external clock for ADC
    clock_gpio_init(PIN_CLKIN, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, 15);       // 120 MHz sys clock / 15 = 8 MHz, in recommended range, datasheet pg. 7

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

    gpio_put(PIN_CS, 1);
    sleep_ms(10);
    ADC_WriteRegister(REG_MODE, ADC_CONFIG_MODE);
    ADC_WriteRegister(REG_CLOCK, ADC_CONFIG_CLOCK);
    //ADC_WriteRegister(REG_THRSHLD_LSB, ADC_CONFIG_THRSHLD_LSB); //disable DC-Block Filter
    ADC_WriteRegister(REG_CH0_CFG, ADC_CONFIG_CH0_CFG); //set channel 0 input selection to positive DC test signal
    sleep_ms(1);
    printf("Registers Set \n");
    // Resync by pulsing shorter than t_W(RSL) (2048 * t_clkin) but longer than one clkin (125 ns @ 8 MHz) -> 10 us should be ok
    uint32_t status = ADC_ReadRegister(REG_STATUS);
    printf("%u \n", status);
    gpio_put(PIN_CS, 0);
    sleep_us(10);
    gpio_put(PIN_CS, 1);
    printf("Reset Done \n");
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
{   printf("CheckData stated \n");
    if (gpio_get(PIN_DRDY) == true) {
        printf("DRDY true - no data \n");
        return false;
    }
    else {
    printf("DRDY false - get data \n");
    ADC_SetCommand(COMMAND_NULL);
    ADC_ExchangeData(len);
    ADC_ConvertResults();
    printf("Results converted \n");
    return true;
    }
}


uint32_t ADC_GetRaw(size_t ch)
{
    printf("ADC_GetRaw started \n");
    return adc_data[ch];
}

uint32_t ADC_CollectData(int amount)
{   
    for (int i = 0; i < amount; i++)
    {
        int32_t x = 0;
        while (gpio_get(PIN_DRDY) == true || x < 625000000)
        {
            //wait for new data
            //printf("DRDY true - no new data \n");
            x++ ;
        }
        if (gpio_get(PIN_DRDY) == true) { 
            ADC_SetCommand(COMMAND_NULL);
            ADC_ExchangeData(len);
            ADC_ConvertResults();
            ADC_CollData[i] = ADC_GetRaw(0);
        } else { //if the while loop ended due to x reaching 625000000, print an error message.
            printf("Timeout - no new data in the last 5 seconds. \n");
            i = amount;
        }
    }
    return ADC_CollData[amount];
    
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
