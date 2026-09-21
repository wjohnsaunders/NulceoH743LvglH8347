/**
 * @file hx8347.h
 *
 */

#pragma once

#include <cstdint>

void lcd_init();
void lcd_set_address_window(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
void lcd_transmit_rgb565_data(uint16_t* rgb565_bitmap, uint32_t num_pixels);
