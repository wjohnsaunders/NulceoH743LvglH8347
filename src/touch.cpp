/**
 * @file touch.c
 *
 */

#include "touch.hpp"
#include "stm32h743xx.h"
#include "stm32h7xx_hal.h"
#include "main.h"
#include "adc.h"

static uint16_t min_x = 305;
static uint16_t max_x = 2545;
static uint16_t min_y = 448;
static uint16_t max_y = 3824;

static void set_pin_as_adc(GPIO_TypeDef* port, uint32_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void set_pin_as_gpio_out(GPIO_TypeDef* port, uint32_t pin, GPIO_PinState initial_state)
{
    // Set initial state before driving the pin to avoid glitches
    HAL_GPIO_WritePin(port, pin, initial_state);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void set_pin_as_gpio_hiz(GPIO_TypeDef* port, uint16_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

uint32_t read_adc_channel(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    uint32_t adc_value = 0;

    // 1. Configure the desired channel into Rank 1 (Single Channel Mode)
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;          // Set as the first/only channel in the sequence
    sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5; // STM32H7 needs generous sampling times depending on source impedance
    sConfig.SingleDiff = ADC_SINGLE_ENDED;       // Default single-ended analog input
    sConfig.OffsetNumber = ADC_OFFSET_NONE;      // No calibration offset manipulation needed
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        // Configuration Error Handling
        Error_Handler(); 
    }

    // 2. Start the conversion
    if (HAL_ADC_Start(&hadc3) != HAL_OK)
    {
        // Start Error Handling
        Error_Handler();
    }

    // 3. Poll for the conversion to finish (Timeout set to 10ms)
    if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK)
    {
        // 4. Retrieve the converted 16-bit value (STM32H7 has up to 16-bit ADC resolution)
        adc_value = HAL_ADC_GetValue(&hadc3);
    }

    // 5. Stop the conversion to cleanly unlock the register state for the next switch
    HAL_ADC_Stop(&hadc3);

    return adc_value;
}

static uint32_t touch_read_x()
{
    // Create a voltage gradiant across the X-axis by setting LCD_RS to high and LCD_D0 to low
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_RESET);    // X- = low
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);      // X+ = high
    set_pin_as_gpio_hiz(LCD_D1_GPIO_Port, LCD_D1_Pin);                  // Y- = Hi-Z
    set_pin_as_adc(LCD_CS_GPIO_Port, LCD_CS_Pin);                       // Y+ = ADC input

    // Read the analog value from the X-axis pin (LCD_CS)
    uint32_t raw_x = read_adc_channel(ADC_CHANNEL_5);

    // Change LCD_CS and LCD_D1 to a digital output again
    set_pin_as_gpio_out(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    set_pin_as_gpio_out(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_RESET);

    return raw_x;
}

static uint32_t touch_read_y()
{
    // Create a voltage gradiant across the Y-axis by setting LCD_CS to high and LCD_D1 to low
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_RESET);    // Y- = low
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);      // Y+ = high
    set_pin_as_gpio_hiz(LCD_D0_GPIO_Port, LCD_D0_Pin);                  // X- = Hi-Z
    set_pin_as_adc(LCD_RS_GPIO_Port, LCD_RS_Pin);                       // X+ = ADC input

    // Read the analog value from the Y-axis pin (LCD_RS)
    uint32_t raw_y = read_adc_channel(ADC_CHANNEL_1);

    // Change LCD_RS and LCD_D0 to a digital output again
    set_pin_as_gpio_out(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    set_pin_as_gpio_out(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_RESET);

    return raw_y;
}

void touch_init()
{
    uint32_t raw_low = 0, raw_high = 0;

    printf("Performing touch self-test...\n\r");
    set_pin_as_adc(LCD_CS_GPIO_Port, LCD_CS_Pin);
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_RESET);
    raw_low = read_adc_channel(ADC_CHANNEL_5);
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_SET);
    raw_high = read_adc_channel(ADC_CHANNEL_5);
    set_pin_as_gpio_out(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    if (raw_low < 200 && raw_high > 3900)
    {
        printf("Touch self-test X-axis passed! Raw low: %lu, Raw high: %lu\n\r", raw_low, raw_high);
    }
    else
    {
        printf("Touch self-test X-axis failed! Raw low: %lu, Raw high: %lu\n\r", raw_low, raw_high);
        Error_Handler();
    }

    set_pin_as_adc(LCD_RS_GPIO_Port, LCD_RS_Pin);
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_RESET);
    raw_low = read_adc_channel(ADC_CHANNEL_1);
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_SET);
    raw_high = read_adc_channel(ADC_CHANNEL_1);
    set_pin_as_gpio_out(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    if (raw_low < 200 && raw_high > 3900)
    {
        printf("Touch self-test Y-axis passed! Raw low: %lu, Raw high: %lu\n\r", raw_low, raw_high);
    }
    else
    {
        printf("Touch self-test Y-axis failed! Raw low: %lu, Raw high: %lu\n\r", raw_low, raw_high);
        Error_Handler();
    }
}

void touch_calibrate(uint16_t _min_x, uint16_t _max_x, uint16_t _min_y, uint16_t _max_y)
{
    min_x = _min_x;
    max_x = _max_x;
    min_y = _min_y;
    max_y = _max_y;
}

bool touch_get_coordinates(uint16_t* x, uint16_t* y)
{
    uint32_t raw_x, raw_y;

    raw_x = touch_read_x();
    raw_y = touch_read_y();

    // printf("Touch coordinates: (0x%lx, 0x%lx)\n\r", raw_x, raw_y);

    if (raw_x >= min_x && raw_x <= max_x && raw_y >= min_y && raw_y <= max_y)
    {
        *x = 240 - ((raw_x - min_x) * 240 / (max_x - min_x));
        *y = 320 - ((raw_y - min_y) * 320 / (max_y - min_y));
        // printf("Touch coordinates: (%hu, %hu)\n\r", *x, *y);
        return true;
    }

    return false;
}
