/**
 * @file hx8347.c
 *
 */

#include "hx8347.hpp"
#include "stm32h743xx.h"
#include "stm32h7xx_hal.h"
#include "main.h"

struct init_cmds
{
	uint8_t cmd;
	uint8_t data;
};

void TFT_GPIO_init()
{
    // Set initial pin states
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
}

static void TFT_reset()
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(15);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(15);
}

void TFT_write_bus(uint8_t value)
{
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, value & 0x80 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, value & 0x40 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, value & 0x20 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, value & 0x10 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, value & 0x08 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, value & 0x04 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, value & 0x02 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, value & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);
}

void TFT_write_cmd(uint8_t value)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    TFT_write_bus(value);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void TFT_write(uint8_t value)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    TFT_write_bus(value);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void TFT_write_data(uint16_t value)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    TFT_write_bus((uint8_t)(value >> 8));
    TFT_write_bus((uint8_t)value);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static const struct init_cmds hx_init_cmds[] =
{
	//set later after scroll init
	//{0x01, 0x08},	//scroll mode on
    {0x16, 0x40},	//set memory access normal
	{0x17, 0x05},	//COLMOD 16bit
	{0x18, 0x34},	//frame rate idle/normal 50Hz/60Hz
	{0x19, 0x01},	//enable oscillator
	{0x1f, 0xd4},	//set power on and exit standby mode
	{0x36, 0x00},	//characteristic SS, GS, BGR
	{0x28, 0x3c}	//gate output and display on
};

void lcd_init()
{
    TFT_GPIO_init();
    TFT_reset();

    HAL_Delay(60);

    unsigned int i;
    for (i = 0; i < (sizeof(hx_init_cmds) / sizeof(hx_init_cmds[0])); ++i)
    {
	    TFT_write_cmd(hx_init_cmds[i].cmd);
	    TFT_write(hx_init_cmds[i].data);
    }
}

void lcd_set_address_window(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    TFT_write_cmd(0x02);	//col start
    TFT_write(x1>>8);
    TFT_write_cmd(0x03);
    TFT_write(x1);

    TFT_write_cmd(0x04);	//col end
    TFT_write(x2>>8);
    TFT_write_cmd(0x05);
    TFT_write(x2);

    TFT_write_cmd(0x06);	//row start
    TFT_write(y1>>8);
    TFT_write_cmd(0x07);
    TFT_write(y1);

    TFT_write_cmd(0x08);	//row end
    TFT_write(y2>>8);
    TFT_write_cmd(0x09);
    TFT_write(y2);
}

void lcd_transmit_rgb565_data(uint16_t* rgb565_bitmap, uint32_t num_pixels)
{
    TFT_write_cmd(0x22);	//GRAM
    while (num_pixels != 0)
    {
       TFT_write_data(*rgb565_bitmap++);
       --num_pixels;
    };
}
