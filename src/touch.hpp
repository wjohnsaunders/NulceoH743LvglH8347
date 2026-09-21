/**
 * @file touch.h
 *
 */

#pragma once

#include <cstdint>

void touch_init();
void touch_calibrate(uint16_t _min_x, uint16_t _max_x, uint16_t _min_y, uint16_t _max_y);
bool touch_get_coordinates(uint16_t* x, uint16_t* y);
